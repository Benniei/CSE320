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

#include "csapp.h"
#include "globals.h"
#include "user_registry.h"
#include "client.h"
#include "protocol.h"
#include "user.h"
#include "mailbox.h"
#include "client_registry.h"

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
	int num_users;
	sem_t mutex;
}USER_REGISTRY;

typedef struct user_reg_node{
	USER* user;
	USER_REG_NODE* next;
	sem_t mutex;
}USER_REG_NODE;

/* Client Struct */
typedef struct client{
	int fd;
	int ref_count;
	char log;
	char state; // state of the client 0-> logout 1->login
	int msgid;
	USER* user;
	MAILBOX* mailbox;
	sem_t mutex;
}CLIENT;

/* Client Registry Struct */
typedef struct client_registry{
	int used;
	int fill;
	CLIENT* clients[MAX_CLIENTS];
	sem_t mutex;
	sem_t slots;
	sem_t items; 
}CLIENT_REGISTRY;

/* Mailbox Struct */
typedef struct mb_node MB_NODE;

typedef struct mailbox{
	char* handle;
	int ref_count;
	sem_t lock;
	MAILBOX_DISCARD_HOOK* hook;
	MB_NODE* next;
} MAILBOX;

typedef struct mb_node{
	MB_NODE* next;
}MB_NODE;