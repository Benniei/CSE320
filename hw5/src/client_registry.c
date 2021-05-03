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

CLIENT_REGISTRY* creg_init(){
    return NULL;
}

void creg_fini(CLIENT_REGISTRY* cr){

}

CLIENT* creg_register(CLIENT_REGISTRY* cr, int fd){
    return NULL;
}

int creg_unregister(CLIENT_REGISTRY* cr, CLIENT* client){
    return -1;
}

CLIENT** creg_all_clients(CLIENT_REGISTRY *cr){
    return NULL;
}

void creg_shutdown_all(CLIENT_REGISTRY* cr){

}