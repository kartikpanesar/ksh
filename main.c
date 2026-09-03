#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFF_MAX 1024


char **shell_line(){

        char buffer[BUFF_MAX]; // maximum length of a single word .
        size_t n = 0;
        size_t n_args = 0;
        int c = 0;
        size_t args_capacity = 32;

        char** args = malloc(sizeof(char *) * args_capacity);

        if(args == 0){
                fprintf(stderr, "Couldn't allocate memory for args.\n");
                exit(EXIT_FAILURE);
        }


        while((c=getchar())!='\n' && c != EOF){
                if(n >= BUFF_MAX){
                        fprintf(stderr, "Too large of a word in your command.\n");
                        exit(EXIT_FAILURE);
                }

                if(c==' ' || c == '\t'){
                        if (n>0){
                                buffer[n] = '\0';
                                // capacity - 1 below , makes sure that there is always space for NULL.
                                // at the end of the args.
                                if(n>=args_capacity-1){     
                                        void * tmp = 0;
                                        args_capacity *= 2;
                                        tmp = realloc(args, sizeof(char*) *args_capacity);
                                        if(tmp==0){
                                                fprintf(stderr, "Couldn't allocate memory for arguments of command.\n");
                                                exit(EXIT_FAILURE);
                                        }
                                        args = tmp;
                                }
                                args[n_args++] = strdup(buffer);
                                n = 0;
                        }
                        continue;
                }
                buffer[n++] = c;
        }

        if (n>0){
                buffer[n] = '\0';
                // capacity - 1 below , makes sure that there is always space for NULL.
                // at the end of the args.
                if(n>=args_capacity-1){
                        void * tmp = 0;
                        args_capacity *= 2;
                        tmp = realloc(args, sizeof(char*) *args_capacity);
                        if(tmp==0){
                                fprintf(stderr, "Couldn't allocate memory for arguments of command.\n");
                                exit(EXIT_FAILURE);
                        }
                        args = tmp;
                }
                args[n_args++] = strdup(buffer);
        }
        args[n] = NULL;

        return args;
}

void shell_loop(void){

        while(1){
                printf("sHELL >");

                char **args = shell_line();
                pid_t pid = fork();

                if(pid<0){
                        fprintf(stderr, "Couldn't fork for some reason.\n");
                        exit(EXIT_FAILURE);
                }

                else if(pid==0){

                        int r = execvp(args[0], args);
                        if(r >=0 || r < 0){
                                fprintf(stderr, "Couldn't execute this command.\n"
                                                "Some error Occured\n");
                        }
                }
                else{
                        wait(NULL);
                }
        }
}



int main(){

        shell_loop();
        return 0;
}
