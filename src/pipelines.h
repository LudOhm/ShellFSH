#ifndef pipelines_h
#define pipelines_h

#include "execute.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include "commandeStructuree.h"

bool isPipeline(char **input);
int pipeline(char ** input);
int separer_pipeline(char* commande, char liste_cmd[100][1024]);
bool redirectionPipelineValide(char ** args, bool premiere, bool derniere);
int pipelineRedirection(char *FIC_out, int flag_out, char * FIC_in, int flag_in, char *err_out, int err_flag);
#endif 