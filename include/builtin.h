#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser.h"

int is_builtin(const char *command);

int builtin_execute(command_t *command);

#endif
