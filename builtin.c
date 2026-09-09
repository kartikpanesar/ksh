#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct command {
        char **args;
        char *input_file;
        char *output_file;
        int output_append;
} cmd;



static char *builtin_commands[] = {"exit", "cd", NULL};

int is_builtin(cmd *command){
        for(int i=0; builtin_commands[i]!=NULL; i++){
                if(strcmp(command->args[0] , builtin_commands[i]) == 0){
                        return 1;
                }
        }
        return 0;
}

int shell_exit(cmd* command){
        if(command->args[1] != NULL){
                fprintf(stderr, "No arguments required.\n");
                return 1;
        }
        return -1;
}


int shell_cd(cmd* command){
        if(command->args[1] == NULL){
                fprintf(stderr, "No arguments found.\n");
                fprintf(stderr, "usage: cd file_path\n");
                exit(1);
        }

        else if(chdir(command->args[1])==-1){
                fprintf(stderr, "Couldn't change directory.\n");
                perror("Error: ");
        }
        return 1;
}



typedef int (*shell_func) (cmd* command);


int builtin_run(cmd* command){

        int exit_flag = 0;

        shell_func cmd_funcs[] = {&shell_exit, &shell_cd};

        for(int i=0; builtin_commands[i]!=NULL; i++){
                if(strcmp(builtin_commands[i], command->args[0])==0){
                        exit_flag = cmd_funcs[i](command);
                }
        }

        return exit_flag;
}

