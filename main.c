#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFF_MAX 1024

// int shell_exit();
// int shell_cd();
int builtin_run(char **args);

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



int shell_execute(char **args){
        
        pid_t p = fork();

        if(p==-1){
                fprintf(stderr, "Couldn't create child process.\n");
                perror("Error: ");
                return 0;
        }

        else if(p==0){

                // doesn't return anything on success.
                if(execvp(args[0] , args)==-1){
                        fprintf(stderr, "execvp syscall failed.\n");
                        perror("Error: ");
                        return 0;
                }
        }

        else{
                wait(NULL);
        }
        return 0;
}


int shell_execute(char **args){
        if(args[0] == NULL){
                fprintf(stderr, "No command found.\n");
                return 0;
        }

        // it returns 1 if the command was builtin , in that case it also run that command.
        // it returns 2 , if the command was exit.
        // it returns 0 otherwise.

        int r = builtin_run(args);

        if(r>0){
                return r;
        }

        else{
                 r = shell_run(args);
        }

        return r;
}




void shell_loop(void){
        int status = 0;
        char ** args = 0;
        char buffer[1024];

        while(1){
                if(status==2){
                        break;
                }

                if(getcwd(buffer, 1024)==NULL){
                        fprintf(stderr, "Change Directory error.\n");
                        perror("Error: ");
                        continue;
                }
                       

                printf("sHELL ");
                printf("%s > ", buffer);

                args = shell_line();
                status = shell_execute(args);
        }

        char ** tmp = args;
        while(*tmp++ != NULL){
                free(tmp);
        }
        free(args);

        return ;
}


int main(){

        shell_loop();
        return 0;
}



