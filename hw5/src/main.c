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

/*
 * "Charla" chat server.
 *
 * Usage: charla <port>=
 */
int main(int argc, char* argv[]){
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.
    int counter = 1; // counters the amount of variables being used
    char hflag = 0, pflag = 0, dflag = 0, qflag = 0, pdec = 0;
    char* port;
    while(*(argv + counter) != NULL){
        if(counter == argc){
            printf("USAGE: -h <hostname> (optional), -p <port> (required), -d, -q\n");
            exit(EXIT_FAILURE);
        }
        if(strcmp(*(argv + counter), "-h") == 0){
            // optional host name
            if(hflag == 1){
                printf("Error: h has been declared twice\n");
                exit(EXIT_FAILURE);
            }
            else
                hflag = 1;

            // printf("-h\n");
            counter++;
            if(counter == argc || **(argv + counter) == '-'){
                // local host will be chosen
                printf("local host is chosen\n");
                continue;
            }
            printf("Host: %s\n", *(argv + counter));
            counter++;
        }
        else if(strcmp(*(argv + counter), "-p") == 0){
            // required port number
            if(pflag == 1){
                printf("Error: p has been declared twice\n");
                exit(EXIT_FAILURE);
            }
            else
                pflag = 1;

            // printf("-p\n");
            counter++;
            if(counter == argc || **(argv + counter) == '-'){
                // Invalid port name
                printf("Error: Invalid port name\n");
                exit(EXIT_FAILURE);
            }
            printf("Portname: %s\n", *(argv + counter));
            pdec = 1;
            port = *(argv + counter);
            counter++;
        }
        else if(strcmp(*(argv + counter), "-d") == 0){
            // debug
            if(dflag == 1){
                printf("Error: d has been declared twice\n");
                exit(EXIT_FAILURE);
            }
            else
                dflag = 1;
            // printf("-d\n");
            counter++;
        }
        else if(strcmp(*(argv + counter), "-q") == 0){
            // run without prompting
            if(qflag == 1){
                printf("Error: q has been declared twice\n");
                exit(EXIT_FAILURE);
            }
            else
                qflag = 1;

            // printf("-q\n");
            counter++;
        }
        else{
            printf("USAGE: -h <hostname> (optional), -p <port> (required), -d, -q\n");
            exit(EXIT_FAILURE);
        }
    }
    if(pdec == 0){
        printf("Port number was not provided");
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
    int listenfd = Open_listenfd(port);
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
