#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "debug.h"
#include "protocol.h"
#include "client_registry.h"
#include "help.h"

void* chla_mailbox_service(void* arg){
    return NULL;
}

void* chla_client_service(void* arg){
    int fd = *(int *)arg;
    debug("Startiing Client Service for fd: %d", fd);
    CLIENT* client = creg_register(client_registry, fd);
    char* payload;
    CHLA_PACKET_HEADER header;
    while(1){
        if(proto_recv_packet(fd, &header, (void **)&payload) == 0){
            header.msgid = ntohl(header.msgid);
            
            if(header.type == CHLA_LOGIN_PKT){
                debug("LOGIN");
                payload[ntohl(header.payload_length) - 2] = '\0';
                if(client_login(client, payload) == 0){
                    client_send_ack(client, header.msgid, NULL, 0);
                }
                else
                    client_send_nack(client, header.msgid);
            }
            else if(header.type == CHLA_USERS_PKT){
                debug("USERS");
                CLIENT** active = creg_all_clients(client_registry);
                if(*active == NULL){
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
                while(*temp != NULL){
                    if((user = client_get_user(*temp, 1)) != NULL){
                        debug("-------> user_get_handle(user)");
                        strcat(handle_all, user_get_handle(user));
                        strcat(handle_all, ending);
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
                size_t cmd_size = strlen(payload) + 1; 
                char* payload_cpy = malloc(cmd_size);
                strcpy(payload_cpy, payload);

                char* recipient = strtok(payload_cpy, "\r\n");
                char* message = strtok(NULL, "\r\n");
                debug("Recipient: \'%s\' (message: %s)", recipient, message);
                CLIENT** active = creg_all_clients(client_registry);
                if(*active == NULL){
                    client_send_nack(client, header.msgid);
                    continue;
                }
                CLIENT** temp = active;
                MAILBOX* to;
                char flag = 0;
                while(*temp != NULL){
                    to = client_get_mailbox(*temp, 0);
                    if(strcmp(mb_get_handle(to), recipient) == 0){
                        flag = 1;
                        client_unref(*temp, "reference in clients list being discarded");
                        break;
                    }
                    mb_unref(to, "unneeded mailbox reference");
                    client_unref(*temp, "reference in clients list being discarded");
                    temp++;
                    
                }
                if(flag == 0)
                    client_send_nack(client, header.msgid);
                free(payload_cpy);
                
                mb_add_message(to, header.msgid, cmb, payload, strlen(payload));
                free(active);
            }
            else if(header.type == CHLA_LOGOUT_PKT){
                debug("LOGOUT");

                if(client_logout(client) == 0){
                    client_logout(client);
                    client_send_ack(client, header.msgid, NULL, 0);
                }
                else{
                    client_send_nack(client, header.msgid);
                }
            }
            if(payload != NULL)
                free(payload);
        }
        else{
            debug("LOGOUT ENTIRE CLIENT");
            if(client_get_user(client, 1) != NULL)
                client_logout(client);
            // wait for mailbox thread to termiante
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