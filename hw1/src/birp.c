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
    global_options = 0; //initilize global variable

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
                        global_options += 0x1;
                        counter += 2;
                    }
                    // input type birp (0x2)
                    else if(help_strcmp(*(argv + counter + 1), "birp")){
                        global_options += 0x2;
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
                        global_options += 0x10;
                        counter += 2;
                    }
                    // output type birp (0x20)
                    else if(help_strcmp(*(argv + counter + 1), "birp")){
                        global_options += 0x20;
                        counter += 2;
                    }
                    // output type ascii (0x30)
                    else if(help_strcmp(*(argv + counter + 1), "ascii")){
                        global_options += 0x30;
                        counter += 2;
                    }
                    // -o specified but there is nothing following -> -1
                    else{
                        return -1;
                    }
                }
            }


            // checks args counter
            if(counter == argc){
                // default
                if(input_flag == 0){
                global_options += 0x2;
                }
                if(output_flag == 0){
                    global_options += 0x20;
                }
                return 0;
            }

            // second check for input output
            if(help_strcmp(*(argv + counter), "-i") || help_strcmp(*(argv + counter), "-o")){
                // equals to -i and no input previously
                if(help_strcmp(*(argv + counter), "-i") && (input_flag == 0)){
                    input_flag = 1;
                    // input type pgm (0x1)
                    if(help_strcmp(*(argv + counter + 1), "pgm")){
                        global_options += 0x1;
                        counter += 2;
                    }
                    // input type birp (0x2)
                    else if(help_strcmp(*(argv + counter + 1), "birp")){
                        global_options += 0x2;
                        counter += 2;
                    }
                    // -i specified but there is nothing following -> -1
                    else{
                        return -1;
                    }
                }
                // equals to -o and no output previously
                else if(help_strcmp(*(argv + counter), "-o") && (output_flag == 0)){
                    output_flag = 1;
                    // output type pgm (0x10)
                    if(help_strcmp(*(argv + counter + 1), "pgm")){
                        global_options += 0x10;
                        counter += 2;
                    }
                    // output type birp (0x20)
                    else if(help_strcmp(*(argv + counter + 1), "birp")){
                        global_options += 0x20;
                        counter += 2;
                    }
                    // output type ascii (0x30)
                    else if(help_strcmp(*(argv + counter + 1), "ascii")){
                        global_options += 0x30;
                        counter += 2;
                    }
                    // -o specified but there is nothing following -> -1
                    else{
                        return -1;
                    }
                }
            }

            // check the flags for input and output for defaults
            if(input_flag == 0){
                global_options += 0x2;
            }
            if(output_flag == 0){
                global_options += 0x20;
            }
            // check the counter
            if(counter == argc){
                return 0;
            }

            //checks for other operations
            // -n -> negative
            if(global_options == 0x22){
                if(help_strcmp(*(argv + counter), "-n")){
                    global_options += 0x100;
                    counter++;
                    //complete
                }
                // -t -> threshold
                else if(help_strcmp(*(argv + counter), "-t")){
                    global_options += 0x200;
                    counter++; //need another increment to account for argument
                    int value = help_strtoint(*(argv + counter)); //check value
                    printf("\n Value t: %d \n", value);
                    if(value >= 0 && value <= 255){
                        value = value << 16;
                        global_options += value;
                        counter++;
                        //complete (untested)
                    }
                    else{
                        return -1;
                    }
                }
                // -z -> zoom negative
                else if(help_strcmp(*(argv + counter), "-z")){
                    global_options += 0x300;
                    counter++; //need another increment to account for argument
                    int value = help_strtoint(*(argv + counter)); //might need to change some bits
                    if(value >= 0 && value <= 16){
                        value *= -1; //negative value
                        value = value << 16; //shift bits to the left
                        value = value & 0x00ff0000; // and bits to get rid of things on the left
                        global_options += value;
                        counter++;
                    }
                    else{
                        return -1;
                    }
                }
                // -Z -> zoom positive
                else if(help_strcmp(*(argv + counter), "-Z")){
                    global_options += 0x300;
                    counter++; //need another increment to account for argument
                    int value = help_strtoint(*(argv + counter));
                    if(value >= 0 && value <= 16){
                        value = value << 16;
                        global_options += value;
                        counter++;
                    }
                }
                // -r rotate
                else if(help_strcmp(*(argv + counter), "-r")){
                    global_options += 0x400;
                    counter++;
                    //complete
                }
            }
            //check if there are any arguments left (should not be any)
            if(counter == argc){
                return 0;
            }
        }
    }
    //TODO: IMPLEMENT BIN/BIRP
    else{
        global_options += 0x22;
        return 0;
    }
    return -1;
}
