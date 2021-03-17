#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"

/* Address alignments */
#define MIN_BLOCK_SIZE 32 //minimum block size (free)
#define WSIZE 8 //size of header and footer
#define DSIZE 16 //16 byte alignment
#define ALIGN(size) ((size + (DSIZE - 1)) & ~0xF)
#define POWERTWO(num) (num != 0) && ((num & (num - 1)) == 0)

/* Pack a size and word */
#define PACK(data, prealloc, alloc) (data + (prealloc<<1) + alloc)

/*Read and write a word/address at p */
#define READ_DATA(p) (*(size_t *)(p)) //use size_t because 8 bytes
#define WRITE_DATA(p, data) (*(size_t *)(p) = (data))
// #define READ_ADDRESS(p) (*p)
// #define WRITE_ADDRESS(p, address) (*p = address)
#define SET_DATA(p, data) (*(sf_block *)p).header = (data)

/* Read size and get allocated fields */
#define GET_SIZE(p) (READ_DATA(p) & ~0xF)
#define GET_PREALLOC(p) (READ_DATA(p) & 0x2)
#define GET_ALLOC(p) (READ_DATA(p) & 0x1)

/* Get address of Header and Footer */
#define HEADER(p) ((char*)p - WSIZE)
#define FOOTER(p) (((char*)(p) + GET_SIZE(HEADER((char *)(p)))) - DSIZE)

/* Heap -> gives pointers to header of adjacent blocks*/
#define LEFT(p) (char*)p - GET_SIZE(HEADER((char *)p) - DSIZE) //goes to footer of the one to the left (only for free blocks)
#define RIGHT(p) (char*)p + GET_SIZE(HEADER((char *)p - WSIZE))

/* Links */
#define SET_PREV(p_source, ptr) (*(sf_block *)p_source).body.links.prev = (sf_block *)ptr
#define SET_NEXT(p_source, ptr) (*(sf_block *)p_source).body.links.next = (sf_block *)ptr

/* Functions */
int sf_init(void);
int sf_find_fit(size_t size);