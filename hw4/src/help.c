#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conversions.h>
#include <time.h>

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

int add_job(char* file_name, FILE_TYPE* file_type, time_t start_time){
	int position = find_job_empty();
	jobs[position].id = position;
	jobs[position].type = file_type;
	jobs[position].status = JOB_CREATED;
	jobs[position].create_time = start_time;
	jobs[position].eligible = -1;
	jobs[position].file_name = file_name;
	if(global_jobfill == global_jobptr)
		global_jobptr++;
	global_jobfill++;
	return position;
}

int find_job_empty(){
	if(global_jobptr == global_jobfill)
		return global_jobfill;
	for(int i = 0; i < global_jobptr; i++){
		if(jobs[i].status == JOB_DELETED)
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

int find_job(int pos){ //complete
	if(pos < global_jobptr || pos < 0)
		return -1;
	if(jobs[pos].status == JOB_DELETED)
		return -1;
	return pos;
}

void free_names(void){
	for(int i = 0; i < global_printerct; i++)
		free(printers[i].name);
}

void free_job_file(void){
	for(int i = 0; i < global_jobptr;i++)
		free(jobs[i].file_name);
}

int strtoint(char *number){
	int result = 0;
	while(*number != '\0'){
		result *= 10;
		//printf("%d \n",result); debug
		int next = *number++ - 48;
		if(next < 0 || next > 9){
			return -1;
		}
		result += next;
	}
	return result;
}