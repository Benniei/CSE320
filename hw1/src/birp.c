/*
 * BIRP: Binary decision diagram Image RePresentation
 */

#include "image.h"
#include "bdd.h"
#include "const.h"
#include "debug.h"
#include "help.h"

int pgm_to_birp(FILE *in, FILE *out) {
    // TO BE IMPLEMENTED
    return -1;
}

int birp_to_pgm(FILE *in, FILE *out) {
    // TO BE IMPLEMENTED
    return -1;
}

int birp_to_birp(FILE *in, FILE *out) {
    // TO BE IMPLEMENTED
    return -1;
}

int pgm_to_ascii(FILE *in, FILE *out) {
    // TO BE IMPLEMENTED
    return -1;
}

int birp_to_ascii(FILE *in, FILE *out) {
    // TO BE IMPLEMENTED
    return -1;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specifed will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere int the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 */
int validargs(int argc, char **argv) {
    // TO BE IMPLEMENTED

    //test code
    // test();

    // printf("%s \n", *argv);
    // printf("%s \n", *(argv + 1));
    // printf("%i \n", argc);
    // char* one = "hello";
    // char* two = "yello";
    // if(help_strcmp("hello", "yello") < 0){
    //     test();
    // }
    // else{
    //     printf("not it");
    // }

    int counter = 1; //counts the arguments used up
    int input_flag = 0; //sees if there is an input
    int output_flag = 0; //sees if there is an output
    if(argc > 1){

        //checks for help flag
        if(help_strcmp(*(argv + 1), "-h")){
            global_options = 0x80000000;
            return 0;
        }
        //checks for other flags
        else{
            // input/output flag check1
            if(help_strcmp(*(argv + counter), "-i") || help_strcmp(*(argv + counter), "-o")){
                // equals to -i
                if(help_strcmp(*(argv + counter), "-i")){
                    input_flag = 1;
                    // input type pgm (0x1)
                    if(help_strcmp(*(argv + counter + 1), "pgm")){
                        global_options = global_options + 0x1;
                        counter += 2;
                    }
                    // input type birp (0x2)
                    else if(help_strcmp(*(argv + counter + 1), "birp")){
                        global_options = global_options + 0x2;
                        counter += 2;
                    }
                    // -i specified but there is nothing following -> -1
                    else{
                        return -1;
                    }
                }
                // equals to -o
                else{
                    output_flag = 1;
                    // output type pgm (0x10)
                    if(help_strcmp(*(argv + counter + 1), "pgm")){
                        global_options = global_options + 0x10;
                        counter += 2;
                    }
                    // output type birp (0x20)
                    else if(help_strcmp(*(argv + counter + 1), "birp")){
                        global_options = global_options + 0x20;
                        counter += 2;
                    }
                    // output type ascii (0x30)
                    else if(help_strcmp(*(argv + counter + 1), "ascii")){
                        global_options = global_options + 0x30;
                        counter += 2;
                    }
                    // -o specified but there is nothing following -> -1
                    else{
                        return -1;
                    }
                }
            }

            //TODO: check the counter

            //second check for input output
            if(help_strcmp(*(argv + counter), "-i") || help_strcmp(*(argv + counter), "-o")){
                // equals to -i and no input previously
                if(help_strcmp(*(argv + counter), "-i") && (input_flag == 0)){
                    input_flag = 1;
                    // input type pgm (0x1)
                    if(help_strcmp(*(argv + counter + 1), "pgm")){
                        global_options = global_options + 0x1;
                        counter += 2;
                    }
                    // input type birp (0x2)
                    else if(help_strcmp(*(argv + counter + 1), "birp")){
                        global_options = global_options + 0x2;
                        counter += 2;
                    }
                    // -i specified but there is nothing following -> -1
                    else{
                        return -1;
                    }
                }
                // equals to -o and no output previously
                else if(help_strcmp(*(argv + counter), "-o") && (output_flag = 0)){
                    output_flag = 1;
                    // output type pgm (0x10)
                    if(help_strcmp(*(argv + counter + 1), "pgm")){
                        global_options = global_options + 0x10;
                        counter += 2;
                    }
                    // output type birp (0x20)
                    else if(help_strcmp(*(argv + counter + 1), "birp")){
                        global_options = global_options + 0x20;
                        counter += 2;
                    }
                    // output type ascii (0x30)
                    else if(help_strcmp(*(argv + counter + 1), "ascii")){
                        global_options = global_options + 0x30;
                        counter += 2;
                    }
                    // -o specified but there is nothing following -> -1
                    else{
                        return -1;
                    }
                }
            }
            //TODO: check the counter
            //TODO: check the flags for input and output for defaults

            //TODO: checks for other operations

            printf("\n 0x%x \n", global_options);

        }
    }
    return -1;
}
