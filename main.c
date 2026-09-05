#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

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
                perror("Error: ");
                exit(EXIT_FAILURE);
        }


        while((c=getchar())!='\n' && c != EOF){
                if(n >= BUFF_MAX){
                        fprintf(stderr, "Too large of a word in your command.\n");
                        perror("Error: ");
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
                                                perror("Error: ");
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
                                perror("Error: ");
                                exit(EXIT_FAILURE);
                        }
                        args = tmp;
                }
                args[n_args++] = strdup(buffer);
        }
        args[n] = NULL;

        return args;
}

int shell_exit(char **args){
        return 2;
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
                exit(1);
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

int shell_run(char **args){
        
        pid_t p = fork();

        if(p==-1){
                fprintf(stderr, "Couldn't create child process.\n");
                perror("Error: ");
                return 0;
        }

        else if(p==0){

                // doesn't return anything on success.
                if(execvp(args[0] , args)==-1){
                        fprintf(stderr, "execvp sys call failed.\n");
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



