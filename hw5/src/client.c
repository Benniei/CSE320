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
#include "protocol.h"
#include "user.h"
#include "mailbox.h"
#include "client_registry.h"
#include "csapp.h"
#include "help.h"

CLIENT *client_create(CLIENT_REGISTRY *creg, int fd){
    CLIENT* client = malloc(sizeof(CLIENT));
    if(client == NULL){
        debug("client create malloc fail");
        return NULL;
    }
    client->fd = fd;
    client->ref_count = 0;
    client->state = 0;
    client->user = NULL;
    client->mailbox = NULL;
    Sem_init(&client->mutex, 0, 1);
    client_ref(client, "newly created client");
    return client;
}

CLIENT *client_ref(CLIENT *client, char *why){
    P(&client->mutex);
    debug("Increase reference count on client %p (%d -> %d) for %s", client, 
		client->ref_count, client->ref_count + 1, why);
    client->ref_count = client->ref_count + 1;
    V(&client->mutex);
    return NULL;
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
            user_unref(client->user, "client is shutting down");
        free(client);
    }
    V(&client->mutex);
}

int client_login(CLIENT *client, char *handle){
    return -1;
}

int client_logout(CLIENT *client){
    return -1;
}

USER *client_get_user(CLIENT *client, int no_ref){
    return NULL;
}

MAILBOX *client_get_mailbox(CLIENT *client, int no_ref){
    return NULL;
}

int client_get_fd(CLIENT *client){
    return -1;
}

int client_send_packet(CLIENT *user, CHLA_PACKET_HEADER *pkt, void *data){
    return -1;
}

int client_send_ack(CLIENT *client, uint32_t msgid, void *data, size_t datalen){
    return -1;
}

int client_send_nack(CLIENT *client, uint32_t msgid){
    return -1;
}