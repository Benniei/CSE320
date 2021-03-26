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
		asize = ALIGN((size + WSIZE)); /* add header*/
	//printf("asize: %ld\n", asize);
	fl_index = sf_find_fit(asize);
	//printf("fl_index: %d\n", fl_index);
	sf_block* head;
	for(int i = fl_index; i < 7; i++){ //checking the freelist except for wilderness block
		head = (sf_free_list_heads + i);
		sf_block* ptr = GET_NEXT(head);
		while(ptr != head){
			if(GET_SIZE(ptr) >= asize){
				head = sf_insert((sf_block *)ptr, asize);
				return TO_PTR(head);
			}
			ptr = GET_NEXT(ptr);
		}
	}
	// taking from wilderness block
	sf_block* extend;
	if(GET_PREALLOC(sf_mem_end() - WSIZE)){
		if((extend = sf_extend_heap()) == NULL){
				return NULL;
		}
		//sf_show_blocks();
		//printf("mem is full");//make new page of memory and let the rest be handled by bottom
	}

	head = GET_NEXT((sf_free_list_heads + 7));
	size_t wilder_size = GET_SIZE(head);
	//printf("wilder size: %ld\n", wilder_size);
	//printf("asize: %ld\n", asize);

	if(asize <= wilder_size)
		head = sf_insert(head, asize);
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
	validate_pointer(pp);
	sf_block* node;
	node = sf_change_to_free((sf_block*)HEADER(pp));
	node = sf_coalesce(node);
	SET_FREE((RIGHT(node))); //sets the prealloc of the next block
    return;
}

void *sf_realloc(void *pp, size_t rsize) {
	validate_pointer(pp);
	size_t asize;
	size_t psize = GET_SIZE(HEADER(pp));
	if(rsize == 0){
		sf_free(pp);
		return NULL;
	}
	if(rsize + WSIZE <= 32)
		asize = 32;
	else
		asize = ALIGN((rsize + WSIZE)); /* add header*/
	//printf("asize: %ld\n", asize);
	if(asize > psize){
	//larger size
		sf_block* ptr;
		if((ptr = sf_malloc(rsize)) == NULL)
			return NULL;
		memcpy(ptr, pp, psize - WSIZE);
		sf_free(pp);
		return ptr;
	}
	else
		sf_split((sf_block*)(HEADER(pp)), asize);
    return pp;
}

void *sf_memalign(size_t size, size_t align) {
	if(align < 32){
		errno = EINVAL;
		return NULL;
	}
	if(POWERTWO(align) == 0){
		errno = EINVAL;
		return NULL;
	}
	size_t nsize = size + align + MIN_BLOCK_SIZE;
	size_t pad = align + MIN_BLOCK_SIZE;
	sf_block* hp = sf_malloc(nsize);
	nsize = GET_SIZE(HEADER(hp));
	size_t excess;
	if((size_t)(hp) % align == 0)
		excess = 0;
	else
		excess = align - ((size_t)(hp) % align);
	size_t palloc = GET_PREALLOC(HEADER(hp))>>1;
	if(excess > 0 && excess < 32)
		excess = excess + align;
	/* cut the front */
	if(excess > 0){
		sf_block* node = (sf_block*)HEADER(hp);
		SET_DATA(HEADER(hp), PACK(excess, palloc, 0));
		SET_DATA(FOOTER(hp), PACK(excess, palloc, 0));
		/* retain the middle */
		hp = (sf_block*)(RIGHT(HEADER(hp)));
		nsize = nsize - excess;
		palloc = 0;
		pad = pad - excess;
		SET_DATA(hp, PACK(nsize, 0, 1));
		sf_coalesce((sf_block*)(node));
		hp = (sf_block*)TO_PTR(hp);
	}
	/* cut the back */
	sf_realloc(hp, (nsize - pad - WSIZE));
    return hp;
}
