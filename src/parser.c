#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"

#define BUFF_MAX 1024

char **shell_line(){

        char buffer[BUFF_MAX]; // maximum length of a single word .
        size_t buff_index = 0;

        size_t n_tokens = 0;
        int c = 0;
        size_t tokens_capacity = 32;

        void *tmp = 0;

        char** args = malloc(sizeof(char *) * tokens_capacity);

        if(args == 0){
                fprintf(stderr, "Couldn't allocate memory for args.\n");
                perror("Error: ");
                exit(EXIT_FAILURE);
        }


        while((c=getchar())!='\n' && c != EOF){
                if(buff_index >= BUFF_MAX){
                        fprintf(stderr, "Too large of a word in your command.\n");
                        perror("Error: ");
                        exit(EXIT_FAILURE);
                }

                if(c==' ' || c == '\t'){
                        if (buff_index>0){
                                buffer[buff_index] = '\0';
                                // capacity - 1 below , makes sure that there is always space for NULL.
                                // at the end of the args.
                                if(n_tokens>=tokens_capacity-1){
                                        tmp = 0;
                                        tokens_capacity *= 2;
                                        tmp = realloc(args, sizeof(char*) *tokens_capacity);
                                        if(tmp==0){
                                                fprintf(stderr, "Couldn't allocate memory for arguments of command.\n");
                                                perror("Error: ");
                                                exit(EXIT_FAILURE);
                                        }
                                        args = tmp;
                                }
                                tmp = strdup(buffer);
                                if(tmp==NULL){
                                        fprintf(stderr, "Couldn't allocate memory for a argument.\n");
                                        perror("Error: ");
                                        exit(1);
                                }
                                args[n_tokens++] = tmp;
                                buff_index = 0;
                        }
                        continue;
                }
                buffer[buff_index++] = c;
        }

        if (buff_index>0){
                buffer[buff_index] = '\0';
                // capacity - 1 below , makes sure that there is always space for NULL.
                // at the end of the args.
                if(n_tokens>=tokens_capacity-1){
                        tmp = 0;
                        tokens_capacity *= 2;
                        tmp = realloc(args, sizeof(char*) *tokens_capacity);
                        if(tmp==0){
                                fprintf(stderr, "Couldn't allocate memory for arguments of command.\n");
                                perror("Error: ");
                                exit(EXIT_FAILURE);
                        }
                        args = tmp;
                }
                tmp = strdup(buffer);
                if(tmp == NULL){
                        fprintf(stderr, "Couldn't allocate memory for a argument.\n");
                        perror("Error: ");
                        exit(1);
                }
                args[n_tokens++] = tmp;
        }
        args[n_tokens] = NULL;

        return args;
}


// re-direction operators
static char *r_operators[3] = {"<", ">", ">>"};

static int is_r_operator(char *string){
        for(int i=0; i<3; i++){
                if(strcmp(string, r_operators[i])==0){
                        return 1;
                }
        }
        return 0;
}

static int parse_input_redirection(char **tokens, cmd *command);

int parse_input_pipe(char **tokens){
        int n_pipes = 0;
        int i = 0;

        if(strcmp(tokens[0], "|")==0){
                fprintf(stderr, "pipe cannot be at the start of the line.\n");
                return -1;
        }

        for(; tokens[i]!=NULL; i++){
                if(strcmp(tokens[i], "|")==0){
                        n_pipes++;
                }
        }

        if(strcmp(tokens[i-1], "|")==0){
                fprintf(stderr, "Pipe cannot be at the end of the line.\n");
                return -1;
        }

        
        cmds = calloc(n_pipes+1, sizeof(cmd *));
        int cmds_index = 0;

        i = 0;
        int s = 0;
        int index = 0;
        void* tmp = 0;
        int result = 0;

        for(; tokens[i]!=NULL; i++){
                if(strcmp(tokens[i], "|")==0){

                        // +1 for NULL at the end.
                        char **single_cmd_tokens = malloc(sizeof(char *) *(i-s+1));
                        index = 0;

                        for(int k=s; k<i; k++){
                                tmp = strdup(tokens[k]);
                                if(tmp==0){
                                        fprintf(stderr, "Error while allocating memory.\n");
                                        perror("Error: ");
                                        exit(1);
                                }
                                single_cmd_tokens[index++] = tmp;
                        }
                        single_cmd_tokens[index] = NULL;

                        s = i+1;

                        cmd *command = cmd_init();

                        result = parse_input_redirection(single_cmd_tokens, command);
                        if(result != 0){
                                fprintf(stderr, "Error in parsing input for redirection.\nTry Again.\n");
                                return -1;
                        }
                        cmds[cmds_index++] = command;


                        // freeing memory for single command tokens.
                        for(int p=0; single_cmd_tokens[p]!=NULL; p++){
                                free (single_cmd_tokens[p]);
                        }
                        free(single_cmd_tokens);

                }
        }

        if(tokens[s]!=NULL){
                char **single_cmd_tokens = malloc(sizeof(char *) *(i-s+1));
                cmd *command = cmd_init();
                index = 0;
                
                for(int k=s; k<i; k++){
                        tmp = strdup(tokens[k]);
                        if(tmp == 0){
                                fprintf(stderr, "Couldn't allocate memory.\n");
                                perror("Error: ");
                                exit(1);
                        }
                        single_cmd_tokens[index++] = tmp;
                }
                single_cmd_tokens[index] = NULL;

                result = parse_input_redirection(single_cmd_tokens, command);
                if(result!=0){
                        fprintf(stderr, "Couldn't parse input for redirection.\nTry Again.\n");
                        return -1;
                }

                cmds[cmds_index++] = command;

                // freeing memory occupied by single_cmd_tokens
                for(int j=0; single_cmd_tokens[j]!=NULL; j++){
                        free(single_cmd_tokens[j]);
                }
                free(single_cmd_tokens);
        }


        cmds[cmds_index] = NULL;

        return n_pipes;
}


static int parse_input_redirection(char **tokens, cmd *command)
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



