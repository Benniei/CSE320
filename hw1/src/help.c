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
	//printf("level: %d left: %d middle: %d right: %d\n",level, left, middle, right);
	if(level > 0){
		if(l == -1 || r == -1){
			if(l == -1 && r == -1)
				return -1;
			else if(l == -1)
				return r;
			else if(r == -1)
				return l;
		}
		return bdd_lookup(level+1, l, r);
	}
	else if(left < w*h && right < w*h){
		//printf("NODE LVL 1 left node: %d right node: %d\n", left, right);
		int raster_left = *(raster + left);
		int raster_right = *(raster + right);
		//printf(" raster_left: %d raster_right %d\n\n", raster_left, raster_right);
		return bdd_lookup(1, raster_left, raster_right);
	}
	else if(left < w * h && right >= w * h){
		int raster_single = *(raster + left);
		//printf("NODE LVL 1 raster: %d\n\n", raster_single);
		return raster_single;
	}
	else{
		return -1;
	}
}

void swap_raster_val(unsigned char* a, unsigned char* b){
	int c = *a;
	int d = *b;
	*a = d;
	*b = c;
}

int help_splithalf(unsigned char *raster, int w, int h, int d, int level, int left, int right, int indicator){
	if(left < right && level%2 == 0){

		level--;
		int middle = (right + left)/2;
		//printf("middle = %d\n", middle);
		//printf("PARENT: level: %d left: %d middle: %d right: %d\n", level+ 1, left, middle, right);

		int l = help_splithalf(raster, w, h, d, level, left, middle, 0);
		int r = help_splithalf(raster, w, h, d, level, middle + 1, right, 1);


		return help_placenode(raster, w, h, d, level, left, middle, right, l, r);
	}
	else if(left < right && level > 1){
		level--;
		d /= 2;
		int a = d;
		int b = (d * 4);
		printf("level: %d d: %d a: %d b: %d\n",level, d, a, b);
		if(indicator == 1){
			printf("right node\n");
			a += (d*d *2);
			b += (d*d *2);
			printf("a: %d b: %d\n", a, b);
		}
		for(int i = 0; i < d/2; i++){

			for(int j = 0; j < d; j++){
				//printf("a: %d b: %d\n", a, b);
				swap_raster_val(raster + a, raster + b);
				a++;
				b++;
			}
			a += d;
			b += d;
		}

		printf("----------\n");
        for(int j2= 0; j2 < 8; j2++){
            for(int i2 = 0; i2 < 8; i2++){
                int a = *(raster + ((j2 *8) +i2));
                if(a >= 0 && a <= 63){
                    printf(" ");
                }
                else if(a >= 192 && a <= 255){
                    printf("@");
                }
            }
            printf("\n");
        }
        printf("-----------\n");

		level--;
		int middle = (right + left)/2;
		//printf("middle = %d\n", middle);
		//printf("PARENT: level: %d left: %d middle: %d right: %d\n", level+ 1, left, middle, right);

		int l = help_splithalf(raster, w, h, d, level, left, middle, 0);

		int r = help_splithalf(raster, w, h, d, level, middle + 1, right, 1);


		return help_placenode(raster, w, h, d, level, left, middle, right, l, r);
	}
	else if(level == 1){
		level--;
		int middle = (right + left)/2;
		//printf("middle = %d\n", middle);
		//printf("PARENT: level: %d left: %d middle: %d right: %d\n", level+ 1, left, middle, right);

		int l = help_splithalf(raster, w, h, d, level, left, middle, 0);
		int r = help_splithalf(raster, w, h, d, level, middle + 1, right, 1);


		return help_placenode(raster, w, h, d, level, left, middle, right, l, r);
	}
	return 0;
}
