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

int help_placenode(unsigned char *raster, int w, int h, int d, int level, int left, int middle, int right, int l, int r){
	printf("level: %d left: %d middle: %d right: %d\n",level, left, middle, right);
	// int width_left = left/d;
	// int height_left = left%d;
	// int width_right = right/d;
	// int height_right = right%d;
	// printf("w: %d r: %d\n", w, h);
	// printf("width_left: %d height_left: %d\nwidth_right: %d, height_right: %d\n", width_left, height_left, width_right, height_right);
	if(level > 0){
		return bdd_lookup(level+1, l, r);
	}
	else if(left < w*h && right < w*h){
		//printf("NODE LVL 1 left node: %d right node: %d\n", left, right);
		char raster_left = *(raster + left);
		char raster_right = *(raster + right);
		return bdd_lookup(1, (int)raster_left, (int)raster_right);
	}
	else if(left < w * h && right >= w * h){
		char raster_single = *(raster + left);
		//printf("NODE LVL 1 raster: %d\n\n", raster_single);
		return raster_single;
	}
	else{
		return 0;
	}
}

int help_splithalf(unsigned char *raster, int w, int h, int d, int level, int left, int right){
	if(left < right){

		level--;
		int middle = (right + left)/2;
		//printf("middle = %d\n", middle);
		//printf("PARENT: level: %d left: %d middle: %d right: %d\n", level+ 1, left, middle, right);

		int l = help_splithalf(raster, w, h, d, level, left, middle);

		int r = help_splithalf(raster, w, h, d, level, middle + 1, right);


		return help_placenode(raster, w, h, d, level, left, middle, right, l, r);
		// if(level + 1 > 0 && right < w * h){
		// 	printf("Level: %d Left: %d Right: %d\n", level + 1, left, right);
		// 	return bdd_lookup(level + 1, l, r);
		// }
		// else if(level == 0 && right < w * h){
		// 	printf("raster data: %d ", *(raster + left));
		// 	return *(raster + left);
		// }
		// else
		// 	return 0;
	}
	return 0;
}