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

#include "user.h"
#include "csapp.h"

/* User Struct */
typedef struct user {
	char* handle;
	int ref_count;
	sem_t mutex; /* protects access to reference count*/
}USER;

/* User_registry Struct */
typedef struct user_reg_node USER_REG_NODE;

typedef struct user_registry{
	USER_REG_NODE* next;
	sem_t mutex;
}USER_REGISTRY;

typedef struct user_reg_node{
	USER* user;
	USER_REG_NODE* next;
	sem_t mutex;
}USER_REG_NODE;