#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "imprimer.h"
#include "conversions.h"
#include "sf_readline.h"


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