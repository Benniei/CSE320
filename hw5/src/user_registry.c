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

USER_REGISTRY* ureg_init(void){
    return NULL;
}

void ureg_fini(USER_REGISTRY* ureg){

}

USER* ureg_register(USER_REGISTRY* ureg, char* handle){
    return NULL;
}

void ureg_unregister(USER_REGISTRY* ureg, char* handle){

}