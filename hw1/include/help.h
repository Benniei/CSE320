//helper functions

#include <stdio.h>

extern int bddnode_ptr; 

/*
@brief compares 2 strings and returns 1(TRUE) if equal else 0(FALSE)
*/
int help_strcmp(char *original, char *comp);

/*
@brief converts a string representation to an integer
*/
int help_strtoint(char *number);

/*
@brief initialates an array to null
*/
void help_inittonull(int size, BDD_NODE *a);

/*
@brief hashing function for bdd
*/
int help_hashfunction(struct bdd_node c);