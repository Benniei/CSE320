/*
 * Imprimer: Command-line interface
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <signal.h>

#include "imprimer.h"
#include "conversions.h"
#include "sf_readline.h"
#include "help.h"

FILE* input;
FILE* output;

void job_check();

volatile sig_atomic_t job_complete;

void sigchild_handler(){
    // printf("sighandler\n");
    job_complete = 1;
    int status;
    pid_t pid;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0){
        // printf("pid: %d\n", pid);
        for(int i = 0; i < global_jobptr; i++){
            if(jobs[i].pgid == pid){
                jobs[i].status = JOB_FINISHED;
                sf_job_status(i, jobs[i].status);
                //if(printers[jobs[i].printer_id].status == PRINTER_BUSY)
                    printers[jobs[i].printer_id].status = PRINTER_IDLE;
                sf_job_finished(i, pid);
                sf_printer_status(printers[jobs[i].printer_id].name, printers[jobs[i].printer_id].status);
            }
        }
    }

}

void readline_callback(){
    if(job_complete == 1){
        // printf("callback\n");
        job_check();
        job_complete = 0;
    }
}

int run_cli(FILE *in, FILE *out)
{
    input = in;
    output = out;
    // TO BE IMPLEMENTED
	// fprintf(stderr, "You have to implement run_cli() before the application will function.\n");
    sigset_t mask, prev;
    signal(SIGCHLD, sigchild_handler);
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sf_set_readline_signal_hook(readline_callback);
    int args_counter;
    while(1){
    	char* command;
        char line[256];
        for(int i = 0; i < global_jobptr; i++){
            if(jobs[i].status == JOB_FINISHED || jobs[i].status == JOB_ABORTED){
                jobs[i].status = JOB_DELETED;
                free_single_job(i);
                sf_job_deleted(i);
            }
        }
        if(in == stdin)
            command = sf_readline("imp> ");
        else{
            if(fgets(line, sizeof(line), in) != NULL){
                //fprintf(stdout, "line: %s\n", line);
                command = strtok(line, "\n");
                command = line;
            }
            else{
                printf("\n");
                return 0;
            }
        }
        if(command == NULL){
            return 0;
        }
    	char* token;
        char status_flag = 0;
        size_t cmd_size = strlen(command) + 1; // considers \0
        char* command_cpy = malloc(cmd_size);
        strcpy(command_cpy, command);
        args_counter = num_args(command_cpy);
        free(command_cpy);
    	token = strtok(command, " ");

        /* Miscellaneous commands  */
        if(strcmp(token, "help") == 0){ // DONE
            if(args_counter > 0){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'help\'\n", args_counter, 0);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                fprintf(out,"Commands are: help quit type printer conversion printers jobs print cancel disable enable pause resume\n");
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "quit") == 0){ // DONE
            if(args_counter > 0){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'quit\'\n", args_counter, 0);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                sigprocmask(SIG_BLOCK, &mask, &prev);
                for(int i = 0; i < global_jobptr; i++){
                    int child_status;
                     if(jobs[i].status == JOB_PAUSED){
                    //SIGTERM
                        kill(jobs[i].pgid, SIGTERM);
                    //SIGCONT
                        kill(jobs[i].pgid, SIGCONT);
                    }
                    else if(jobs[i].status == JOB_RUNNING){
                    //SIGTERM
                        kill(jobs[i].pgid, SIGTERM);
                    }
                    waitpid(jobs[i].pgid, &child_status, 0);
                }
                fclose(stdin);
                fclose(stdout);
                fclose(stderr);
                free_names();
                free_job_file();
                sf_cmd_ok();
                if(in == stdin || in == NULL)
                    free(command);
                return -1;
            }
        }
        else if(strcmp(token, "type") == 0){ // DONE
            if(args_counter != 1){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'type\'\n", args_counter, 1);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                char* file_type;
                file_type = strtok(NULL, " ");
                if(define_type(file_type) == NULL){
                    fprintf(out, "Type not defined (ran out of memory)");
                    sf_cmd_error("type (out of memory)");
                    goto end_free;
                }
                sf_cmd_ok();
            }

        }
        /* Configuration Commands */
        else if(strcmp(token, "printer") == 0){ // DONE
            if(args_counter != 2){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'printer\'\n", args_counter, 2);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                char* printer_name;
                char* file_type;
                printer_name = strtok(NULL, " ");
                file_type = strtok(NULL, " ");
                // printf("printer: %s, %s\n", printer_name, file_type);
                if(global_printerct >= MAX_PRINTERS){
                    fprintf(out,"The printer list is full\n");
                    sf_cmd_error("printer (list full)");
                    goto end_free;
                }
                if(find_type(file_type) == NULL){
                    fprintf(out,"Unknown file type: %s\n", file_type);
                    sf_cmd_error("printer (Unknown File Type)");
                    goto end_free;
                }
                for(int i = 0; i < global_printerct; i++){
                    if(strcmp(printers[i].name,printer_name) == 0){
                        fprintf(out,"Printer name \'%s\' not unique\n", printer_name);
                        sf_cmd_error("printer (name not unique)");
                        goto end_free;
                    }
                }
                int pos = global_printerct;
                char* pt_name = malloc(strlen(printer_name) + 1);
                strcpy(pt_name, printer_name);
                add_printer(pt_name, file_type);
                sf_printer_defined(pt_name, file_type);
                fprintf(out,"PRINTER: id=%d, name=%s, type=%s, status=%s\n", printers[pos].id, printers[pos].name, printers[pos].type->name, printer_status_names[printers[pos].status]);
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "conversion") == 0){ // DONE
            if(args_counter < 3){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'conversion\'\n", args_counter, 3);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                char* file_type1;
                char* file_type2;
                char* conversion_pgm[args_counter - 1];
                file_type1 = strtok(NULL, " ");

                if(find_type(file_type1) == NULL){
                    fprintf(out,"Unknown file type: %s\n", file_type1);
                    sf_cmd_error("conversion (Unknown File Type)");
                    goto end_free;
                }

                file_type2 = strtok(NULL, " ");

                if(find_type(file_type1) == NULL){
                    fprintf(out,"Unknown file type: %s\n", file_type1);
                    sf_cmd_error("conversion (Unknown File Type)");
                    goto end_free;
                }

                int i = 0;
                token = strtok(NULL, " ");
                while(token != NULL){

                    conversion_pgm[i] = token;
                    token = strtok(NULL, " ");
                    i++;
                }
                conversion_pgm[i] = NULL;
                //printf("conversion: %s, %s, %s..\n", file_type1, file_type2, *conversion_pgm);
                define_conversion(file_type1, file_type2, conversion_pgm);

                // CONVERSION** owo=  find_conversion_path("aaa", file_type2);
                // int num = check_conversion("aaa",file_type2);
                // printf("%p %d\n", owo, num);

                sf_cmd_ok();
            }
        }
        /* Informational Commands */
        else if(strcmp(token, "printers") == 0){ // DONE
            if(args_counter > 0){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'printers\'\n", args_counter, 0);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                for(int i = 0; i < global_printerct; i++){
                    fprintf(out,"PRINTER: id=%d, name=%s, type=%s, status=%s\n", printers[i].id, printers[i].name, printers[i].type->name, printer_status_names[printers[i].status]);
                }
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "jobs") == 0){
            if(args_counter > 0){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'jobs\'\n", args_counter, 0);
                sf_cmd_error("arg count");
            }
            else{
                for(int i = 0; i < global_jobptr; i++){
                    if(jobs[i].status == JOB_CREATED){
                        fprintf(out,"JOB[%d]: type=%s, creation(%.19s), status(%.19s)=%s, eligible=%08x, file=%s\n", i, jobs[i].type->name, ctime(&jobs[i].create_time),
                            ctime(&jobs[i].create_time), job_status_names[jobs[i].status], jobs[i].eligible, jobs[i].file_name);
                    }
                    else{
                        fprintf(out,"JOB[%d]: type=%s, creation(%.19s), status(%.19s)=%s, eligible=%08x, file=%s, pgid=%d, printer=%s\n", i, jobs[i].type->name, ctime(&jobs[i].create_time),
                            ctime(&jobs[i].create_time), job_status_names[jobs[i].status], jobs[i].eligible, jobs[i].file_name, jobs[i].pgid, printers[jobs[i].printer_id].name);
                    }
                }
                sf_cmd_ok();
            }
        }
        /* Spooling commands */
        else if(strcmp(token, "print") == 0){
            if(args_counter < 1){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'print\'\n", args_counter, 1);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                char* file_name;
                char* printer;
                file_name = strtok(NULL, " ");
                FILE_TYPE * file_type = infer_file_type(file_name);
                if(global_jobfill >= MAX_JOBS){
                    fprintf(out,"The jobs list is full\n");
                    sf_cmd_error("print (list full)");
                    goto end_free;
                }
                if(file_type == NULL){
                    fprintf(out,"Unable to infer file type\n");
                    sf_cmd_error("print (Unknown File Type)");
                    goto end_free;
                }
                //time
                time_t t;
                time(&t);
                char* jb_name = malloc(strlen(file_name) + 1);
                strcpy(jb_name, file_name);
                int pos = add_job(jb_name, file_type, t);

                int counter = 0;
                printer = strtok(NULL, " ");
                if(printer != NULL)
                    jobs[pos].eligible = 0;
                while(printer != NULL){
                    int printer_pos = find_printer(printer);
                    if(printer_pos == -1){
                        fprintf(out, "Invalid printer \'%s\'", printer);
                        printer = strtok(NULL, " ");
                        continue;
                    }
                    jobs[pos].eligible = jobs[pos].eligible | (1<<printer_pos);
                    jobs[pos].eligible_printers[counter++] = printers[printer_pos].name; //findprinter
                    printer = strtok(NULL, " ");
                }
                jobs[pos].num_eligible = counter;
                status_flag = 1;
                // printf("\n current time is : %s",ctime(&t));

                printf("JOB[%d]: type=%s, creation(%.19s), status(%.19s)=%s, eligible=%08x, file=%s\n", pos, file_type->name, ctime(&jobs[pos].create_time),
                    ctime(&jobs[pos].create_time), job_status_names[jobs[pos].status], jobs[pos].eligible, jobs[pos].file_name);
                sf_job_created(pos, file_name, file_type->name);
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "cancel") == 0){
            if(args_counter != 1){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'cancel\'\n", args_counter, 1);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                int job_number;
                job_number = strtoint(strtok(NULL, " "));
                // printf("cancel: %d\n", job_number);
                if(job_number >= global_printerct){
                    fprintf(out,"Job [%d] is out of range\n", job_number);
                    sf_cmd_error("cancel (invalid job)");
                    goto end_free;
                }
                if(jobs[job_number].status == JOB_DELETED){
                    fprintf(out,"Job [%d] does not exist\n", job_number);
                    sf_cmd_error("cancel (invalid job)");
                    goto end_free;
                }
                if(jobs[job_number].status == JOB_PAUSED){
                    //SIGTERM
                    kill(jobs[job_number].pgid, SIGTERM);
                    //SIGCONT
                    kill(jobs[job_number].pgid, SIGCONT);
                }
                else if(jobs[job_number].status == JOB_RUNNING){
                    //SIGTERM
                    kill(jobs[job_number].pgid, SIGTERM);
                }
                jobs[job_number].status = JOB_ABORTED;
                sf_job_status(job_number, jobs[job_number].status);
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "pause") == 0){
            if(args_counter != 1){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'pause\'\n", args_counter, 1);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                int job_number;
                job_number = strtoint(strtok(NULL, " "));
                // printf("pause: %d\n", job_number);
                if(job_number >= global_printerct){
                    fprintf(out,"Job [%d] is out of range\n", job_number);
                    sf_cmd_error("pause (invalid job)");
                    goto end_free;
                }
                if(jobs[job_number].status == JOB_DELETED){
                     fprintf(out,"Job [%d] does not exist\n", job_number);
                    sf_cmd_error("pause (invalid job)");
                    goto end_free;
                }
                //SIGNAL SIGSTOP (READ DOC)
                kill(jobs[job_number].pgid, SIGSTOP);
                jobs[job_number].status = JOB_PAUSED;
                sf_job_status(job_number, jobs[job_number].status);
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "resume") == 0){
            if(args_counter != 1){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'resume\'\n", args_counter, 1);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                int job_number;
                job_number = strtoint(strtok(NULL, " "));
                // printf("resume: %d\n", job_number);
                if(job_number >= global_printerct){
                    fprintf(out,"Job [%d] is out of range\n", job_number);
                    sf_cmd_error("resume (invalid job)");
                    goto end_free;
                }
                if(jobs[job_number].status == JOB_DELETED){
                     fprintf(out,"Job [%d] does not exist\n", job_number);
                    sf_cmd_error("resume (invalid job)");
                    goto end_free;
                }
                jobs[job_number].status = JOB_RUNNING;
                sf_job_status(job_number, jobs[job_number].status);
                //Send SIGCONT
                kill(jobs[job_number].pgid, SIGCONT);
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "disable") == 0){
            if(args_counter != 1){
                fprintf(out,"Wrong number of args(given: %d, required: %d) for CLI command \'disable\'\n", args_counter, 1);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                char* printer_name;
                printer_name = strtok(NULL, " ");
                // printf("disable: %s\n", printer_name);
                int pos = find_printer(printer_name);
                if(pos == -1){
                    fprintf(out,"Printer \'%s\' not found\n", printer_name);
                    sf_cmd_error("disable (printer not found)");
                    goto end_free;
                }
                printers[pos].status = PRINTER_DISABLED;
                fprintf(out,"PRINTER: id=%d, name=%s, type=%s, status=%s\n", printers[pos].id, printers[pos].name, printers[pos].type->name, printer_status_names[printers[pos].status]);
                sf_printer_status(printer_name, printers[pos].status);
                sf_cmd_ok();
            }
        }
        else if(strcmp(token, "enable") == 0){
            if(args_counter != 1){
                fprintf(out, "Wrong number of args(given: %d, required: %d) for CLI command \'enable\'\n", args_counter, 1);
                sf_cmd_error("arg count");
                goto end_free;
            }
            else{
                char* printer_name;
                printer_name = strtok(NULL, " ");
                int pos = find_printer(printer_name);
                // printf("disable: %s\n", printer_name);
                if(pos == -1){
                    fprintf(out,"Printer \'%s\' not found\n", printer_name);
                    sf_cmd_error("enable (printer not found)");
                    goto end_free;
                }
                printers[pos].status = PRINTER_IDLE;
                status_flag = 1;
                fprintf(out,"PRINTER: id=%d, name=%s, type=%s, status=%s\n", printers[pos].id, printers[pos].name, printers[pos].type->name, printer_status_names[printers[pos].status]);
                sf_printer_status(printer_name, printers[pos].status);
                sf_cmd_ok();
            }
        }
        /* Unrecognized Command */
        else{
            fprintf(out,"Unrecognized Command: %s\n", token);
            sf_cmd_error("unrecognized command");
            goto end_free;
        }
        end_free:
        if(in == stdin || in == NULL)
            free(command);
        if(status_flag == 1){
            job_check();
        }
    }

    abort();
}

