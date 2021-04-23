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
#include "help.h"
#include "csapp.h"

int proto_send_packet(int fd, CHLA_PACKET_HEADER *hdr, void *payload){
	printf("proto_send_packet\n");
	printf("File descriptor: %d\n", fd);
	return 0;
}

int proto_recv_packet(int fd, CHLA_PACKET_HEADER *hdr, void **payload){
	printf("proto_recv_packet\n");
	printf("File descriptor: %d\n", fd);
	return 0;
}