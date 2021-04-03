#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conversions.h>

#include "imprimer.h"
#include "sf_readline.h"
#include "help.h"

/* Global Variables */
int global_printerct = 0;
int global_jobptr = 0;
int global_jobfill = 0;

/* enum arrays */
char* printer_status_names[] = {"disabled", "idle", "busy"};
char* job_status_names[] = {"created", "running", "paused", "finished", "aborted", "deleted"};

/* Functions */
int num_args(char* command){
	char* token;
	int word = 0;
	token = strtok(command, " ");
	while(token != NULL){
		// printf("%s\n", token);
		token = strtok(NULL, " ");
		word++;
	}
	return word - 1;
}

void add_printer(char* name, char* filetp){
	printers[global_printerct].id = global_printerct;
	printers[global_printerct].name = name;
	printers[global_printerct].type = find_type(filetp);
	printers[global_printerct].status = PRINTER_DISABLED;
	global_printerct++;
}

void free_names(void){
	for(int i = 0; i < global_printerct; i++)
		free(printers[i].name);
}