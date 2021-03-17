#include <stdio.h>
#include "help.h"
#include "sfmm.h"

int main(int argc, char const *argv[]) {
	// printf("Show Blocks\n");
	sf_show_blocks();
	//printf("Show Heap\n");
	// sf_show_heap();
	//printf("Show Free List\n");
	//sf_show_free_lists();
	//printf("Show Blocks\n");
	sf_init();
	sf_show_blocks();
	sf_show_free_lists();
	// printf("Start Address:\n");
	// printf("%p\n", sf_mem_start());
	// printf("End Address:\n");
	// printf("%p\n", sf_mem_end());

    double* ptr = sf_malloc(sizeof(double));

    *ptr = 320320320e-320;

    printf("%f\n", *ptr);

    sf_free(ptr);

    return EXIT_SUCCESS;
}
