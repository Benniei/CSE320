#include <stdio.h>
#include "help.h"
#include "sfmm.h"

int main(int argc, char const *argv[]) {
	char * ptr1 = sf_malloc(1);
    *(ptr1) = 'A';

    //sf_show_blocks();
    printf("\n");

    char * ptr2 = sf_malloc(1);
    *(ptr2) = 'B';

    //sf_show_blocks();
    printf("\n");

    int * ptr3 = sf_malloc(24 * sizeof(int));
    *(ptr3 + 0) = 1;
    *(ptr3 + 1) = 69;
    *(ptr3 + 2) = 80;
    *(ptr3 + 23) = 69;
    //sf_show_heap();
    sf_free(ptr1);
    //sf_show_heap();
    sf_free(ptr2);
    printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    sf_show_heap();
    printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    sf_free(ptr3);
    sf_show_heap();
    printf("\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    printf("\n");
    return EXIT_SUCCESS;
}