int match_job_to_ptr(int i){ // i is the job id
    int num_conv = -1;
    int other_conv = -1;
    char flag = 0;
    if(jobs[i].status != JOB_CREATED)
        return -1;
    if(jobs[i].num_eligible == 0){
        // Check through all the printers
        for(int k = 0; k < global_printerct; k++){
            // printf("use global\n");
            if(printers[k].status != PRINTER_IDLE)
                continue;

            // check if conversion exist
            num_conv = check_conversion(jobs[i].type->name, printers[k].type->name);

            if(num_conv == -1)
                continue;
            if(flag == 0){
                flag = 1;
                other_conv = num_conv;
                jobs[i].printer_id = k;
                jobs[i].num_conversions = num_conv;
            }
            if(num_conv < other_conv){
                jobs[i].printer_id = k;
                jobs[i].num_conversions = num_conv;
                other_conv = num_conv;
            }
        }
    }
    else{
        for(int k = 0; k < jobs[i].num_eligible; k++){
            // printf("use eligible\n");
            int pos = find_printer(jobs[i].eligible_printers[k]);
            if(pos == -1)
                continue;
            if(printers[pos].status != PRINTER_IDLE)
                continue;
            // check if conversion exist
            num_conv = check_conversion(jobs[i].type->name, printers[pos].type->name);
            if(num_conv == -1)
                continue;
            if(flag == 0){
                flag = 1;
                other_conv = num_conv;
                jobs[i].printer_id = pos;
                jobs[i].num_conversions = num_conv;
            }
            if(num_conv < other_conv){
                jobs[i].printer_id = pos;
                jobs[i].num_conversions = num_conv;
                other_conv = num_conv;
            }
        }
    }
    return other_conv;
}

