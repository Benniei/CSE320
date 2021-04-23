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
#include "help.h"
#include "globals.h"
#include "csapp.h"

USER* user_create(char* handle){
	//fprintf(stderr,"\n\n\nyessir\n\n");
	return NULL;
}

USER* user_ref(USER* user, char* why){
	return NULL;
}

void user_unref(USER* user, char* why){

}

char* user_get_handle(USER* user){
	return NULL;
}