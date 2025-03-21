#include "commandeInterne.h"


int pwd(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        ssize_t result = write(STDOUT_FILENO, cwd, strlen(cwd));
        if (result == -1) { // pas < strlen(cwd)??
            perror("write");
            return 1;
         } else {
            if (write(STDOUT_FILENO, "\n", 1)  == -1) { // < 1
                perror("write");
                return 1;
            } else{
                return  0;
            }
        }
    } else {
        perror("pwd");
        return 1;
    }
}

int cd(const char *path) {
    if (path == NULL) {
        path = getenv("HOME");  // Si aucun chemin 
    } else if (strcmp(path, "-") == 0) {
        path = getenv("OLDPWD");  // aller au dernier répertoire
    }
    if (path == NULL || chdir(path) != 0) {
        perror("cd");
        return 1;
    }else {
        // Mettre à jour OLDPWD et PWD
        setenv("OLDPWD", getenv("PWD"), 1);
        setenv("PWD", path, 1);
        return 0;
    }
}


int fsh_ftype(const char *path) {
    struct stat path_stat;
     if (path == NULL) {
        const char *message = "argument manquant\n";
        if(write(STDOUT_FILENO, message, strlen(message)) < 0) {
            perror("write");
            return 1;
        }
        return 1;
    }
    if (lstat(path, &path_stat) != 0) {
        perror("ftype");
        return 1;
    }
    if (S_ISLNK(path_stat.st_mode)) {
        type_sortie("symbolic link\n");
    } else if (S_ISREG(path_stat.st_mode)) {
        type_sortie("regular file\n");
    } else if (S_ISDIR(path_stat.st_mode)) {
        type_sortie("directory\n");
    } else if (S_ISFIFO(path_stat.st_mode)) {
        type_sortie("named pipe\n");
    } else {
        type_sortie("other\n");
    }
    
    return 0;
}

void type_sortie(const char *message) {
    if(write(STDOUT_FILENO, message, strlen(message)) < 0) {
        perror("Erreur lors de l'écriture");
        exit(EXIT_FAILURE);
    }
}

void fsh_exit(int exit_code){
    exit(exit_code);
}