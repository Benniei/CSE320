//helper functions

#include <stdio.h>

#include "const.h"
#include "bdd.h"

int global_bddptr = 256;
int global_bddindex = 0;

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
		int next = *number++ - 48;
		if(next < 0 || next > 9){
			return -1;
		}
		result += next;
	}
	return result;
}

void help_inithashtonull(){
    for(int i = 0; i < BDD_HASH_SIZE; i++){
        *(bdd_hash_map + i) = NULL;
    }
}

void help_clearindexmap(){
	global_bddindex = 0;
}

void help_clearrasterdata(){
	for(int i = 0; i < RASTER_SIZE_MAX; i++){
		*(raster_data + i) = 0;
	}
}

int compare_bdd(struct bdd_node a, struct bdd_node c){
	if((a.level == c.level) && (a.left == c.left) && (a.right == c.right)){
		return 1;
	}
	return 0;
}

int help_hashfunction(struct bdd_node c){
	int index = (int)(c.level) + c.left + c.right;
	int multiply = 5;
	index *= multiply;
	index = index % BDD_HASH_SIZE;
	return index;
}

int help_placenode(unsigned char *raster, int w, int h, int d, int level, int l, int r){
	//printf("row: %d column: %d\n", l, r);
	int left = (l * w) + r;
	int right = left + 1;


	if(l < h && (r + 1) < w ){
		//printf("NODE LVL 1 left node: %d right node: %d\n", left, right);
		int raster_left = *(raster + left);
		int raster_right = *(raster + right);
		//printf(" raster_left: %d raster_right %d\n\n", raster_left, raster_right);
		return bdd_lookup(1, raster_left, raster_right);
	}
	else if(l < h && (r + 1) == w){
		int raster_single = *(raster + left);
		//printf("NODE LVL 1 raster: %d\n\n", raster_single);
		return raster_single;
	}
	else{
		return 0;
	}
}

int help_splithalf(unsigned char *raster, int w, int h, int d, int level, int minx, int miny, int maxx, int maxy){
	if(level == 1){
		level--;
		//printf("(%d, %d) (%d, %d) <bot %d>\n", minx, miny, maxx, maxy, level+1);
		int hold = help_placenode(raster, w, h, d, level, minx, miny);
		// printf("HOLD: %d\n", hold);
		return hold;
	}
	if(level > 0){
		if(level%2 == 0){ //top bottom split

			level--;
			//printf("PARENT TOPBOT: level: %d \n", level+ 1);
			//printf("(%d, %d) (%d, %d) <tb %d>\n", minx, miny, maxx, maxy, level+1);
			int l = help_splithalf(raster, w, h, d, level, minx, miny, (minx + maxx)/2, maxy);
			int r = help_splithalf(raster, w, h, d, level, (minx + maxx)/2, miny, maxx, maxy);
			return bdd_lookup(level + 1, l, r);
		}
		else{ //left right split
			level--;
			//printf("PARENT LEFTRIGHT: level: %d \n", level+ 1);
			//printf("(%d, %d) (%d, %d) <lr %d>\n", minx, miny, maxx, maxy, level+1);
			int l = help_splithalf(raster, w, h, d, level, minx, miny, maxx, (miny + maxy)/2);
			int r = help_splithalf(raster, w, h, d, level, minx, (miny+maxy)/2, maxx, maxy);
			return bdd_lookup(level + 1, l, r);
		}
	}

	return 0;
}

int help_inbddindex(int a){
	int flag = 0;
	for(int i = 0; i < global_bddindex; i++){
		if (a == *(bdd_index_map + i)){
			flag = 1;
			break;
		}
	}
	return flag;
}

int help_inbddfindserial(int a){
	int location = -1;
	for(int i = 0; i < global_bddindex; i++){
		if(a == *(bdd_index_map + i)){
			location = i + 1;
			break;
		}
	}
	return location;
}

int help_bddmap(BDD_NODE* node, unsigned char (*func)(unsigned char)){
	BDD_NODE root = *node;
    int left, right;
    if((root.left) > 255){
        left = bdd_map(bdd_nodes + root.left, func);
    }else{
        return func(root.left);
    }
    if((root.right) > 255){
        right = bdd_map(bdd_nodes + root.right, func);
    }else{
        return func(root.right);
    }
    int node_index = bdd_lookup(root.level, left, right);
    return node_index;
}