#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "debug.h"
#include "globals.h"
#include "protocol.h"
#include "user.h"
#include "mailbox.h"
#include "client_registry.h"
#include "csapp.h"
#include "help.h"



CLIENT *client_create(CLIENT_REGISTRY *creg, int fd){
    CLIENT* client = calloc(1, sizeof(CLIENT));
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
    client->msgid = 0;
    client->log = 1;
    client->tid = pthread_self();
    Sem_init(&client->mutex, 0, 1);
    Sem_init(&client_mutex, 0, 1);
    Sem_init(&network_mutex, 0, 1);
    client_ref(client, "newly created client");
    return client;
}

CLIENT *client_ref(CLIENT *client, char *why){
    P(&client->mutex);
    debug("Increase reference count on client(%d) %p (%d -> %d) for %s",client->fd, client, 
		client->ref_count, client->ref_count + 1, why);
    client->ref_count = client->ref_count + 1;
    V(&client->mutex);
    return client;
}

void client_unref(CLIENT *client, char *why){
    P(&client->mutex);
    debug("Decrease reference count on client(%d) %p (%d -> %d) for %s", client->fd, client, 
		client->ref_count, client->ref_count - 1, why);
    client->ref_count = client->ref_count - 1;
    V(&client->mutex);
    if(client->ref_count == 0){
        // freeing things 
        // not sure if need to free mailbox 
        if(client->state == 1){
            while(client->user->ref_count > 1)
                user_unref(client->user, "reference being removed from now-logged-out client");
            user_unref(client->user, "referernce being removed from now-logged-out client");
        }
        //creg_unregister(client_registry, client);
        debug("Free client %p", client);
        sem_destroy(&client->mutex);
        free(client);
    }
}

int client_login(CLIENT *client, char *handle){
    debug("client_login()");
    P(&client_mutex);
    if(client->state == 1){
        return -1;
    }
    USER* user;
    if((user = ureg_register(user_registry, handle)) == NULL){
        debug("CLIENT LOGIN ERORR from client_login()");
        V(&client_mutex);
        return -1;
    }
    if(user->ref_count != 2){
        debug("Client already logged in from client_login()");
        V(&client_mutex);
        return -1;
    }
    client->user = user;
    debug("client login() -> mb_init() [handle: %s]", handle);
    client->mailbox = mb_init(handle); 
    client->state = 1;
    debug("Log in client %p as user %p [%s] with mailbox %p", client, client->user, handle, client->mailbox);
    V(&client_mutex); 
    return 0;
}

int client_logout(CLIENT *client){
    debug("Logout client %p", client);
    P(&client_mutex);
    if(client->state == 0){
        V(&client_mutex);
        return -1;
    }
    mb_shutdown(client->mailbox);
    client->state = 0;
    user_unref(client->user, "reference being discared by terminating client service thread");
    mb_unref(client->mailbox, "--reference being discarded by terminating mailbox service thread");
    client->user = NULL;
    client->mailbox = NULL;
    debug("End client_logout()");
    V(&client_mutex);
    return 0;
}

USER *client_get_user(CLIENT *client, int no_ref){
    P(&client->mutex);
    if(client->state == 0){
        V(&client->mutex);
        return NULL;
    }
    if(no_ref == 0)
        user_ref(client->user, "refernced by client_get_user()");
    V(&client->mutex);
    return client->user;
}

MAILBOX *client_get_mailbox(CLIENT *client, int no_ref){
    P(&client->mutex);
    if(client->state == 0){
        V(&client->mutex);
        return NULL;
    }
    if(no_ref == 0)
        mb_ref(client->mailbox, "referenced by client_get_mailbox()");
    V(&client->mutex);
    return client->mailbox;
}

int client_get_fd(CLIENT *client){
    if(client == NULL)
        return -1;
    return client->fd;
}

int client_send_packet(CLIENT *user, CHLA_PACKET_HEADER *pkt, void *data){
    P(&network_mutex);
    debug("client_send_packet (%d): %d\n", pkt->type, (pkt->payload_length));
    if(proto_send_packet(user->fd, pkt, data) == -1){
        debug("Client_send_packet() failed to send to client %d", user->fd);
        V(&network_mutex);
        return -1;
    }
    V(&network_mutex);
    return 0;
}

int client_send_ack(CLIENT *client, uint32_t msgid, void *data, size_t datalen){
    P(&network_mutex);
    CHLA_PACKET_HEADER* header = calloc(1, sizeof(CHLA_PACKET_HEADER));
    header->type = CHLA_ACK_PKT;
    header->payload_length = ntohl(datalen);
    header->msgid = ntohl(msgid);
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);
    header->timestamp_sec = timestamp.tv_sec;
    header->timestamp_nsec = timestamp.tv_nsec;
    debug("Send ACK packet (clientfd = %d, msgid = %d)", client->fd,msgid);
    if(proto_send_packet(client->fd, header, data) == -1){
        debug("Client_send_ack() failed to send to client %d", client->fd);
        V(&network_mutex);
        return -1;
    }
    free(header);
    V(&network_mutex);
    return 0;
}

int client_send_nack(CLIENT *client, uint32_t msgid){
    P(&network_mutex);
    CHLA_PACKET_HEADER* header = calloc(1, sizeof(CHLA_PACKET_HEADER));
    header->type = CHLA_NACK_PKT;
    header->msgid = ntohl(msgid);
    header->payload_length = ntohl(0);
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);
    header->timestamp_sec = timestamp.tv_sec;
    header->timestamp_nsec = timestamp.tv_nsec;
    debug("Send NACK packet (clientfd = %d, msgid = %d)", client->fd,msgid);
    if(proto_send_packet(client->fd, header, NULL) == -1){
        debug("Client_send_nack() failed to send to client %d", client->fd);
        V(&network_mutex);
        return -1;
    }
    free(header);
    V(&network_mutex);
    return 0;
}