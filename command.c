#include <stdlib.h>

typedef struct command {
        char **args;
        char *input_file;
        char *output_file;
        int output_append;
} cmd;

cmd * cmd_init(){
        cmd *command1 = calloc(1, sizeof(cmd));
        return command1;
}

