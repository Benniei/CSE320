//helper functions

#include <stdio.h>

int global_bddptr;
int global_bddindex;

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
void help_inithashtonull();

/*
@brief clears the index map
*/
void help_clearindexmap();

/*
@brief clears the raster data
*/
void help_clearrasterdata();

/*
@brief compares 2 bdd nodes to see if they are equal and returns T/F
*/
int compare_bdd(struct bdd_node a, struct bdd_node c);

/*
@brief hashing function for bdd
*/
int help_hashfunction(struct bdd_node c);

/*
@brief breaks the raster into half
*/
int help_splithalf(unsigned char *raster, int w, int h, int d, int level, int minx, int miny, int maxx, int maxy);

/*
@brief checks if in bdd_index_map
*/
int help_inbddindex(int a);

/*
@brief finds serial number in index_map
*/
int help_inbddfindserial(int a);

/*
@brief threshold function
*/
unsigned char threshold_mask(unsigned char a);

/*
@brief negative function
*/
unsigned char negative_mask(unsigned char a);

/*
@brief helper for bdd_map traversal
*/
int help_bddmap(BDD_NODE* node, unsigned char (*func)(unsigned char));