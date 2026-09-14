
#ifndef COMMAND_H
#define COMMAND_H

typedef struct command{
        char **args;
        char *input_file;
        char *output_file;
        int output_append;
} cmd;

extern cmd **cmds;

cmd* cmd_init(void);

void cmd_destroy(cmd *command);

#endif
