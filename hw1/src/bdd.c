#include <stdlib.h>
#include <stdio.h>

#include "bdd.h"
#include "debug.h"
#include "help.h"

/*
 * Macros that take a pointer to a BDD node and obtain pointers to its left
 * and right child nodes, taking into account the fact that a node N at level l
 * also implicitly represents nodes at levels l' > l whose left and right children
 * are equal (to N).
 *
 * You might find it useful to define macros to do other commonly occurring things;
 * such as converting between BDD node pointers and indices in the BDD node table.
 */
#define LEFT(np, l) ((l) > (np)->level ? (np) : bdd_nodes + (np)->left)
#define RIGHT(np, l) ((l) > (np)->level ? (np) : bdd_nodes + (np)->right)

/**
 * Look up, in the node table, a BDD node having the specified level and children,
 * inserting a new node if a matching node does not already exist.
 * The returned value is the index of the existing node or of the newly inserted node.
 *
 * The function aborts if the arguments passed are out-of-bounds.
 */
int bdd_lookup(int level, int left, int right) {
    // TO BE IMPLEMENTED
    if(level == 0){
        printf("in");
        return -1;
    }
    if(left == right){
        return left;
    }
    BDD_NODE c = {level + '@', left, right};
    //check hashtable for entry
    int hash_index = help_hashfunction(c);
    int inc = hash_index - 1;
    int found = 0; //flag

    while(found == 0 && *(bdd_hash_map + hash_index) != NULL && hash_index != inc){
        if(compare_bdd(**(bdd_hash_map + hash_index), c)){
            found = 1;
            break;
        }
        if(hash_index == BDD_HASH_SIZE)
            hash_index = 0;
        else
            hash_index++;
    }

    if(found){
        //return the index where it is found
        //printf("difference: %ld\n", (*(bdd_hash_map + hash_index) - bdd_nodes));
        int location = (*(bdd_hash_map + hash_index) - (bdd_nodes));
        return location;
    }
    else{
        //insert into table and hashtable
        (bdd_nodes + global_bddptr)->level= level + '@';
        (bdd_nodes + global_bddptr)->left= left;
        (bdd_nodes + global_bddptr)->right= right;
        *(bdd_hash_map + hash_index) = (bdd_nodes+ global_bddptr);
        //printf("direction: %p\n", (bdd_nodes + global_bddptr));
        printf("%d, %d, %d\n\n", (bdd_nodes+ global_bddptr)->level, (bdd_nodes+ global_bddptr)->left, (bdd_nodes+ global_bddptr)-> right);
        global_bddptr += 1;
        return global_bddptr - 1;
    }
}

int bdd_min_level(int w, int h) {
    //to be implemented
    int max = (w > h)? w:h;
    int copy = max;
    int result = 0;
    while(max){
        result++;
        max >>= 1;
    }
    max = 1 << (result - 1);
    if(max == copy){
        result--;
    }

    return result * 2;
}

BDD_NODE *bdd_from_raster(int w, int h, unsigned char *raster) {
    // TO BE IMPLEMENTED
    int min_level = bdd_min_level(w, h);
    int square_dim = (min_level/2) << 1;
    int expanded_raster = (1 << square_dim) - 1;
    //printf("Min_level: %d Square_dimensions: %d End of raster: %d\n", min_level, square_dim, expanded_raster);
    help_splithalf(raster, w, h, square_dim, min_level, 0, expanded_raster);
    return (bdd_nodes + global_bddptr - 1);
}

void bdd_to_raster(BDD_NODE *node, int w, int h, unsigned char *raster) {
    // TO BE IMPLEMENTED
}

int bdd_serialize(BDD_NODE *node, FILE *out) {
    // TO BE IMPLEMENTED
    return -1;
}

BDD_NODE *bdd_deserialize(FILE *in) {
    // TO BE IMPLEMENTED
    return NULL;
}

unsigned char bdd_apply(BDD_NODE *node, int r, int c) {
    // TO BE IMPLEMENTED
    return 0;
}

BDD_NODE *bdd_map(BDD_NODE *node, unsigned char (*func)(unsigned char)) {
    // TO BE IMPLEMENTED
    return NULL;
}

BDD_NODE *bdd_rotate(BDD_NODE *node, int level) {
    // TO BE IMPLEMENTED
    return NULL;
}

BDD_NODE *bdd_zoom(BDD_NODE *node, int level, int factor) {
    // TO BE IMPLEMENTED
    return NULL;
}
