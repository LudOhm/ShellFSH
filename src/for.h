#ifndef FOR_H
#define FOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pthread.h>
#include <ctype.h>
#include "execute.h"
#include "commandeStructuree.h"

void err_write(const char *message);
int comm_for( char *command);

#endif