void job_check(){
    int i;
    int num_conv;
    for(i = 0; i < global_jobptr; i++){
        num_conv = match_job_to_ptr(i);
        // top gives you number of conversions and sets the fields of jobs
        // fprintf(out, "Number conversion: %d\n", num_conv);
        if(num_conv != -1){
            // printf("Job[%d] matched with %s (Conversions: %d)\n", i, printers[jobs[i].printer_id].name, jobs[i].num_conversions);
            // forking and pipelining
            // remember to free file name before deleting
            pid_t pid = 0;
            int child_status;
            jobs[i].status = JOB_RUNNING;
            sf_job_status(jobs[i].id, JOB_RUNNING);
            sf_printer_status(printers[jobs[i].printer_id].name, PRINTER_BUSY);
            printers[jobs[i].printer_id].status = PRINTER_BUSY;
            if((jobs[i].pgid = fork()) == 0){ // Child Process
                //signals

                setpgid(pid, 0);
                int printer_fd = imp_connect_to_printer(printers[jobs[i].printer_id].name, printers[jobs[i].printer_id].type->name, PRINTER_NORMAL);
                if(printer_fd == -1){
                    free_names();
                    free_job_file();
                    conversions_fini();
                    fprintf(stderr, "unable to connect to printer");
                    exit (1);
                }

                // same type to same type
                if(num_conv == 0){
                    int fd[2];// 0 is read, 1 is write
                    char* cat[3]; // used to read the file
                    cat[0] = "cat";
                    cat[1] = jobs[i].file_name;
                    cat[2] = NULL;
                    char* bincat[2] = {"/bin/cat", NULL};
                    char* term_commands[2] = {"/bin/cat", NULL};
                    sf_job_started(jobs[i].id, printers[jobs[i].printer_id].name, (int) getpgrp(), term_commands);

                    if(pipe(fd) == -1){
                        fprintf(stderr, "Cannot create pipe");
                        sf_job_aborted(i, jobs[i].status);
                        exit(1);
                    }

                    dup2(printer_fd, 1);
                    if((pid = fork()) == 0){ // Child Process
                        // writes into the pipe
                        dup2(fd[1], 1); // replace stdout with writing to pipe
                        close(fd[0]); // close read part of pipe
                        if(execvp(*cat, cat) == -1){
                            fprintf(stderr, "unable to perform command");
                        }
                        exit (0);
                    }
                    else{ // Parent Process
                        // reads from the pipe
                        dup2(fd[0], 0);  // replace stdin with reading from pipe
                        close(fd[1]); // close write part of pipe
                        if(execvp(*bincat, bincat) == -1)
                            fprintf(stderr, "unable to perform command");
                        close(printer_fd);
                        exit (0);
                    }
                    close(printer_fd);
                    free_names();
                    free_job_file();
                    conversions_fini();
                    if(input != stdin || input != NULL)
                        fclose(input);
                    exit (0);
                }
                // conversion pipeline
                else{
                    char* term_commands[num_conv + 1];
                    CONVERSION** temp = find_conversion_path(jobs[i].type->name, printers[jobs[i].printer_id].type->name);
                    term_commands[0] = (*temp)->cmd_and_args[0];

                    for(int m = 1; m < num_conv; m++){
                        free(temp);
                        temp = find_conversion_path(jobs[i].type->name, printers[jobs[i].printer_id].type->name);
                        term_commands[m] = (*temp)->cmd_and_args[0];
                    }
                    free(temp);
                    char* cat[3]; // used to read the file
                    cat[0] = "cat";
                    cat[1] = jobs[i].file_name;
                    cat[2] = NULL;
                    term_commands[num_conv] = NULL;
                    sf_job_started(jobs[i].id, printers[jobs[i].printer_id].name, (int) getpgrp(), term_commands);

                    int num_pipes = num_conv *2;

                    // fprintf(out, "Num pipes: %d\n", num_pipes);

                    int fd[num_pipes];// 0 is read, 1 is write
                    for(int f = 0; f < num_conv; f++){
                        if(pipe(fd + (2*f)) == -1){
                            fprintf(stderr, "Cannot create pipe");
                            sf_job_aborted(i, jobs[i].status);
                            close(printer_fd);
                            free_names();
                            free_job_file();
                            conversions_fini();
                            fclose(input);
                            exit(1);
                        }
                    }

                    pid_t pid2;
                    
                    dup2(printer_fd, 1);

                    if((pid2 = fork()) == 0){ // Child Process
                        // actually stuff goes here
                        pid_t child_pid[num_conv];
                        int chp = 0;

                        char* begarg = jobs[i].type->name;
                        for(int c = 0; c < num_conv; c++){
                            int read_from = c*2;
                            int write_to = read_from + 3;
                            temp = find_conversion_path(begarg, printers[jobs[i].printer_id].type->name);
                            begarg = (*temp)->to->name;
                            // fprintf(out, "doing conversion %s -> %s\n", (*temp)->from->name, (*temp)->to->name);
                            char** comm = (*temp)->cmd_and_args;
                            if(c == num_conv - 1){
                                if((child_pid[chp++] = fork()) == 0){
                                    dup2(fd[num_pipes - 2], 0);
                                    for(int pnum = 0; pnum < num_pipes; pnum++){
                                        if(pnum == (num_pipes-2))
                                            continue;
                                        close(fd[pnum]);
                                    }
                                    // execvp
                                    if(execvp(*comm, comm) == -1){
                                        exit(1);
                                    }
                                    exit(0);
                                }

                                // printf("pid %d\n", child_pid[i]);

                            }
                            else{
                                if((child_pid[chp++] = fork()) == 0){
                                    // fprintf(out, "piping READ %d -> WRITE %d\n", read_from, write_to);

                                    dup2(fd[read_from], 0);
                                    dup2(fd[write_to], 1);

                                    for(int pnum = 0; pnum < num_pipes; pnum++){
                                        if(pnum == read_from || pnum == write_to)
                                            continue;
                                        close(fd[pnum]);
                                    }
                                    // execvp
                                    if(execvp(*comm, comm) == -1){
                                        exit(1);
                                    }
                                    exit(0);
                                }
                                // printf("pid %d\n", child_pid[i]);
                            }
                            free(temp);
                        }
                        for(int pnum = 0; pnum < num_pipes; pnum++){
                            close(fd[pnum]);
                        }
                        for(int p = 0; p < num_conv; p++){
                            // fprintf(stdout, "pid[%d]: %d\n", p, child_pid[p]);
                            waitpid(child_pid[p], &child_status, 0);
                            if(WIFEXITED(child_status)){
                                continue;
                            }
                            else{
                                close(printer_fd);
                                free_names();
                                free_job_file();
                                conversions_fini();
                                fclose(stdout);
                                fclose(stdin);
                                fclose(stderr);
                                if(input != stdin || input != NULL)
                                    fclose(input);
                                exit (1);
                            }
                        }
                        close(printer_fd);
                        free_names();
                        free_job_file();
                        conversions_fini();
                        fclose(stdout);
                        fclose(stdin);
                        fclose(stderr);
                        if(input != stdin || input != NULL)
                            fclose(input);
                        exit (0);
                    }
                    else{ // Parent Process
                        //start the writing to the pipe
                        dup2(fd[1], 1);
                        for(int pnum = 0; pnum < num_pipes; pnum++){
                            if(pnum == 1)
                                continue;
                            close(fd[pnum]);
                        }
                        if(execvp(*cat, cat) == -1){
                            free_names();
                            free_job_file();
                            conversions_fini();
                            sf_job_aborted(i, jobs[i].status);
                            if(input != stdin || input != NULL)
                                fclose(input);
                            fprintf(stderr, "unable to perform command");
                            exit(1);
                        }
                    }
                    waitpid(pid2, &child_status, 0);
                    if(WIFEXITED(child_status)){
                        continue;
                    }
                    else{
                        close(printer_fd);
                        fclose(stdout);
                        fclose(stdin);
                        fclose(stderr);
                        free_names();
                        free_job_file();
                        conversions_fini();
                        if(input != stdin || input != NULL)
                            fclose(input);
                        exit (1);
                    }
                    close(printer_fd);
                    fclose(stdout);
                    fclose(stdin);
                    fclose(stderr);
                    free_names();
                    free_job_file();
                    conversions_fini();
                    if(input != stdin || input != NULL)
                        fclose(input);
                    exit (0);
                }
            }
            
            // waitpid(jobs[i].pgid, &child_status, 0);
            // if(WIFEXITED(child_status)){

            // }
            // else{
            //     jobs[i].status = JOB_ABORTED;
            //     sf_job_aborted(i, jobs[i].status);
            // }
        }
    }
}
