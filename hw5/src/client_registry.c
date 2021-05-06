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
#include <time.h>

#include "debug.h"
#include "globals.h"
#include "protocol.h"
#include "user.h"
#include "mailbox.h"
#include "csapp.h"
#include "help.h"

static sem_t cr_mutex;

CLIENT_REGISTRY* creg_init(){
    debug("Initialize client registry");
    CLIENT_REGISTRY* creg = malloc(sizeof(CLIENT_REGISTRY));
    creg->fill = 0;
    creg->used = 0;
    Sem_init(&creg->mutex, 0, 1);
    Sem_init(&cr_mutex, 0, 1);
    return creg;
}

void creg_fini(CLIENT_REGISTRY* cr){
    free(cr);
}

CLIENT* creg_register(CLIENT_REGISTRY* cr, int fd){
    //debug("Starting client service for fd %d", fd);
    P(&cr->mutex);
    int loc;
    if(cr->fill == cr->used){
        loc = cr->fill;
        cr->fill++;
        cr->used++;
    }
    else{
        for(int i = 0; i < cr->fill; i++){
            if(cr->clients[i]->log == 0){
                loc = i;
                cr->used++;
                break;
            }
        }
    }
    debug("Register client fd %d (total connected: %d)", fd, cr->used);
    cr->clients[loc] = client_create(cr, fd);
    client_ref(cr->clients[loc], "client being returned by creg_register()");
    V(&cr->mutex);
    return cr->clients[loc];
}

int creg_unregister(CLIENT_REGISTRY* cr, CLIENT* client){
    P(&cr->mutex);
    int loc = -1;
    for(int i = 0; i < cr->fill; i++){
        if(cr->clients[i] == client){
            loc = i;
            break;
        } 
    }
    if(loc == -1){
        debug("Client %p not found", client);
        return -1;
    }
    client_unref(client, "reference being discarded for client being unregistered");
    if(loc == cr->fill - 1)
        cr->fill--;
    cr->used--;
    V(&cr->mutex);
    return 0;
}

CLIENT** creg_all_clients(CLIENT_REGISTRY *cr){
    CLIENT** clients = malloc((cr->used + 1) * sizeof(CLIENT*));
    int counter = 0;
    debug("amount %d", cr->used);
    for(int i = 0; i < cr->fill; i++){
        if(cr->clients[i] == NULL)
            continue;
        if(cr->clients[i]->state == 1){
            *(clients + counter) = cr->clients[i];
            counter++;
            client_ref(cr->clients[i], "ferenced by creg_all_clients()");
        }
    }
    *(clients + counter) = NULL;
    return clients;
}

void creg_shutdown_all(CLIENT_REGISTRY* cr){
    // int counter = 0;
    // pthread_t tid[cr->used];
    //CLIENT* clients[cr->used];
    P(&cr_mutex);
    for(int i = 0; i < cr->fill; i++){
        if(cr->clients[i] == NULL)
            continue;
        if(cr->clients[i]->log == 1){
            debug("Shutting down client %d with tid %ld", cr->clients[i]->fd, cr->clients[i]->tid);
            shutdown(cr->clients[i]->fd, SHUT_RDWR);
        }
    }
    sleep(1);
    V(&cr_mutex);
}