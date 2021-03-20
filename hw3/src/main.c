#include <stdio.h>
#include "help.h"
#include "sfmm.h"

int main(int argc, char const *argv[]) {

	 char * ptr1 = sf_malloc(50 * sizeof(double));
    *(ptr1) = 'A';

    sf_show_blocks();
    printf("\n");

    char * ptr2 = sf_malloc(78 * sizeof(double));
    *(ptr2) = 'A';

    sf_show_blocks();
    printf("\n");

    char * ptr3 = sf_malloc(1 * sizeof(double));
    *(ptr3) = 'A';

    sf_show_blocks();
    printf("\n");

    ptr1 = sf_realloc(ptr1, 300);

    sf_show_blocks();
    printf("\n");

    ptr2 = sf_realloc(ptr2, 640);

    sf_show_blocks();
    printf("\n");

    sf_free(ptr1);
    sf_show_blocks();
    printf("\n");

    ptr2 = sf_realloc(ptr2, 300);
    sf_show_blocks();
    printf("\n");


    sf_free(ptr2);
    sf_show_blocks();
    printf("\n");

    char * ptr4 = sf_malloc(7048);
    *(ptr4) = 'A';

    sf_show_blocks();
    printf("\n");

    char * ptr5 = sf_malloc(6000);
    *(ptr5) = 'A';

    sf_show_blocks();
    printf("\n");

    char * ptr6 = sf_malloc(2168);
    *(ptr6) = 'A';

    sf_show_blocks();
    printf("\n");

    ptr6 = sf_realloc(ptr6, 2000);
    sf_show_blocks();
    printf("\n");

    ptr6 = sf_realloc(ptr6, 2000);
    sf_show_blocks();
    printf("\n");


    char * ptr7 = sf_malloc(1000);
    *(ptr7) = 'A';
    sf_show_blocks();
    printf("\n");
    printf("---------------------------------------------------------------------\n");
    char * ptr8 = sf_realloc(ptr7, 1048);
    *(ptr8) = 'A';
    sf_show_heap();
    printf("\n-------------------------------------------------------------------\n");
    return EXIT_SUCCESS;
}
