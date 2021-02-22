#include <stdio.h>
#include <stdlib.h>

#include "const.h"
#include "debug.h"
#include "help.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    if(validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);
    if(global_options & HELP_OPTION)
        USAGE(*argv, EXIT_SUCCESS);
    // TO BE IMPLEMENTED

    int command_options = global_options & 0xff; // filters the commands
    int success_flag;

    if(command_options == 0x11){ // pgm to pgm
    	// do not see the option?
        USAGE(*argv, EXIT_FAILURE);
    }
    else if(command_options == 0x21){ // pgm to birp
        help_inithashtonull();
        success_flag = pgm_to_birp(stdin, stdout);
        if(success_flag == -1){
            return EXIT_FAILURE;
        }

    }
    else if(command_options == 0x31){ // pgm to ascii
    	success_flag = pgm_to_ascii(stdin, stdout);
        if(success_flag == -1){
            return EXIT_FAILURE;
        }

    }
    else if(command_options == 0x12){ // birp to pgm
        help_inithashtonull();
        success_flag = birp_to_pgm(stdin, stdout);
        if(success_flag == -1){
            return EXIT_FAILURE;
        }
    }
    else if(command_options == 0x22){ // birp to birp
        help_inithashtonull();
        if(global_options == 0x22){ // no flags are specified
            USAGE(*argv, EXIT_FAILURE);
        }
        success_flag = birp_to_birp(stdin, stdout);
        if(success_flag == -1){
            return EXIT_FAILURE;
        }
    }
    else if(command_options == 0x32){ // birp to ascii
        help_inithashtonull();
        success_flag = birp_to_ascii(stdin, stdout);
        if(success_flag == -1){
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
