#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"
#include "help.h"

int sf_init(void){
	char *head;
	if((head = sf_mem_grow()) == NULL)
		return -1;
	WRITE_DATA(head, 0);


	return 0;
}