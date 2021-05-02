#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "debug.h"
#include "globals.h"
#include "protocol.h"
#include "user.h"
#include "mailbox.h"
#include "client_registry.h"
#include "csapp.h"
#include "help.h"

static sem_t client_mutex;
sem_t network_mutex;

CLIENT *client_create(CLIENT_REGISTRY *creg, int fd){
    CLIENT* client = malloc(sizeof(CLIENT));
    if(client == NULL){
        debug("client create malloc fail");
        return NULL;
    }
    debug("Starting client service for fd: %d\n", fd);
    client->fd = fd;
    client->ref_count = 0;
    client->state = 0;
    client->user = NULL;
    client->mailbox = NULL;
    client->msgid = 1;
    Sem_init(&client->mutex, 0, 1);
    Sem_init(&client_mutex, 0, 1);
    Sem_init(&network_mutex, 0, 1);
    client_ref(client, "newly created client");
    return client;
}

CLIENT *client_ref(CLIENT *client, char *why){
    P(&client->mutex);
    debug("Increase reference count on client %p (%d -> %d) for %s", client, 
		client->ref_count, client->ref_count + 1, why);
    client->ref_count = client->ref_count + 1;
    V(&client->mutex);
    return client;
}

void client_unref(CLIENT *client, char *why){
    P(&client->mutex);
    debug("Decrease reference count on client %p (%d -> %d) for %s", client, 
		client->ref_count, client->ref_count + 1, why);
    client->ref_count = client->ref_count - 1;
    if(client->ref_count == 0){
        // freeing things 
        // not sure if need to free mailbox 
        if(client->user != NULL)
            user_unref(client->user, "reference being removed from now-logged-out client");
        if(client->mailbox !== NULL)
            mailbox_unref(client->mailbox, "reference being removed from now-logged-out client");
        mb_shutdown(client->mailbox);
        free(client);
    }
    V(&client->mutex);
}

int client_login(CLIENT *client, char *handle){
    // TODO: CHECK IF USER IS LOGGED IN
    P(&client_mutex);
    if(client->state == 1){
        return -1;
    }
    USER* user;
    if((user = ureg_register(user_registry, handle)) == NULL){
        debug("CLIENT LOGIN ERORR from client_login()");
        client_send_nack(client, client->msgid++);
        return -1;
    }
    if(user->ref_count != 2){
        debug("Client already logged in from client_login()");
        client_send_nack(client, client->msgid++);
        return -1;
    }
    client->user = user;
    client->mailbox = mb_init(handle); 
    client->state = 1;
    debug("Log in client %p as user %p [%s] with mailbox %p", client, client->user, handle, client->mailbox);
    client_send_ack(client, client->msgid++, handle, strlen(handle) + 2);
    V(&client_mutex); 
    return 0;
}

int client_logout(CLIENT *client){
    P(&client_mutex);
    if(client->state == 0){
        return -1;
    }
    while(client->ref_count > 1)
        client_unref(client, )
    V(&client_mutex);
    return -1;
}

USER *client_get_user(CLIENT *client, int no_ref){
    P(&client->mutex);
    if(client->user == NULL)
        return NULL;
    if(no_ref == 0)
        user_ref(client->user, "refrenced by client_get_user()");
    V(&client->mutex);
    return client->user;
}

MAILBOX *client_get_mailbox(CLIENT *client, int no_ref){
    P(&client->mutex);
    if(client->mailbox == NULL)
        return NULL;
    if(no_ref == 0)
        mb_ref(client->mailbox, "referenced by client_get_mainbox()");
    V(&client->mutex);
    return client->mailbox;
}

int client_get_fd(CLIENT *client){
    return client->fd;
}

int client_send_packet(CLIENT *user, CHLA_PACKET_HEADER *pkt, void *data){
    fprintf(stderr, "send message\n");
    P(&network_mutex);
    if(proto_send_packet(user->fd, pkt, data) == -1){
        debug("Client_send_packet() failed to send to client %d", user->fd);
        return -1;
    }
    V(&network_mutex);
    return 0;
}

int client_send_ack(CLIENT *client, uint32_t msgid, void *data, size_t datalen){
    P(&network_mutex);
    CHLA_PACKET_HEADER* header = malloc(sizeof(CHLA_PACKET_HEADER));
    header->type = CHLA_ACK_PKT;
    header->payload_length = ntohl(datalen);
    header->msgid = ntohl(msgid);
    debug("Send ACK packet (clientfd = %d)", client->fd);
    if(proto_send_packet(client->fd, header, data) == -1){
        debug("Client_send_ack() failed to send to client %d", client->fd);
        return -1;
    }
    free(header);
    V(&network_mutex);
    return 0;
}

int client_send_nack(CLIENT *client, uint32_t msgid){
     P(&network_mutex);
    CHLA_PACKET_HEADER* header = malloc(sizeof(CHLA_PACKET_HEADER));
    header->type = CHLA_NACK_PKT;
    header->msgid = ntohl(msgid);
    if(proto_send_packet(client->fd, header, NULL) == -1){
        debug("Client_send_nack() failed to send to client %d", client->fd);
        return -1;
    }
    free(header);
    V(&network_mutex);
    return 0;
}