#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "debug.h"
#include "protocol.h"
#include "client_registry.h"
#include "mailbox.h"
#include "help.h"

void bounce(MAILBOX_ENTRY* entry){
    debug("bounce()");
    if(entry->type == NOTICE_ENTRY_TYPE)
        return;

    MAILBOX* from = entry->content.message.from;
    mb_add_notice(from, BOUNCE_NOTICE_TYPE, entry->content.message.msgid);
    mb_unref(from, "derefernence for not being used after bounce is sent");
    
}

void* chla_mailbox_service(void* arg){ 
    pthread_detach(pthread_self());
    CLIENT* client = (CLIENT*)arg;
    debug("Starting mailbox services for: %s (fd = %d)", client->user->handle, client->fd);
    client_ref(client, "reference being retained by mailbox service thread");
    MAILBOX* mailbox = client_get_mailbox(client, 0);
    V(&able_mutex);
    while(1){
        if(mailbox == NULL)
            break;
        if(mailbox->defunct == 1)
            break;
        MAILBOX_ENTRY* entry = mb_next_entry(mailbox);
        if(mailbox->defunct == 1)
                break;
        if(entry->type == MESSAGE_ENTRY_TYPE){
            debug("Process message (msgid: %d, from= \'%s\'", entry->content.message.msgid,entry->content.message.from->handle);
            CHLA_PACKET_HEADER* header = calloc(1, sizeof(CHLA_PACKET_HEADER));
            header->type = CHLA_MESG_PKT;
            header->payload_length = ntohl(entry->content.message.length);
            header->msgid = ntohl(entry->content.message.msgid);
            struct timespec timestamp;
            clock_gettime(CLOCK_REALTIME, &timestamp);
            header->timestamp_sec = ntohl(timestamp.tv_sec);
            header->timestamp_nsec = ntohl(timestamp.tv_nsec);
            if(client_send_packet(client, header, entry->content.message.body) == 0)
                mb_add_notice(mailbox, RRCPT_NOTICE_TYPE, entry->content.message.msgid);
            else   
                mb_add_notice(mailbox, BOUNCE_NOTICE_TYPE, entry->content.message.msgid);
            free(entry->content.message.body);
            free(entry);
            free(header);
        }
        else if(entry->type == NOTICE_ENTRY_TYPE){
            debug("Process message (msgid: %d, type= \'%d\'", entry->content.message.msgid, entry->content.notice.type);
            CHLA_PACKET_HEADER* header = calloc(1, sizeof(CHLA_PACKET_HEADER));
            if(entry->content.notice.type == NO_NOTICE_TYPE){
                header->type = CHLA_NO_PKT;
            }
            else if(entry->content.notice.type == BOUNCE_NOTICE_TYPE){
                header->type = CHLA_BOUNCE_PKT;
            }
            else if(entry->content.notice.type == RRCPT_NOTICE_TYPE){
                header->type = CHLA_RCVD_PKT;
            }
            header->payload_length = ntohl(0);
            header->msgid = ntohl(entry->content.message.msgid);
            struct timespec timestamp;
            clock_gettime(CLOCK_REALTIME, &timestamp);
            header->timestamp_sec = ntohl(timestamp.tv_sec);
            header->timestamp_nsec = ntohl(timestamp.tv_nsec);
            if(client_send_packet(client, header, NULL) != 0)
                debug("Unable to send notice");
            free(entry);
            free(header);
        }
    }
    debug("Ending mailbox services for: %s (fd = %d)", client->user->handle, client->fd);
    mb_unref(mailbox, "reference being discarded by terminating mailbox service");
    client_unref(client, "reference being discarded by terminating mailbox service thread");
    V(&able_mutex);
    pthread_exit(NULL);
}

