#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int shell_exit(char **args){
        return -1;
}


int shell_cd(char **args){
        if(args[1] == NULL){
                fprintf(stderr, "No arguments found.\n");
                fprintf(stderr, "usage: cd file_path\n");
                exit(1);
        }

        else if(chdir(args[1])==-1){
                fprintf(stderr, "Couldn't change directory.\n");
                perror("Error: ");
        }
        return 1;
}



typedef int (*shell_func) (char **args);


int builtin_run(char **args){

        int exit_flag = 0;
        char *builtin_commands[] = {"exit", "cd"};
        int cmds = 2;
        shell_func cmd_funcs[] = {&shell_exit, &shell_cd};

        for(int i=0; i<cmds; i++){
                if(strcmp(builtin_commands[i], args[0])==0){
                        exit_flag = cmd_funcs[i](args);
                }
        }
        return exit_flag;
}

