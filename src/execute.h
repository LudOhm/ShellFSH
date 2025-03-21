#ifndef EXECUTE_H
#define EXECUTE_H

#include "commandeInterne.h"
#include "for.h"
#include "commandeStructuree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>
#include "commandeStructuree.h"
#include "redirections.h"
#include <stdbool.h>
#include "pipelines.h"
int execute_command(char *command, char **args, int exit_status);
int find_path(const char*command, char *path_env, char* full_path);
void parse_command(char *input, char **args);
int commande_externe(char *command, char **args);
bool parsePriorite(char **args);
#endif // EXECUTE_H
