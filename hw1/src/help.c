//helper functions

#include <stdio.h>

#include "bdd.h"

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

int help_strtoint(char *number){
	int result = 0;
	while(*number != '\0'){
		result *= 10;
		//printf("%d \n",result); debug
		result += *number++ - 48;
	}
	return result;
}

void help_inittonull(int size, BDD_NODE **a){
	int i;
	for(int i = 0; i < size; i++){
		*(a + i) = NULL;
	}
}

int help_hashfunction(struct bdd_node c){
	int index = (int)(c.level) + c.left + c.right;
	int multiply = 5;
	index *= index * multiply;
	index = index % BDD_HASH_SIZE;
	return index;
}