#include <stdlib.h>
#include <command.h>

// typedef struct command {
//         char **args;
//         char *input_file;
//         char *output_file;
//         int output_append;
// } cmd;

cmd **cmds;

cmd * cmd_init(){
        cmd *command1 = calloc(1, sizeof(cmd));
        return command1;
}

void cmd_destroy(cmd *command){
        for(int i=0; command->args[i]!=NULL; i++){
                free(command->args[i]);
        }
        free(command);

        return ;
}

