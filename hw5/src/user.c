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
	USER* new_user = malloc(sizeof(USER));
	if(new_user == NULL){
		debug("user_create malloc fail [new_user]\n");
		return NULL;
	}
	char* p_handle = malloc(strlen(handle) + 1);
	if(p_handle == NULL){
		debug("user_create malloc fail [p_handle]\n");
		return NULL;
	}
	strcpy(p_handle, handle);
	new_user->handle = p_handle;
	Sem_init(&new_user->mutex, 0, 1); /* Binary semaphore for locking ref_count*/
	new_user->ref_count = 0; 
	// printf("before debug\n");
	user_ref(new_user, "newly created user");
	return new_user;
}

USER* user_ref(USER* user, char* why){
	P(&user->mutex);
	debug("Increase reference count on user %p [%s] (%d -> %d) for %s", user, user->handle,
		user->ref_count, user->ref_count + 1, why);
	user->ref_count = user->ref_count + 1;
	V(&user->mutex);
	return user;
}

void user_unref(USER* user, char* why){
	P(&user->mutex);
	if(user->ref_count == 0){
		debug("Unref error: User %p [%s] has reference count of 0", user, user->handle);
		V(&user->mutex);
		return;
	}
	debug("Decrease reference count on user %p [%s] (%d -> %d) for %s", user, user->handle,
		user->ref_count, user->ref_count - 1, why);
	user->ref_count = user->ref_count - 1;
	if(user->ref_count == 0){
		free(user->handle);
		free(user);
	}
	V(&user->mutex);
}

char* user_get_handle(USER* user){
	return user->handle;
}