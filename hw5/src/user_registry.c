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
#include "server.h"
#include "globals.h"
#include "csapp.h"
#include "user_registry.h"
#include "help.h"

USER_REGISTRY* ureg_init(void){
    USER_REGISTRY* head = malloc(sizeof(USER_REGISTRY));
    head->next = NULL;
    Sem_init(&head->mutex, 0, 1);
    debug("Initialize User Registry");
    return head;
}

void ureg_fini(USER_REGISTRY* ureg){
    USER_REG_NODE* loc = ureg->next;
    USER_REG_NODE* temp;
    while(loc != NULL){
        temp = loc;
        loc = temp->next;    debug("ureg");
        free(temp->user->handle);
        free(temp);
    }
    free(ureg);
}

USER* ureg_register(USER_REGISTRY* ureg, char* handle){
    USER_REG_NODE* loc = ureg->next;
    while(loc != NULL){
        if(strcmp(handle, loc->user->handle) == 0){
            return loc->user;
        }
        loc = loc->next;
    }
    USER* new_user = user_create(handle); 
    USER_REG_NODE* new_node = malloc(sizeof(USER_REG_NODE));
    loc = new_node;
    new_node->user = new_user;
    new_node->next = NULL;
    Sem_init(&new_node->mutex, 0, 1);
    user_ref(new_user, "reference being retained by user registry");
    return new_user;
}

void ureg_unregister(USER_REGISTRY* ureg, char* handle){
    USER_REG_NODE* loc = ureg->next;
    USER_REG_NODE* prev;
    while(loc != NULL){
        if(strcmp(handle, loc->user->handle) == 0){
            if(prev == NULL)
                ureg->next = loc->next;
            else
                prev->next = loc->next;
            user_unref(loc->user, "reference being unregistered in user registry");
            free(loc->user->handle);
            free(loc->user);
            break;
        }
        prev = loc;
        loc = loc->next;
    }
    if(loc == NULL){
        debug("User not found [ureg_unregister]");
    }
}