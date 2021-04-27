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


int proto_send_packet(int fd, CHLA_PACKET_HEADER *hdr, void *payload){
	// fprintf(stderr, "send packet\n");
	if(rio_writen(fd, hdr, sizeof(CHLA_PACKET_HEADER)) < 0){
		debug("Send Packet Failed [header]\n");
		return -1;
	}
	if(hdr->payload_length != 0){
		uint32_t hbo = htonl(hdr->payload_length);
		if(rio_writen(fd, payload, hbo) < 0){
			fprintf(stderr, "payload length [send]: %d", hdr->payload_length);
			debug("Send Packet Failed [payload]\n");
			return -1;
		}
	}
	return 0;
}

int proto_recv_packet(int fd, CHLA_PACKET_HEADER *hdr, void **payload){
	// fprintf(stderr, "recieve packet\n");
	if(rio_readn(fd, hdr, sizeof(CHLA_PACKET_HEADER)) < 0){
		debug("Recieve Packet Failed [header]\n");
		return -1;
	}
	if(hdr->payload_length != 0){
		//uint32_t temp = hdr->payload_length;
		uint32_t hbo = ntohl(hdr->payload_length);;
		*payload = malloc(hbo + 1);
		if(rio_readn(fd, *payload, hbo) <= 0){
			debug("Recieve Packet Failed [payload]\n");
			return -1;
		}
	}
	return 0;
}