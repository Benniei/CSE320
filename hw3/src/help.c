#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "sfmm.h"
#include "help.h"

int sf_init_free_list(){
	for(int i = 0; i < 8; i++){
		sf_free_list_heads[i].body.links.prev = &sf_free_list_heads[i];
		sf_free_list_heads[i].body.links.next = &sf_free_list_heads[i];
	}
	return 0;
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

