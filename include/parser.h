#include "command.h"

#ifndef PARSER_H
#define PARSER_H

char **shell_line();
int parse_input_pipe(char **tokens);

#endif
