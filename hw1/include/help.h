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
@brief compares 2 bdd nodes to see if they are equal and returns T/F
*/
int compare_bdd(struct bdd_node a, struct bdd_node c);

/*
@brief hashing function for bdd
*/
int help_hashfunction(struct bdd_node c);
