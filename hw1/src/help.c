//helper functions

#include <stdio.h>

int test(){
	printf("Hello from test \n");
	return 0;
}

int help_strcmp(char *original, char *comp){

	while((*original != '\0' || *comp != '\0')){
		if(*original > *comp)
			return 0;
		if(*original < *comp)
			return 0;
		original++;
		comp++;
	}
	return 1;
}