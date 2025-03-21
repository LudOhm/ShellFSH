#ifndef commandeStructuree_h
#define commandeStructuree_h

#include "execute.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>


int commandeStruc(char **input);
int separer_cmd(char* commande, char *separateur, char liste_cmd[100][1024]);
int comm_if( char* commande);
int parseIf(char * commande , char** test , char** ifTrue  , char ** _else);
int isStruct(char **input);
void recupere_String(char **input, char *result);


#endif 
