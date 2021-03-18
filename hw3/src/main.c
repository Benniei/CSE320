#include <stdio.h>
#include "help.h"
#include "sfmm.h"

int main(int argc, char const *argv[]) {
	// printf("Show Blocks\n");
	// sf_show_blocks();
	// printf("Show Heap\n");
	// sf_show_heap();
	// printf("Show Free List\n");
	// sf_show_free_lists();
	// printf("Show Blocks\n");
	// printf("Start Address:\n");
	// printf("%p\n", sf_mem_start());
	// printf("End Address:\n");
	// printf("%p\n", sf_mem_end());

	// sf_init();
 //    printf("End Address:\n");
 //    printf("%p\n", sf_mem_end());
 //    sf_extend_heap();
 //    sf_show_heap();


	// printf("End Address:\n");
	// printf("%p\n", sf_mem_end());
	// sf_show_heap();
	// sf_block* head = (sf_free_list_heads + 7);
	// sf_block* node = (sf_block*)TO_PTR(GET_NEXT(head));
	// sf_show_block((sf_block*)HEADER(node));
	// printf("\n");
	// printf("node: %p\n", node);
	// sf_coalesce(node);
	return 0;

    double* ptr = sf_malloc(sizeof(double));
    sf_show_heap();

    *ptr = 320320320e-320;

    printf("%f\n", *ptr);

    sf_free(ptr);

    return EXIT_SUCCESS;
}
