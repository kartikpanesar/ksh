#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>


#define BUFF_MAX 1024


typedef struct command {
        char **args;
        char *input_file;
        char *output_file;
        int output_append;
} cmd;

cmd* cmd_init();

int is_builtin(cmd* command);
int builtin_run(cmd* command);
int parsed_input(char **tokens, cmd* command);


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
                                        void * tmp = 0;
                                        tokens_capacity *= 2;
                                        tmp = realloc(args, sizeof(char*) *tokens_capacity);
                                        if(tmp==0){
                                                fprintf(stderr, "Couldn't allocate memory for arguments of command.\n");
                                                perror("Error: ");
                                                exit(EXIT_FAILURE);
                                        }
                                        args = tmp;
                                }
                                void *tmp = strdup(buffer);
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
                        void * tmp = 0;
                        tokens_capacity *= 2;
                        tmp = realloc(args, sizeof(char*) *tokens_capacity);
                        if(tmp==0){
                                fprintf(stderr, "Couldn't allocate memory for arguments of command.\n");
                                perror("Error: ");
                                exit(EXIT_FAILURE);
                        }
                        args = tmp;
                }
                void *tmp = strdup(buffer);
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



int shell_execute(cmd* command){
        
        pid_t p = fork();

        if(p==-1){
                fprintf(stderr, "Couldn't create child process.\n");
                perror("Error: ");
                return 0;
        }

        else if(p==0){

                if(command->input_file!=0){
                        int fd1 = open(command->input_file, O_RDONLY );
                        if(fd1==-1){
                                fprintf(stderr, "Couldn't open %s\n", command->input_file);
                                perror("Error: ");
                                exit(1);
                        }

                        if(dup2(fd1, STDIN_FILENO)==-1){
                                fprintf(stderr, "error pointing STDIN_FILENO to fd.\n");
                                perror("Error: ");
                                exit(1);
                        }
                        close(fd1);
                }

                if(command->output_file!=0){
                        int fd2 = 0;
                        if(command->output_append){
                                fd2 = open(command->output_file, O_WRONLY | O_CREAT | O_APPEND, 06444);
                        }

                        else{
                                fd2 = open(command->output_file , O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        }

                        if(fd2==-1){
                                fprintf(stderr, "Couldn't open or create %s\n", command->output_file);
                                perror("Error: ");
                                exit(1);
                        }

                        if(dup2(fd2, STDOUT_FILENO)==-1){
                                fprintf(stderr, "Error pointing STDOUT_FILENO to fd.\n");
                                perror("Error: ");
                                exit(1);
                        }
                        close(fd2);
                }

                execvp(command->args[0], command->args);
                fprintf(stderr, "execvp_error: ;Couldn't execute command.\n");
                perror("Error: ");
                exit(1);
        }

        else{
                wait(NULL);
        }
        return 0;
}


int shell_run(cmd* command){
        if(command->args[0] == NULL){
                fprintf(stderr, "No command found.\n");
                return 0;
        }

        // result is 0 , if command ran successfully.
        // 1 , if it failed.
        // -1 , if the command was exit.

        int result = 0;

        if(is_builtin(command)){
                result = builtin_run(command);
        }

        else{
                 result = shell_execute(command);
        }

        return result;
}

void free_tokens(char **tokens){

        // freeing the memory for arguments.
        for(int i=0; tokens[i]!=NULL; i++){
                free(tokens[i]);
        }
        free(tokens);

        return ;
}

void free_command(cmd *command){
        for(int i=0; command->args[i]!=NULL; i++){
                free(command->args[i]);
        }
        free(command);
}


void shell_loop(void){
        int status = 0;
        char path_name[1024];

        char ** tokens = 0;
        cmd *command = 0;

        while(1){
                if(status==-1){
                        break;
                }


                if(getcwd(path_name, 1024)==NULL){
                        fprintf(stderr, "Change Directory error.\n");
                        perror("Error: ");
                        continue;
                }

                printf("ksh > ");
                printf("%s > ", path_name);


                tokens = shell_line();
                command = cmd_init();

                if(parsed_input(tokens, command)==-1){
                        printf("couldn't create command.\n");
                        continue;
                }

                free(tokens);

                status = shell_run(command);

                free_command(command);
        }


        return ;
}


int main(){

        shell_loop();
        return 0;
}



