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
#include "protocol.h"
#include "help.h"

MAILBOX* mb_init(char* handle){
    MAILBOX* mailbox = malloc(sizeof(MAILBOX));
    if(mailbox == NULL){
        debug("mb_init malloc fail [mailbox]\n");
		return NULL;
    }
    char* p_handle = malloc(strlen(handle) + 1);
    strcpy(p_handle, handle);
    if(p_handle == NULL){
        debug("mb_init malloc fail [p_handle]\n");
		return NULL;
    }
    pthread_mutex_init(&mailbox->lock, NULL);
    mailbox->ref_count = 0;
    mb_ref(mailbox, "newly created mailbox");
    mailbox->hook = NULL;
    mailbox->next = NULL;
    return mailbox;
}

void mb_set_discard_hook(MAILBOX* mb, MAILBOX_DISCARD_HOOK* hook){
    mb->hook = hook;
}

void mb_ref(MAILBOX* mb, char* why){

}

void mb_unref(MAILBOX* mb, char* why){

}

void mb_shutdown(MAILBOX* mb){

}

char* mb_get_handle(MAILBOX* mb){
    return NULL;
}

void mb_add_message(MAILBOX* mb, int msgid, MAILBOX* from, void* body, int length){

}

void mb_add_notice(MAILBOX* mb, NOTICE_TYPE ntype, int msgid){

}

MAILBOX_ENTRY* mb_next_entry(MAILBOX* mb){
    return NULL;
}