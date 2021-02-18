//helper functions

#include <stdio.h>

#include "bdd.h"

int global_bddptr = 256;

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

void help_inithashtonull(){
    for(int i = 0; i < BDD_HASH_SIZE; i++){
        *(bdd_hash_map + BDD_HASH_SIZE) = NULL;
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
	printf("l: %d r: %d", l, r);
	int left = (l * w) + r;
	int right = left + 1;

	// int ele11 = l;
	// int ele12 = r;
	// int ele21 = l;
	// int ele22 = r+1;

	//printf("left: %d right: %d\n", left, right);
	if(l < w && (r + 1) < h  ){
		//printf("NODE LVL 1 left node: %d right node: %d\n", left, right);
		int raster_left = *(raster + left);
		int raster_right = *(raster + right);
		//printf(" raster_left: %d raster_right %d\n\n", raster_left, raster_right);
		return bdd_lookup(1, raster_left, raster_right);
	}
	else if(l < w){
		int raster_single = *(raster + left);
		//printf("NODE LVL 1 raster: %d\n\n", raster_single);
		return raster_single;
	}
	else{
		return -1;
	}
}

int help_splithalf(unsigned char *raster, int w, int h, int d, int level, int minx, int miny, int maxx, int maxy){
	if(level == 1){
		level--;
		//printf("(%d, %d) (%d, %d) <bot %d>\n", minx, miny, maxx, maxy, level+1);
		return help_placenode(raster, w, h, d, level, minx, miny);
	}
	if(level > 0){
		if(level%2 == 0){ //top bottom split

			level--;
			//printf("PARENT TOPBOT: level: %d \n", level+ 1);
			//printf("(%d, %d) (%d, %d) <tb %d>\n", minx, miny, maxx, maxy, level+1);
			int l = help_splithalf(raster, w, h, d, level, minx, miny, (minx + maxx)/2, maxy);
			int r = help_splithalf(raster, w, h, d, level, (minx + maxx)/2, miny, maxx, maxy);
			//printf("(%d, %d) (%d, %d) <tb1 %d>\n", minx, miny, maxx, maxy, level);

			return bdd_lookup(level + 1, l, r);
		}
		else{ //left right split

			level--;
			//printf("PARENT LEFTRIGHT: level: %d \n", level+ 1);

			//printf("(%d, %d) (%d, %d) <lr %d>\n", minx, miny, maxx, maxy, level+1);
			int l = help_splithalf(raster, w, h, d, level, minx, miny, maxx, (miny + maxy)/2);
			int r = help_splithalf(raster, w, h, d, level, minx, (miny+maxy)/2, maxx, maxy);
			//printf("(%d, %d) (%d, %d) <lr1 %d>\n", minx, miny, maxx, maxy, level);

			return bdd_lookup(level + 1, l, r);
		}
	}

	return 0;
}
