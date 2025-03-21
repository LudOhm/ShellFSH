#ifndef redirections_h
#define redirections_h

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include "execute.h"


int handle_redirection( char * input);
bool isRedir(char **input);
int parse_redirection(char * input, char **args, char ** FIC_out, int * flag_out, char ** FIC_in, int *flag_in, char ** err_out, int * err_flag);
#endif 