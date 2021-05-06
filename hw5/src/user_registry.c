#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "debug.h"
#include "server.h"
#include "csapp.h"
#include "user_registry.h"
#include "help.h"

USER_REGISTRY* ureg_init(void){
    debug("Initialize User Registry");
    USER_REGISTRY* head = malloc(sizeof(USER_REGISTRY));
    head->next = NULL;
    head->num_users = 0;
    Sem_init(&head->mutex, 0, 1);
    return head;
}

void ureg_fini(USER_REGISTRY* ureg){
    USER_REG_NODE* loc = ureg->next;
    USER_REG_NODE* temp;
    while(loc != NULL){
        
        temp = loc;
        loc = temp->next; 
        while(temp->user->ref_count > 1)
            user_unref(temp->user, "being removed from the now-logged-out client");   
        user_unref(temp->user, "being removed from the now-logged-out client");
        sem_destroy(&temp->mutex);
        free(temp);
    }
    sem_destroy(&temp->mutex);
    free(ureg);
}

USER* ureg_register(USER_REGISTRY* ureg, char* handle){
    // fprintf(stderr, "register user\n");
    debug("ureg_register()");
    USER_REG_NODE* prev_loc;
    USER_REG_NODE* loc = ureg->next;
    // lock 
    P(&ureg->mutex);
    prev_loc = NULL;
    loc = ureg->next;
    while(loc != NULL){
        if(strcmp(handle, loc->user->handle) == 0){
            user_ref(loc->user, "reference found in user_registry");
            V(&ureg->mutex);
            return loc->user;
        }
        prev_loc = loc;
        loc = loc->next;
    }
    debug("User with handle \'%s\' does not yet exist", handle);
    USER* new_user = user_create(handle); 
    USER_REG_NODE* new_node = malloc(sizeof(USER_REG_NODE));
    if(prev_loc == NULL)
        ureg->next = new_node;
    else
        prev_loc->next = new_node;
    new_node->user = new_user;
    new_node->next = NULL;
    ureg->num_users++;
    Sem_init(&new_node->mutex, 0, 1);
    user_ref(new_user, "reference being retained by user registry");
    // unlock
    loc = ureg->next;
    V(&ureg->mutex);
    return new_user;
}

void ureg_unregister(USER_REGISTRY* ureg, char* handle){
    USER_REG_NODE* loc = ureg->next;
    USER_REG_NODE* prev;
    // lock
    P(&ureg->mutex);
    loc = ureg->next;
    while(loc != NULL){
        if(strcmp(handle, loc->user->handle) == 0){
            if(prev == NULL)
                ureg->next = loc->next;
            else
                prev->next = loc->next;
            user_unref(loc->user, "reference being unregistered in user registry");
            ureg->num_users--;
            break;
        }
        prev = loc;
        loc = loc->next;
    }
    if(loc == NULL){
        debug("User not found [ureg_unregister]");
    }
    // unlock
    loc = ureg->next;
    V(&ureg->mutex);
}