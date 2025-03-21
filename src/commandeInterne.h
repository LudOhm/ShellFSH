#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

// Affiche le répertoire 
int pwd(void);

// Change le répertoire
int cd(const char *path);

// Quitte le shell 
void fsh_exit(int exit_code);

// donne le type
int fsh_ftype(const char *path);

// pour afficher le type
void type_sortie(const char *message);

#endif // COMMANDS_H
