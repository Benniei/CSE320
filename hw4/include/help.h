#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
	int eligible;
	char* file_name;
}JOB;

JOB jobs[MAX_JOBS];
int global_jobptr;
int global_jobfill;

/* Functions */
int num_args(char* command); /* returns the number of arguments */

void add_printer(char* name, char* filetp);

void free_names(void);