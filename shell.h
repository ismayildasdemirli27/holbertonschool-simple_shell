#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

extern char **environ;

char *_getenv(const char *name);
char *get_location(char *command);
void print_error(char *argv0, char *cmd);
int execute_cmd(char **args, char *argv0, char *line);

#endif
