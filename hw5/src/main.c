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

static void terminate(int);

void handler(int sig){
    // Clean termination of the server
    terminate(EXIT_SUCCESS);
}
/*
 * "Charla" chat server.
 *
 * Usage: charla <port>=
 */
int main(int argc, char* argv[]){
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.
    int counter = 1; // counters the amount of variables being used;
    char* port;
    if(argc != 3){
        printf("USAGE: -p <port> (required)\n");
        exit(EXIT_FAILURE);
    }
    if(strcmp(*(argv + counter), "-p") == 0){
        counter++;
        if(**(argv + counter) == '-'){
            // Invalid port name
            printf("Error: Invalid port name\n");
            exit(EXIT_FAILURE);
        }
        printf("Portname: %s\n", *(argv + counter));
        port = *(argv + counter);
        counter++;
    }
    else{
        printf("USAGE: -p <port> (required)\n");
        exit(EXIT_FAILURE);
    }

    // Perform required initializations of the client_registry and
    // player_registry.
    user_registry = ureg_init();
    client_registry = creg_init();

    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function charla_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.

    // SIGHUP handler using sigaction
    int *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    int listenfd;

    Signal(SIGHUP, handler);

    if((listenfd = Open_listenfd(port)) < 0){
        terminate(EXIT_FAILURE);
    }
    while(1){
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA*) &clientaddr, &clientlen);
        Pthread_create(&tid, NULL, chla_client_service, connfdp);
    }
    // fprintf(stderr, "You have to finish implementing main() "
	   //  "before the server will function.\n");

    terminate(EXIT_FAILURE);
}

/*
 * Function called to cleanly shut down the server.
 */
static void terminate(int status) {
    // Shut down all existing client connections.
    // This will trigger the eventual termination of service threads.
    creg_shutdown_all(client_registry);

    // Finalize modules.
    creg_fini(client_registry);
    ureg_fini(user_registry);

    debug("%ld: Server terminating", pthread_self());
    exit(status);
}
