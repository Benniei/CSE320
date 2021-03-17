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
	//size_t asize; // adjusted block size
	//char* bp;

	if(size == 0)
		return NULL;

	//asize = ALIGN((size + DSIZE)); /* add header and footer */

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
