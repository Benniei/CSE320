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
	// fprintf(stderr, "send packet\n");
	if(rio_writen(fd, hdr, sizeof(CHLA_PACKET_HEADER)) < 0){
		debug("Send Packet Failed [Interrupt]\n");
		// set errno
		return -1;
	}
	if(hdr->payload_length > 0){
		if(rio_writen(fd, payload, hdr->payload_length) < 0){
			debug("Send Packet Failed [Interrupt]\n");
			// set errno
			return -1;
		}
	}
	return 0;
}

int proto_recv_packet(int fd, CHLA_PACKET_HEADER *hdr, void **payload){
	// fprintf(stderr, "recieve packet\n");
	if(rio_readn(fd, hdr, sizeof(CHLA_PACKET_HEADER)) < 0){
		debug("Recieve Packet Failed [Interrupt]\n");
		// set errno
		return -1;
	}
	if(hdr->payload_length > 0){
		uint32_t hbo = ntohl(hdr->payload_length);
		//fprintf(stderr, "down here %d\n", hbo);
		*payload = malloc(hbo);
		if(read(fd, *payload, hbo) <= 0){
			debug("Recieve Packet Failed [Interrupt]\n");
			// set errno
			return -1;
		}
		fprintf(stderr, "payload: %s", (char*)*payload);
	}
	return 0;
}