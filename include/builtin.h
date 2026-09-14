#include "command.h"

#ifndef BUILTIN_H
#define BUILTIN_H

int is_builtin(cmd *command);
int builtin_run(cmd *command);
int is_single_builtin(cmd **cmds, int n);

#endif
