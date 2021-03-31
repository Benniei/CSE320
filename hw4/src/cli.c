/*
 * Imprimer: Command-line interface
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#include "imprimer.h"
#include "conversions.h"
#include "sf_readline.h"
#include "help.h"

int run_cli(FILE *in, FILE *out)
{
    // TO BE IMPLEMENTED
	// fprintf(stderr, "You have to implement run_cli() before the application will function.\n");
    while(1){
    	char* command = sf_readline("imp> ");
    	char* token;

    	token = strtok(command, " ");

    	printf("%s\n", command);
    	printf("%s\n", token);
        token = strtok(NULL, " ");
        printf("%s\n", token);
    }

    abort();
}
