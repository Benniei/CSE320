/*
 * BIRP: Binary decision diagram Image RePresentation
 */

#include "image.h"
#include "bdd.h"
#include "const.h"
#include "debug.h"
#include "help.h"

int pgm_to_birp(FILE *in, FILE *out) { //done
    // TO BE IMPLEMENTED
    int wp = 0;
    int hp = 0;
    unsigned char *raster = raster_data;
    size_t size = RASTER_SIZE_MAX;
    help_clearrasterdata();
    int res = img_read_pgm(in, &wp, &hp, raster, size);

    if(res == 0){
        BDD_NODE* a = bdd_from_raster(wp, hp, raster);
        help_clearindexmap();
        img_write_birp(a, wp, hp, out);
        return 0;
    }
    return -1;
}

int birp_to_pgm(FILE *in, FILE *out) { //done
    // TO BE IMPLEMENTED
    int wp, hp;
    //printf("P5\n# CREATOR: The GIMP's PNM Filter Version 1.0\n130 130\n255\n");
    help_clearindexmap();
    BDD_NODE* root = img_read_birp(in, &wp, &hp); //deserialize the birp
    if(root == NULL){
        return -1;
    }
    help_clearrasterdata();
    unsigned char *raster = raster_data;
    bdd_to_raster(root, wp, hp, raster);
    img_write_pgm(raster, wp, hp, out);
    //testing with comments
    // for(int i = 0; i < wp; i++){
    //     for(int j = 0; j < hp; j++){
    //         fputc(*(raster++), out);
    //     }
    // }
    return 0;
}

int birp_to_birp(FILE *in, FILE *out) {
    // TO BE IMPLEMENTED
    int wp, hp;
    help_clearindexmap();
    BDD_NODE* root = img_read_birp(in, &wp, &hp);
    if(root == NULL){
        return -1;
    }

    int command = global_options & 0xf00;
    int value;

    if(command == 0x100){ //negative
        bdd_map(root, negative_mask);
    }
    else if(command == 0x200){ //threshhold
        bdd_map(root, threshold_mask);
    }
    else if(command == 0x300){ //zoom
        value = global_options & 0xff0000;
        if((value & 0x800000) > 0){ //negative zoom
            value = value | 0xff000000;
            //printf("zoom value negs: %x\n", value);
        }
        value = value >> 16;
        //printf("zoom value: %d\n", value);
    }
    else if(command == 0x400){ //rotate

    }

    return 0;
}

int pgm_to_ascii(FILE *in, FILE *out) { //done
    // TO BE IMPLEMENTED

    int wp = 0;
    int hp = 0;
    unsigned char *raster = raster_data;
    size_t size = RASTER_SIZE_MAX;

    int res = img_read_pgm(in, &wp, &hp, raster, size);
//    printf("\nWP: %d\n", wp);
//    printf("HP: %d\n", hp);
//    printf("Raster: %d\n", *raster);

    if(res == 0){
        for(int j = 0; j < hp; j++){
            for(int i = 0; i < wp; i++){
                int a = *raster++;
                if(a >= 0 && a <= 63){
                    fputc(' ', out);
                }
                else if(a >= 64 && a <= 127){
                    fputc('.', out);
                }
                else if(a >= 128 && a <= 191){
                    fputc('*', out);
                }
                else if(a >= 192 && a <= 255){
                    fputc('@', out);
                }
            }
            fputc('\n', out);
        }
        return 0;
    }
    return -1;
}

int birp_to_ascii(FILE *in, FILE *out) { //done
    // TO BE IMPLEMENTED
    int wp, hp;
    help_clearindexmap();
    BDD_NODE* root = img_read_birp(in, &wp, &hp); //deserialize the birp
    if(root == NULL){
        return -1;
    }
    ;
    help_clearrasterdata();
    unsigned char *raster = raster_data;
    bdd_to_raster(root, wp, hp, raster);

    for(int j = 0; j < hp; j++){
            for(int i = 0; i < wp; i++){
                int a = *raster++;
                if(a >= 0 && a <= 63){
                    fputc(' ', out);
                }
                else if(a >= 64 && a <= 127){
                    fputc('.', out);
                }
                else if(a >= 128 && a <= 191){
                    fputc('*', out);
                }
                else if(a >= 192 && a <= 255){
                    fputc('@', out);
                }
            }
            fputc('\n', out);
        }
    return 0;
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
                    // -i specified but there is nothing following -> -1
                    if(argc == 2){
                        return -1;
                    }
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
                    else{
                        return -1;
                    }
                }
                // equals to -o
                else{
                    output_flag = 1;
                    // output type pgm (0x10)
                    // -o specified but there is nothing following -> -1
                    if(argc == 2){
                        return -1;
                    }
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
                    // -i specified but there is nothing following -> -1
                    if(argc == counter + 1){
                        return -1;
                    }
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
                    else{
                        return -1;
                    }
                }
                // equals to -o and no output previously
                else if(help_strcmp(*(argv + counter), "-o") && (output_flag == 0)){
                    output_flag = 1;
                    // -o specified but there is nothing following -> -1
                    if(argc == counter + 1){
                        return -1;
                    }
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
                    if(argc == counter){
                        return -1;
                    }
                    int value = help_strtoint(*(argv + counter)); //check value
                    if(value >= 0 && value <= 255){
                        value = value << 16;
                        global_options += value;
                        counter++;
                    }
                    else{
                        return -1;
                    }
                }
                // -z -> zoom negative
                else if(help_strcmp(*(argv + counter), "-z")){
                    global_options += 0x300;
                    counter++; //need another increment to account for argument
                    if(argc == counter){
                        return -1;
                    }
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
                    if(argc == counter){
                        return -1;
                    }
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
    //default for no arguments
    else{
        global_options += 0x22;
        return 0;
    }
    return -1;
}
