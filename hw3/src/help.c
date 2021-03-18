#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "sfmm.h"
#include "help.h"

/*
Functions to implement:
cut out a portion of/entire free
	Insert into free_list
extend heap
remove from free_list
*/
void sf_init_free_list(){
	for(int i = 0; i < 8; i++){
		sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
		sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
	}
}

sf_block* remove_free_list(sf_block* bp){ //Parameter: pointer to location on sf_free_lists
	sf_block* node = bp;
	SET_NEXT(GET_PREV(bp), GET_NEXT(bp));
	SET_PREV(GET_NEXT(bp), GET_PREV(bp));
	SET_PREV(bp, NULL);
	SET_NEXT(bp, NULL);
	return node;
}

sf_block* insert_free_list(sf_block* bp, sf_block* ins){
	if(GET_NEXT(bp) == bp){ //only the header
		SET_NEXT(bp, ins);
		SET_PREV(bp, ins);
		SET_NEXT(ins, bp);
		SET_PREV(ins, bp);
	}
	else{
		sf_block* node_b = GET_NEXT(bp);
		SET_NEXT(bp, ins);
		SET_NEXT(ins, node_b);
		SET_PREV(node_b, ins);
		SET_PREV(ins, bp);
	}
	return bp;
}


sf_block* sf_insert(sf_block* bp, size_t asize){
	remove_free_list(bp);
	size_t csize = GET_SIZE(bp);
	sf_block* node = bp;
	if((csize - asize) >= MIN_BLOCK_SIZE){
		SET_DATA(bp, PACK(asize, 0, 1));
		bp = (sf_block*)(RIGHT(bp));
		SET_DATA(bp, PACK(csize-asize, 1, 0));
		SET_DATA(FOOTER(TO_PTR(bp)), PACK(csize-asize, 1, 0));
		sf_coalesce(bp);
		return node;
	}else{
		SET_DATA(bp, PACK(csize, 0, 1));
		SET_PALLOC((sf_block*)(RIGHT(bp)));
		return bp;
	}
}

int check_wilderness(sf_block* bp){
	if((GET_SIZE(RIGHT(bp)) == 0) && (GET_ALLOC(RIGHT(bp)) == 1))
		return 1;
	return 0;
}

sf_block* sf_coalesce(sf_block* bp){ // pass in the pointer
	size_t prev_alloc = GET_PREALLOC(bp)>>1;
	size_t next_alloc = GET_ALLOC(RIGHT(bp));
	size_t size = GET_SIZE(bp);
	if(prev_alloc && next_alloc){
		if(check_wilderness(bp)){
			insert_free_list((sf_free_list_heads + 7), bp);
		}
		else{
			int index = sf_find_fit(size);
			insert_free_list((sf_free_list_heads + index), bp);
		}
		return bp;
	}
	else if(prev_alloc && !next_alloc){ // previous block allocated and next block is not
		sf_block* node = remove_free_list((sf_block*)(RIGHT(bp)));
		size += GET_SIZE(node);
		SET_DATA(bp, PACK(size, 1, 0));
		SET_DATA(FOOTER(TO_PTR(node)), PACK(size, 1, 0));
		if(check_wilderness(bp)){
			insert_free_list((sf_free_list_heads + 7), bp);
		}
		else{
			int index = sf_find_fit(size);
			insert_free_list((sf_free_list_heads + index), bp);
		}
	}
	else if(!prev_alloc && next_alloc){
		sf_block* node = remove_free_list((sf_block*)(LEFT(bp)));
		size += GET_SIZE(node);
		SET_DATA(node, PACK(size, 1, 0));
		SET_DATA((FOOTER(TO_PTR(node))), PACK(size, 1, 0));
		bp = node;
		if(check_wilderness(bp)){
			insert_free_list((sf_free_list_heads + 7), bp);
		}
		else{
			int index = sf_find_fit(size);
			insert_free_list((sf_free_list_heads + index), bp);
		}
	}
	else{
		sf_block* node1 = remove_free_list((sf_block*)LEFT(bp));
		sf_block* node2 = remove_free_list((sf_block*)RIGHT(bp));
		size += GET_SIZE(node1) + GET_SIZE(node2);
		SET_DATA(node1, PACK(size, 1, 0));
		SET_DATA(FOOTER(TO_PTR(node2)), PACK(size, 1, 0));
		bp = node1;
		if(check_wilderness(bp)){
			insert_free_list((sf_free_list_heads + 7), bp);
		}
		else{
			int index = sf_find_fit(size);
			insert_free_list((sf_free_list_heads + index), bp);
		}
	}
	return bp;
}

sf_block* sf_extend_heap(void){
	char* head;
	if((head = sf_mem_grow()) == NULL){
		sf_errno = ENOMEM;
		return NULL;
	}

	size_t prealloc = GET_PREALLOC(HEADER(head)) >> 1;
	size_t size = PAGE_SZ; // footer and epilogue
	SET_DATA(HEADER(head), PACK(size, prealloc, 0)); //need to add footer
	SET_DATA(FOOTER(head), PACK(size, prealloc, 0));
	/* Epilogue */
	SET_DATA((sf_mem_end() - WSIZE), PACK(0, 0, 1));
	return sf_coalesce((sf_block*)HEADER(head));
}

int sf_init(void){
	char* head;
	if((head = sf_mem_grow()) == NULL){
		sf_errno = ENOMEM;
		return -1;
	}
	char* wilderness_header;
	char* wilderness_footer;
	size_t size_free;
	sf_init_free_list();
	/* Prologue */
	SET_DATA((head + (1*WSIZE)), PACK(MIN_BLOCK_SIZE, 0, 1));
	/* Header and Footer for Wildernexx Block */
	wilderness_header = (head + WSIZE + MIN_BLOCK_SIZE);
	size_free = PAGE_SZ - MIN_BLOCK_SIZE - (2 * WSIZE);
	SET_DATA(wilderness_header, PACK(size_free, 1, 0));
	wilderness_footer = FOOTER((wilderness_header + WSIZE));

	SET_DATA(wilderness_footer, PACK(size_free, 1, 0));
	SET_PREV(wilderness_header, sf_free_list_heads+7);
	SET_NEXT(wilderness_header, sf_free_list_heads+7);
	SET_PREV((sf_free_list_heads+7), wilderness_header);
	SET_NEXT((sf_free_list_heads+7), wilderness_header);
	/* Epilogue */
	SET_DATA((sf_mem_end() - WSIZE), PACK(0, 0, 1));
	return 0;
}

int sf_find_fit(size_t size){
	size_t temp;
	int counter = 0;
	if(size <= 32){
		return 0;
	}
	temp = size/32;
	if(size > (temp * 32)) // base case
		temp++;
	if(temp > 32) // blocks bigger than 32M
		return 6;
	size_t store = temp;
	while(temp > 0){
		temp = temp >> 1;
		counter++;
	}
	if(store == (1<<(counter - 1)))
		counter--;
	return counter;
}