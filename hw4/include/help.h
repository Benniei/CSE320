#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "imprimer.h"
#include "sf_readline.h"

/* Printer Struct */
typedef struct printer {
	int id;
	char* name;
	FILE_TYPE* type;
	PRINTER_STATUS status;
}PRINTER;

PRINTER printers[MAX_PRINTERS];
int global_printerct;

/* Job Struct */
typedef struct job {
	int id;
	FILE_TYPE* type;
	JOB_STATUS status;
	time_t create_time;
	int eligible;
	char* file_name;
	int num_eligible;
	char* eligible_printers[MAX_PRINTERS];
	/* fork */
	pid_t pgid;
	int printer_id;
	int num_conversions;
}JOB;

JOB jobs[MAX_JOBS];
int global_jobptr; // pointer for end of array
int global_jobfill; // how many jobs are filled (always <= jobptr)

/* Functions */
int num_args(char* command); /* returns the number of arguments */

void add_printer(char* name, char* filetp);

int add_job(char* file_name, FILE_TYPE* file_type, time_t start_time);

int find_job_empty();

int find_printer(char* name);

int find_job(int pos);

void free_names(void);

void free_single_job(int i);

void free_job_file(void);

int strtoint(char* number);

int check_conversion(char* type1, char* type2);

