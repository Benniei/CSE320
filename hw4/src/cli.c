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
    int args_counter;
    while(1){
    	char* command = sf_readline("imp> ");
    	char* token;

        args_counter = num_args(command);
        // printf("args_counter: %d\n", args_counter);
    	token = strtok(command, " ");


        /* Miscellaneous commands  */
        if(strcmp(token, "help") == 0){
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'help\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }
            else{
                printf("Commands are: help quit type printer conversion printers jobs print cancel disable enable pause resume\n");
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "quit") == 0){
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'help\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }
            else{
                sf_cmd_ok();
                return 0;
            }
        }
        else if(strcmp(token, "type") == 0){

        }
        /* Confiruration Commands */
        else if(strcmp(token, "printer") == 0){

        }
        else if(strcmp(token, "conversion") == 0){

        }
        else if(strcmp(token, "quit") == 0){

        }
        /* Informational Commands */
        else if(strcmp(token, "printers") == 0){

        }
        else if(strcmp(token, "jobs") == 0){

        }
        else if(strcmp(token, "print") == 0){

        }
        else if(strcmp(token, "cancel") == 0){

        }
        else if(strcmp(token, "pause") == 0){

        }
        else if(strcmp(token, "resume") == 0){

        }
        else if(strcmp(token, "disable") == 0){

        }
        else if(strcmp(token, "enable") == 0){

        }
        /* Unrecognized Command */
        else{
            printf("Unrecognized Command: %s\n", token);
            sf_cmd_error("unrecognized command");
        }
    }

    abort();
}
