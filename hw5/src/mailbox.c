#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "debug.h"
#include "mailbox.h"
#include "protocol.h"
#include "help.h"

MAILBOX* mb_init(char* handle){
    debug("stating mailbox service for: %s", handle);
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
    Sem_init(&mailbox->lock, 0, 1);
    Sem_init(&mailbox->send, 0, 1);
    Sem_init(&mailbox->items, 0, 0);
    mailbox->handle = p_handle;
    mailbox->ref_count = 0;
    mb_ref(mailbox, "newly created mailbox");
    mailbox->hook = NULL;
    mailbox->next = NULL;
    mailbox->last = NULL;
    mailbox->defunct = 0;
    return mailbox;
}

void mb_set_discard_hook(MAILBOX* mb, MAILBOX_DISCARD_HOOK* hook){
    mb->hook = hook;
}

void mb_ref(MAILBOX* mb, char* why){
    P(&(mb->lock));
    debug("Increase reference count on mailbox (%s) %p (%d -> %d) for %s", mb->handle, mb,
		mb->ref_count, mb->ref_count + 1, why);
    mb->ref_count++;
    V(&(mb->lock));
}

void mb_unref(MAILBOX* mb, char* why){
    P(&(mb->lock));
    debug("Decrease reference count on mailbox (%s) %p (%d -> %d) for %s", mb->handle, mb,
		mb->ref_count, mb->ref_count - 1, why);
    mb->ref_count--;
    if(mb->ref_count == 0){
        MB_NODE* loc = mb->next;
        if(loc != NULL)
            mb->next = loc->next;
        while(loc != NULL){
            if(loc->entry->type == MESSAGE_ENTRY_TYPE)
                free(loc->entry->content.message.body);
            free(loc->entry);
            free(loc);
            loc = mb->next;
            if(loc == NULL)
                mb->next = NULL;
        }
        V(&(mb->lock));
        sem_destroy(&mb->lock);
        sem_destroy(&mb->items);
        sem_destroy(&mb->send);
        free(mb->handle);
        free(mb);
        return;
    }
    V(&mb->lock);
}

void mb_shutdown(MAILBOX* mb){
    debug("Shut down mailbox: %s", mb->handle);
    mb->defunct = 1;
    V(&mb->items);
}

char* mb_get_handle(MAILBOX* mb){
    if(mb == NULL)
        return NULL;
    return mb->handle;
}

void mb_add_message(MAILBOX* mb, int msgid, MAILBOX* from, void* body, int length){
    debug("mb_add_message()");
    P(&(mb->send)); 
    MAILBOX_ENTRY* mb_entry = malloc(sizeof(MAILBOX_ENTRY));
    mb_entry->type = MESSAGE_ENTRY_TYPE;
    mb_entry->content.message.msgid = msgid;
    mb_entry->content.message.from = from;
    mb_entry->content.message.body = body;
    mb_entry->content.message.length = length;
    MB_NODE* node = malloc(sizeof(MB_NODE));
    node->next = NULL;
    node->entry = mb_entry;
    if(mb != from)
        mb_ref(from, "being added in mb_add_message");
    if(mb->next == NULL){
        mb->next = node;
        mb->last = node;
    }
    else{
        mb->last->next = node;
        mb->last = node;
    }
    V(&(mb->send));
    V(&(mb->items));
}

void mb_add_notice(MAILBOX* mb, NOTICE_TYPE ntype, int msgid){
    debug("mb_add_message()");
    P(&mb->send);
    MAILBOX_ENTRY* mb_entry = malloc(sizeof(MAILBOX_ENTRY));
    if(mb_entry == NULL)
        return;
    mb_entry->type = NOTICE_ENTRY_TYPE;
    mb_entry->content.notice.type = ntype;
    mb_entry->content.notice.msgid = msgid;
    MB_NODE*node = malloc(sizeof(MB_NODE));
    node->next = NULL;
    node->entry = mb_entry;
    if(mb->next == NULL){
        mb->next = node;
        mb->last = node;
    }
    else{
        mb->last->next = node;
        mb->last = node;
    }
    V(&mb->send);
    V(&mb->items);
}

MAILBOX_ENTRY* mb_next_entry(MAILBOX* mb){
    debug("mb_next_entry()");
    P(&mb->items);
    if(mb->defunct == 1)
        return NULL;
    P(&mb->send);
    MAILBOX_ENTRY* temp = mb->next->entry;
    MB_NODE* node = mb->next;
    mb->next = mb->next->next;
    free(node);
    if(mb->next == NULL)
        mb->last = NULL;
    if(temp->type == MESSAGE_ENTRY_TYPE){
        if(mb != temp->content.message.from)
            mb_unref(&(*(temp->content.message).from), "removed from mb_next_entry()");
    }
    V(&mb->send);
    return temp;
}