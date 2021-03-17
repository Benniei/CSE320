/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "sfmm.h"
#include "help.h"

void *sf_malloc(size_t size) {
	size_t asize; // adjusted block size
	int fl_index; // free_list index

	if(size == 0)
		return NULL;

	if(sf_mem_start() == sf_mem_end()) // initates the heap
		sf_init();

	asize = ALIGN((size + DSIZE)); /* add header and footer */
	fl_index = sf_find_fit(asize);

	for(int i = fl_index; i < 7; i++){ //checking the freelist except for wilderness block
		sf_block* head = (sf_free_list_heads + i);
		sf_block* pointer = (sf_free_list_heads + i);
		if(GET_NEXT(pointer) != head){
			// allocate the block calls remove/insert FL and insert
			// sf_block* freed = remove_free_list(GET_NEXT(pointer));
		}
	}
	// taking from wilderness block

	/*
	Check if fit_in wilderness block -> yes, then split or just take the whole wilderness
	-> else extended the page and then check again
	*/
    return NULL;
}

void sf_free(void *pp) {
    return;
}

void *sf_realloc(void *pp, size_t rsize) {
    return NULL;
}

void *sf_memalign(size_t size, size_t align) {
    return NULL;
}
