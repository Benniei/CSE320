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
        int location = (*(bdd_hash_map + hash_index) - (bdd_nodes));
        return location;
    }
    else{
        //insert into table and hashtable
        (bdd_nodes + global_bddptr)->level= level + '@';
        (bdd_nodes + global_bddptr)->left= left;
        (bdd_nodes + global_bddptr)->right= right;
        *(bdd_hash_map + hash_index) = (bdd_nodes+ global_bddptr);
        //printf("%d  %c, %d, %d\n", global_bddptr, (bdd_nodes+ global_bddptr)->level, (bdd_nodes+ global_bddptr)->left, (bdd_nodes+ global_bddptr)-> right);
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
    int square_dim = 1 << (min_level/2);
    //printf("Min_level: %d Square_dimensions: %d w: %d h: %d\n", min_level, square_dim, w, h);
    help_splithalf(raster, w, h, square_dim, min_level, 0, 0, square_dim, square_dim);
    return (bdd_nodes + global_bddptr - 1);
}

void bdd_to_raster(BDD_NODE *node, int w, int h, unsigned char *raster) {
    // TO BE IMPLEMENTED
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            *raster++ = bdd_apply(node, i, j);
        }
    }
}

int bdd_serialize(BDD_NODE *node, FILE *out) {
    // TO BE IMPLEMENTED
    BDD_NODE root = *node;
    if((root.left) > 255){
        bdd_serialize(bdd_nodes + root.left, out);
    }else{
        if(help_inbddindex(root.left) == 0){
            *(bdd_index_map + global_bddindex++) = root.left;
            char a = root.left;
            fputc('@', out);
            fputc(a , out);
        }
    }
    if((root.right) > 255){
        bdd_serialize(bdd_nodes + root.right, out);
    }else{
        if(help_inbddindex(root.right) == 0){
            *(bdd_index_map + global_bddindex++) = root.right;
            char a = root.right;
            fputc('@', out);
            fputc(a, out);
        }
    }
    //printf("%c %d %d  \n", root.level, root.left, root.right);
    int node_index = bdd_lookup((int)(root.level - '@'), root.left, root.right);
    if(help_inbddindex(node_index) == 0){
        *(bdd_index_map + global_bddindex++) = node_index;
        fputc(root.level, out);
        int left = help_inbddfindserial(root.left);
        int right = help_inbddfindserial(root.right);
        fputc(left & 0xff, out);
        fputc(left>>8 & 0xff, out);
        fputc(left>>16 & 0xff, out);
        fputc(left>>24 & 0xff, out);
        fputc(right & 0xff, out);
        fputc(right>>8 & 0xff, out);
        fputc(right>>16 & 0xff, out);
        fputc(right>>24 & 0xff, out);
    }
    return 0;
}

BDD_NODE *bdd_deserialize(FILE *in) {
    // TO BE IMPLEMENTED
    int c;
    do{
        c = fgetc(in);
        //printf("%d ", c);

        if (c == 64){ //@
            c = fgetc(in);
            //printf("Leaf: %d\n", c);
            *(bdd_index_map + global_bddindex++) = c;
        }
        else if(c > 64){
            //printf("c: %c\n", c);
            int a, b;
            fread(&a, 4, 1, in); //index of left node
            fread(&b, 4, 1, in); //index of right node
            // printf("a: %d b: %d\n", a, b);
            // printf("a: %d b: %d\n", *(bdd_index_map + a - 1), *(bdd_index_map + b - 1));
            if(a - 1 > global_bddindex || b-1 > global_bddindex){
                return NULL;
            }
            int loc = bdd_lookup(c - 64, *(bdd_index_map + a - 1), *(bdd_index_map + b - 1));
            *(bdd_index_map + global_bddindex++) = loc;
        }
        if(c == EOF){
            break;
        }

    }while(1);
    return (bdd_nodes + global_bddptr - 1);
}

unsigned char bdd_apply(BDD_NODE *node, int r, int c) {
    // TO BE IMPLEMENTED
    int level = node->level - '@';
    int size = 1 << (level/2);
    int minx = 0, miny = 0;
    int maxx = size;
    int maxy = size;
    //printf("Level: %d size: %d\n", level, size);
    if(r < size || c < size){
        while(1){
            level = node->level - '@';
            if(level%2 ==0){//topbot
                // printf("(%d, %d) (%d, %d) <tb %d> %d\n", minx, miny, maxx, maxy, level+1, bdd_lookup(level + 1, node->left, node->right));
                // printf("left: %d right: %d\n", node->left, node-> right);
                if((r >= minx) && (c >= miny) && (r < (minx + maxx)/2) && (c < maxy)){//left node
                    //printf("left<T>\n");
                    maxx = (minx + maxx)/2;
                    if(node->left < 256){
                        return node->left;
                    }
                    else{
                        node = LEFT(node, level + 1);
                    }
                }
                else{
                    //printf("right<B>\n");
                    minx = (minx + maxx)/2;
                    if(node->right < 256){
                        return node->right;
                    }
                    else{
                        node = RIGHT(node, level + 1);
                    }
                }
            }
            else{//leftright
                //printf("(%d, %d) (%d, %d) <lr %d> %d\n", minx, miny, maxx, maxy, level+1, bdd_lookup(level + 1, node->left, node->right));
                if(((r >= minx) && (c>= miny)) && ((r < maxx) && (c < (miny + maxy)/2))){
                    //printf("left<L>\n");
                    maxy = (miny + maxy)/2;
                    if(node->left < 256){
                        return node->left;
                    }
                    else{
                        node = LEFT(node, level + 1);
                    }
                }
                else{
                    //printf("right<R>\n");
                    miny = (miny + maxy)/2;
                    if(node->right < 256){
                        return node->right;
                    }
                    else{
                        node = RIGHT(node, level + 1);
                    }
                }
            }
        }
    }
    return -1;
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
