#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "builtin.h"
#include "command.h"
#include "parser.h"




void shell_execute(cmd* command){
        
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
                 shell_execute(command);
        }

        return result;
}


int shell_run_cmds(cmd **cmds, int n)
{
        // int status = 0;
        pid_t pids[n];

        // creating n-1 pipes.
        int pipes[n-1][2];
        for(int k=0; k<n-1; k++){
                if(pipe(pipes[k])==-1){
                        fprintf(stderr, "Couldn't create a pipe.\n");
                        perror("Error: ");
                }
        }

        // if it is a single builtin command, run it before forking.
        if(is_single_builtin(cmds, n)){
                return shell_run(cmds[0]);
        }


        for(int k=0; k<n; k++){
                pids[k] = fork();

                if(pids[k]<0){
                        fprintf(stderr, "Error forking.\n");
                        perror("Error: ");
                        exit(1);
                }

                else if(pids[k]==0){

                        // if there is only single command, NO PIPES.
                        if(n==1){
                                shell_run(cmds[0]);
                                return 0;
                        }

                        if(k==0){
                                if(dup2(pipes[k][1], STDOUT_FILENO)==-1){
                                        perror("Error in dup2: ");
                                        exit(1);
                                }

                                // close all the pipe ends of all the pipes.
                                for(int i=0; i<n-1; i++){
                                        close(pipes[i][0]);
                                        close(pipes[i][1]);
                                }
                                shell_run(cmds[k]);
                        }
                        else if(k==n-1){
                                if(dup2(pipes[k-1][0], STDIN_FILENO)==-1){
                                        perror("Error in dup2: ");
                                        exit(1);
                                }

                                // close all the pipe ends of all the pipes.
                                for(int i=0; i<n-1; i++){
                                        close(pipes[i][0]);
                                        close(pipes[i][1]);
                                }

                                shell_run(cmds[k]);
                        }
                        else{
                                if(dup2(pipes[k-1][0], STDIN_FILENO)==-1){
                                        perror("Error in dup2: ");
                                        exit(1);
                                }

                                if(dup2(pipes[k][1], STDOUT_FILENO)==-1){
                                        perror("Error in dup2: ");
                                        exit(1);
                                }
                                
                                // close all the pipe ends of all the pipes.
                                for(int i=0; i<n-1; i++){
                                        close(pipes[i][0]);
                                        close(pipes[i][1]);
                                }

                                shell_run(cmds[k]);
                        }
                }

        }

        // Parent Process.

        // if pipes exist.
        if(n>1){
                // close all the pipe ends of all the pipes.
                for(int j=0; j<n-1; j++){
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                }
        }


        // Wait for all the children.
        for(int i=0; i<n; i++){
                waitpid(pids[i], NULL, 0);
        }


        return 0;
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

        int n_cmds = 0;

        int pipedes[2] = {0};
        pipe(pipedes);

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
                n_cmds = parse_input_pipe(tokens) ;

                if(n_cmds == -1){
                        fprintf(stderr, "Error parsing the input.\n");
                        free_tokens(tokens);
                        continue;
                }

                n_cmds += 1;

                free_tokens(tokens);

                status = shell_run_cmds(cmds, n_cmds);


                for(int i=0; cmds[i]!=NULL; i++){
                        free_command(cmds[i]);
                }
                free(cmds);
        }

        return ;
}


int main(){

        shell_loop();

        return 0;
}



