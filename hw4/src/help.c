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

int add_job(char* file_name, FILE_TYPE* file_type){
	int position = find_job_empty();
	jobs[position].id = position;
	jobs[position].type = file_type;
	jobs[position].status = JOB_CREATED;
	jobs[position].eligible = -1;
	jobs[position].file_name = file_name;
	return position;
}

int find_job_empty(){
	if(global_jobptr == global_jobfill)
		return global_jobfill;
	for(int i = 0; i < global_jobptr; i++){
		if(jobs[i].used_entry == 0)
			return i;
	}
	return -1;
}

int find_printer(char* name){
	for(int i = 0; i < global_printerct; i++){
		if(strcmp(printers[i].name, name) == 0)
			return i;
	}
	return -1;
}

void free_names(void){
	for(int i = 0; i < global_printerct; i++)
		free(printers[i].name);
}