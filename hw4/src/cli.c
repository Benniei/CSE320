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
    	char* command;
        char* line_buf = NULL;
        size_t line_buf_size = 0;
        ssize_t line_size = 0;
        if(in == stdin)
            command = sf_readline("imp> ");
        else{
            line_size = getline(&line_buf, &line_buf_size, in);
            if(line_size <= 0){
                // printf("\n");
                free(line_buf);
                return 0;
            }
            command = line_buf;
        }

    	char* token;
        // printf("%s\n", command);
        args_counter = num_args(command);
    	token = strtok(command, " ");

        /* Miscellaneous commands  */
        if(strcmp(token, "help") == 0){
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'help\'\n", args_counter, 0);
                sf_cmd_error("arg count");
                free(command);
            }
            else{
                printf("Commands are: help quit type printer conversion printers jobs print cancel disable enable pause resume\n");
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "quit") == 0){
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'quit\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }
            else{
                sf_cmd_ok();
                free(command);
                return 1;
            }
        }
        else if(strcmp(token, "type") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'type\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        /* Confiruration Commands */
        else if(strcmp(token, "printer") == 0){
            if(args_counter != 2){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'printer\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        else if(strcmp(token, "conversion") == 0){ //check again
            if(args_counter != 3){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'conversion\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        /* Informational Commands */
        else if(strcmp(token, "printers") == 0){
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'printers\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        else if(strcmp(token, "jobs") == 0){
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'jobs\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        else if(strcmp(token, "print") == 0){ //check again
            if(args_counter != 2){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'print\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        else if(strcmp(token, "cancel") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'cancel\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        else if(strcmp(token, "pause") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'pause\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        else if(strcmp(token, "resume") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'resume\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        else if(strcmp(token, "disable") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'disable\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        else if(strcmp(token, "enable") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'disable\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }

        }
        /* Unrecognized Command */
        else{
            printf("Unrecognized Command: %s\n", token);
            sf_cmd_error("unrecognized command");
        }
        free(command);
    }

    abort();
}
