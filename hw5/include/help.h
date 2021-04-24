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

typedef struct user {
	char* handle;
	int fd;
	int ref_count;
	sem_t mutex; /* protects access to reference count*/
}USER;