void* chla_client_service(void* arg){
    int fd = *(int *)arg;
    pthread_detach(pthread_self());
    debug("Startiing Client Service for fd: %d", fd);
    CLIENT* client = creg_register(client_registry, fd);
    char* payload;
    
    pthread_t tid;
    
    while(1){
        CHLA_PACKET_HEADER header;
        if(proto_recv_packet(fd, &header, (void **)&payload) == 0){
            header.msgid = ntohl(header.msgid);
            
            if(header.type == CHLA_LOGIN_PKT){
                debug("LOGIN");
                payload[ntohl(header.payload_length) - 2] = '\0';
                if(client_login(client, payload) == 0){
                    mb_set_discard_hook(client_get_mailbox(client, 1), bounce);
                    
                    
                    if(pthread_create(&tid, NULL, chla_mailbox_service, client)){
                        goto fail;
                    }
                    P(&able_mutex);
                    client_send_ack(client, header.msgid, NULL, 0);
                }
                else
                    client_send_nack(client, header.msgid);
                if(payload != NULL)
                    free(payload);
            }
            else if(header.type == CHLA_USERS_PKT){
                debug("USERS");
                CLIENT** active = creg_all_clients(client_registry);
                if(*active == NULL){
                    free(active);
                    client_send_ack(client, header.msgid, NULL, 0);
                    continue;
                }
                CLIENT** temp = active;
                USER* user;
                size_t size = 0;
                int ct = 0;
                while(*temp != NULL){
                    if((user = client_get_user(*temp, 1)) != NULL){
                        size += strlen(user_get_handle(user));
                        ct++;
                    }
                    temp++;
                }
                size = size + (2*ct);
                char* handle_all = malloc(size + 1);

                temp = active;
                char* ending = "\r\n";
                char cd = 0;
                while(*temp != NULL){
                    if((user = client_get_user(*temp, 1)) != NULL){
                        debug("-------> user_get_handle(user)");
                        char* u_handle = user_get_handle(user);
                        if(cd == 0){
                            strcpy(handle_all, u_handle);
                            strcat(handle_all, ending);
                            cd++;
                        }
                        else{
                            strcat(handle_all, u_handle);
                            strcat(handle_all, ending);
                        }
                    }
                    client_unref(*temp, "reference being discarded for client being unregistered");
                    temp++;
                }
                client_send_ack(client, header.msgid, handle_all, size);
                free(handle_all);
                free(active);
            }
            else if(header.type == CHLA_SEND_PKT){
                debug("SEND");
                MAILBOX* cmb = client_get_mailbox(client, 1);
                char* payload_cpy;
                int cmd_size = ntohl(header.payload_length);  
                char* recipient;
                char* message;
                int newsize;
                if(payload != NULL){
                    newsize = strlen(client->user->handle) + cmd_size;
                    recipient = strtok(payload, "\r\n");
                    newsize -= strlen(recipient);
                    payload_cpy = malloc(newsize + 1) ;
                    strcpy(payload_cpy, client->user->handle);
                    strcat(payload_cpy, "\r\n");
                    message = strtok(NULL, "\r\n");
                    strcat(payload_cpy, message);
                    debug("size: %d", newsize);
                }
                  
                debug("Recipient: \'%s\'", recipient);
                CLIENT** active = creg_all_clients(client_registry);
                if(*active == NULL){
                    client_send_nack(client, header.msgid);
                    continue;
                }
                CLIENT** temp = active;
                MAILBOX* to;
                MAILBOX* found;
                char flag = 0;
                while(*temp != NULL){
                    to = client_get_mailbox(*temp, 0);
                    if(strcmp(mb_get_handle(to), recipient) == 0){
                        if(flag == 0)
                            found = client_get_mailbox(*temp, 1);
                        flag = 1;
                        client_unref(*temp, "reference in clients list being discarded");
                        temp++;
                        continue;
                    }
                    mb_unref(to, "unneeded mailbox reference");
                    client_unref(*temp, "reference in clients list being discarded");
                    temp++;
                    
                }
                if(flag == 0){
                    debug("user %s not found", recipient);
                    free(active);
                    free(payload);
                    if(payload != NULL)
                        free(payload);
                    client_send_nack(client, header.msgid);
                    continue;
                }
                free(payload);
  
                mb_add_message(found, header.msgid, cmb, payload_cpy, newsize);
                mb_unref(found, "message has been added to recipient's mailbox");
                client_send_ack(client, header.msgid, NULL, 0);
                free(active);
            }
            else if(header.type == CHLA_LOGOUT_PKT){
                debug("LOGOUT");

                if(client_logout(client) == 0){
                    client_logout(client);
                    client_send_ack(client, header.msgid, NULL, 0);
                    P(&client_registry->mutex);
                    debug("Waiting for mailbox service thread (%ld) to terminate", tid);
                    P(&able_mutex);
                    V(&client_registry->mutex);
                }
                else{
                    client_send_nack(client, header.msgid);
                }
            }
            else{
                continue;
            }
        }
        else{
            fail:
            debug("LOGOUT ENTIRE CLIENT");
            if(client->state == 1){
                client_logout(client);
                debug("Waiting for mailbox service thread (%ld) to terminate", tid);
            }
            client_unref(client, "reference being discarded by termianting client service thread");
            creg_unregister(client_registry, client);
            debug("Ending client service for fd: %d", fd);
            free(arg);
            close(fd);
            pthread_exit(NULL);
        }
    }
    return NULL;
}