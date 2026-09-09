#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct command {
        char **args;
        char *input_file;
        char *output_file;
        int output_append;
} cmd;

cmd* cmd_init();


// re-direction operators
static char *r_operators[3] = {"<", ">", ">>"};

int is_r_operator(char *string){
        for(int i=0; i<3; i++){
                if(strcmp(string, r_operators[i])==0){
                        return 1;
                }
        }
        return 0;
}


int parsed_input(char **tokens, cmd *command)
{
        char ** args;

        int r_index = -1;  // index of the first r operator.
        int argument_end = 0;
        int i = 0;

        for(; tokens[i]!=NULL; i++){
                if(is_r_operator(tokens[i])){

                        //if not encountered any r operator yet.
                        if(r_index == -1){
                                r_index = i;
                        }

                        if(tokens[i+1] == NULL){
                                return -1;
                        }
                        else if(strcmp(tokens[i], ">>")==0){
                                if((command->output_file = strdup(tokens[i+1]))== 0){
                                        fprintf(stderr, "Couldn't allocate memory for output_filename\n");
                                        perror("Error: ");
                                        return -1;
                                }
                                command->output_append = 1;
                        }
                        else if(strcmp(tokens[i], ">")==0){
                                if((command->output_file = strdup(tokens[i+1]))== 0){
                                        fprintf(stderr, "Couldn't allocate memory for output_filename\n");
                                        perror("Error: ");
                                        return -1;
                                }
                        }

                        else{
                                if((command->input_file= strdup(tokens[i+1]))== 0){
                                        fprintf(stderr, "Couldn't allocate memory for input_filename\n");
                                        perror("Error: ");
                                        return -1;
                                }
                        }
                }
        }

        argument_end = i;

        // if any r operator is found .
        if(r_index != -1){
                argument_end = r_index;
        }

        args = malloc(sizeof(char *) * (argument_end + 1));
        // command_end + 2 , because we need space for NULL at the end.
        if(args==NULL){
                fprintf(stderr, "Couldn't allocate memory for command arguments list.\n");
                perror("Error: ");
                return -1;
        }

        int j = 0;
        for(; j<argument_end; j++){
                args[j] = strdup(tokens[j]);
                if(args[j]==NULL){
                        fprintf(stderr, "Couldn't allocate memory for a command argument.\n");
                        perror("Error: ");
                        return -1;
                }
        }
        args[j] = NULL;

        command->args = args;

        return 0;
}




