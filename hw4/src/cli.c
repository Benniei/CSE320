/*
 * Imprimer: Command-line interface
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

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
        size_t cmd_size = strlen(command) + 1; // considers \0
        char* command_cpy = malloc(cmd_size);
        strcpy(command_cpy, command);
        args_counter = num_args(command_cpy);
        free(command_cpy);
    	token = strtok(command, " ");

        /* Miscellaneous commands  */
        if(strcmp(token, "help") == 0){ // DONE
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'help\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }
            else{
                printf("Commands are: help quit type printer conversion printers jobs print cancel disable enable pause resume\n");
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "quit") == 0){ // DONE
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'quit\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }
            else{
                free_names();
                sf_cmd_ok();
                free(command);
                return 1;
            }
        }
        else if(strcmp(token, "type") == 0){ // DONE
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'type\'\n", args_counter, 1);
                sf_cmd_error("arg count");
            }
            else{
                char* file_type;
                file_type = strtok(NULL, " ");
                define_type(file_type);
                sf_cmd_ok();
            }

        }
        /* Confiruration Commands */
        else if(strcmp(token, "printer") == 0){ // DONE
            if(args_counter != 2){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'printer\'\n", args_counter, 2);
                sf_cmd_error("arg count");
            }
            else{
                char* printer_name;
                char* file_type;
                printer_name = strtok(NULL, " ");
                file_type = strtok(NULL, " ");
                // printf("printer: %s, %s\n", printer_name, file_type);
                if(global_printerct >= MAX_PRINTERS){
                    printf("The printer list is full\n");
                    sf_cmd_error("printer (list full)");
                    goto end_free;
                }
                if(find_type(file_type) == NULL){
                    printf("Unknown file type: %s\n", file_type);
                    sf_cmd_error("printer (Unknown File Type)");
                    goto end_free;
                }
                for(int i = 0; i < global_printerct; i++){
                    if(strcmp(printers[i].name,printer_name) == 0){
                        printf("Printer name \'%s\' not unique\n", printer_name);
                        sf_cmd_error("printer (name not unique)");
                        goto end_free;
                    }
                }
                int pos = global_printerct;
                char* pt_name = malloc(strlen(printer_name) + 1);
                strcpy(pt_name, printer_name);
                add_printer(pt_name, file_type);
                sf_printer_defined(pt_name, file_type);
                printf("PRINTER: id=%d, name=%s, type=%s, status=%s\n", printers[pos].id, printers[pos].name, printers[pos].type->name, printer_status_names[printers[pos].status]);
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "conversion") == 0){ // DONE
            if(args_counter < 3){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'conversion\'\n", args_counter, 3);
                sf_cmd_error("arg count");
            }
            else{
                char* file_type1;
                char* file_type2;
                char* conversion_pgm[args_counter - 2];
                file_type1 = strtok(NULL, " ");

                if(find_type(file_type1) == NULL){
                    printf("Unknown file type: %s\n", file_type1);
                    sf_cmd_error("conversion (Unknown File Type)");
                    goto end_free;
                }

                file_type2 = strtok(NULL, " ");

                if(find_type(file_type1) == NULL){
                    printf("Unknown file type: %s\n", file_type1);
                    sf_cmd_error("conversion (Unknown File Type)");
                    goto end_free;
                }

                int i = 0;
                token = strtok(NULL, " ");
                while(token != NULL){
                    conversion_pgm[i] = token;
                    token = strtok(NULL, " ");
                    i++;
                }
                // printf("conversion: %s, %s,%s\n", file_type1, file_type2, (command_cpy + conv_buf));
                define_conversion(file_type1, file_type2, conversion_pgm);
            }
        }
        /* Informational Commands */
        else if(strcmp(token, "printers") == 0){ // DONE
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'printers\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }
            else{
                for(int i = 0; i < global_printerct; i++){
                    printf("PRINTER: id=%d, name=%s, type=%s, status=%s\n", printers[i].id, printers[i].name, printers[i].type->name, printer_status_names[printers[i].status]);
                }
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "jobs") == 0){
            if(args_counter > 0){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'jobs\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }
            else{}
        }
        /* Spooling commands */
        else if(strcmp(token, "print") == 0){ //check again
            if(args_counter < 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'print\'\n", args_counter, 1);
                sf_cmd_error("arg count");
            }
            else{
                char* file_name;
                // char* printer;
                file_name = strtok(NULL, " ");
                FILE_TYPE * file_type = infer_file_type(file_name);
                if(global_jobfill >= MAX_JOBS){
                    printf("The jobs list is full\n");
                    sf_cmd_error("print (list full)");
                    goto end_free;
                }
                if(file_type == NULL){
                    printf("Unable to infer file type\n");
                    sf_cmd_error("print (Unknown File Type)");
                    goto end_free;
                }
                // printer = strtok(NULL, " ");
                // printf("print: %s, %s\n", file_name, printer);

            }
        }
        else if(strcmp(token, "cancel") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'cancel\'\n", args_counter, 1);
                sf_cmd_error("arg count");
            }
            else{
                char* job_number;
                job_number = strtok(NULL, " ");
                printf("cancel: %s\n", job_number);
            }
        }
        else if(strcmp(token, "pause") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'pause\'\n", args_counter, 1);
                sf_cmd_error("arg count");
            }
            else{
                char* job_number;
                job_number = strtok(NULL, " ");
                printf("pause: %s\n", job_number);
            }

        }
        else if(strcmp(token, "resume") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'resume\'\n", args_counter, 1);
                sf_cmd_error("arg count");
            }
            else{
                char* job_number;
                job_number = strtok(NULL, " ");
                printf("resume: %s\n", job_number);
            }
        }
        else if(strcmp(token, "disable") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'disable\'\n", args_counter, 1);
                sf_cmd_error("arg count");
            }
            else{
                char* printer_name;
                printer_name = strtok(NULL, " ");
                printf("disable: %s\n", printer_name);
            }
        }
        else if(strcmp(token, "enable") == 0){
            if(args_counter != 1){
                printf("Wrong number of args(given: %d, required: %d) for CLI command \'disable\'\n", args_counter, 1);
                sf_cmd_error("arg count");
            }
            else{
                char* printer_name;
                printer_name = strtok(NULL, " ");
                printf("disable: %s\n", printer_name);
            }
        }
        /* Unrecognized Command */
        else{
            printf("Unrecognized Command: %s\n", token);
            sf_cmd_error("unrecognized command");
        }
        end_free:
        free(command);
    }

    abort();
}
