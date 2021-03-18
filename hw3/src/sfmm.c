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

	if(size + WSIZE <= 32)
		asize = 32;
	else
		asize = ALIGN((size + WSIZE)); /* add header and footer */
	//printf("asize: %ld\n", asize);
	fl_index = sf_find_fit(asize);
	//printf("fl_index: %d\n", fl_index);
	sf_block* head;
	for(int i = fl_index; i < 7; i++){ //checking the freelist except for wilderness block
		head = (sf_free_list_heads + i);
		if(GET_NEXT(head) != head){
			head = sf_insert((sf_block *)GET_NEXT(head), asize);
			return TO_PTR(head);
		}
	}
	// taking from wilderness block
	sf_block* extend;
	if(GET_PREALLOC(sf_mem_end() - WSIZE)){
		if((extend = sf_extend_heap()) == NULL){
				return NULL;
		}
		//printf("mem is full");//make new page of memory and let the rest be handled by bottom
	}

	head = GET_NEXT((sf_free_list_heads + 7));
	size_t wilder_size = GET_SIZE(head);
	//printf("wilder size: %ld\n", wilder_size);
	//printf("asize: %ld\n", asize);

	if(asize <= wilder_size){
		head = sf_insert(head, asize);
		printf("%p", head);
	}
	else{
		while(asize > wilder_size){
			if((extend = sf_extend_heap()) == NULL){
				return NULL;
			}
			wilder_size += PAGE_SZ;
		}
		head = sf_insert(head, asize);
	}

	/*
	Check if fit_in wilderness block -> yes, then split or just take the whole wilderness
	-> else extended the page and then check again
	*/
	return TO_PTR(head);
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
