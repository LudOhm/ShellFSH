#include "redirections.h"

/* FLAGS nécessaires pour fic SI NOCLOBBER 
    CMD > fic  O_WRONLY O_CREAT O_EXCL  permissionsfichier // sans écrasement
    CMD >> fic O_WRONLY O_CREAT  O_APPEND permissionsfichier
    CMD >| fic  O_WRONLY O_CREAT O_TRUNC permissionsfichier
    CMD  < fic   O_RDONLY
    + redirections vers sortie erreur standard
*/

bool isRedir(char **input){
    // 0 == pas de  ;  1 = oui
    for (char **ptr = input; *ptr != NULL; ptr++) {
        for (int j = 0; (*ptr)[j] != '\0'; j++) {
            if ((*ptr)[j] == '<' || (*ptr)[j] == '>') {
                return true;
            }
        }
    }
    return false;
}

int handle_redirection( char * input){
    int res = 0;
    int old_fd = -1, new_fd = -1; // new_fd entree/sortie/erreur standard old_fd les fichiers
    char *args[100];

    // pour redirections sortie standard 
    char * FIC_out = NULL;
    int flag_out = -1;
    // pour redirection entrée standard
    char * FIC_in = NULL ;
    int flag_in = -1;
    // pour redirections  sortie erreur standard
    char * erreur_output = NULL;
    int erreur_flags = -1;
    
    int stdout = -1, stdin = -1, stderr = -1;
    if(parse_redirection(input, args, &FIC_out, &flag_out, &FIC_in, &flag_in, &erreur_output, &erreur_flags) != 0){
        res = 2; // car erreur syntaxe
        goto error;
    } 

    

    // redirections > >> >|
    if(FIC_out != NULL){
        old_fd = open(FIC_out, flag_out, 0666) ;
        stdout = dup(1);
        if (stdout < 0) {
            perror("dup");
            res = 1;
            goto error;
        }
        if(old_fd < 0) {
            perror("open");
            res = 1; 
            goto error;
        }
        new_fd = 1;
        if(dup2(old_fd, new_fd)== -1){
            res = 1; 
            goto error;
        }
        close(old_fd); 
        old_fd = -1;
    }
    // redirection <
    if (FIC_in != NULL){
        stdin = dup(0);
         if (stdin < 0) {
            perror("dup");
            res = 1;
            goto error;
        }
        old_fd = open(FIC_in, flag_in) ;
        if(old_fd < 0) {
            perror("open");
            res = 1; 
            goto error;
        }
        new_fd = 0;
        if(dup2(old_fd, new_fd)== -1){
            res = 1; 
            goto error;
        }
        close(old_fd); 
        old_fd = -1;
    }
    // redirections 2> 2>> 2>|
    if(erreur_output != NULL){
        stderr = dup(2);
        if (stderr < 0) {
            perror("dup");
            res = 1;
            goto error;
        }
        old_fd = open(erreur_output, erreur_flags, 0666) ;
        if(old_fd < 0) {
            perror("open");
            res = 1; 
            goto error;
        }
        new_fd = 2;
        if(dup2(old_fd, new_fd)== -1){
            res = 1; 
            goto error;
        }
        close(old_fd);  
        old_fd = -1;
    }
    // solution provisoire => exit < titi pas considere comme arguments en trop à deplacer dans execute +tard
    if (strcmp(input, "exit") == 0){
            exit(0);
        
    }
    else{
        res =  execute_command(input, args, res);
    }
    if (stdout >= 0) {
        dup2(stdout, 1);
        close(stdout);
    }
    if (stdin >= 0) {
        dup2(stdin, 0);
        close(stdin);
    }
    if (stderr >= 0) {
        dup2(stderr, 2);
        close(stderr);
    }
    return res;

    error : 
        if(old_fd >=0) close(old_fd);
        old_fd = -1;
        if (stdout >= 0) close(stdout);
        if (stdin >= 0) close(stdin);
        if (stderr >= 0) close(stderr);
        return res;
}

int parse_redirection(char *input, char **args, char ** FIC_out, int * flag_out, char ** FIC_in, int * flag_in, char ** err_out, int * err_flag){
    int index =0;
    int erreur_syntaxe = 2; int succes = 0;
    char *token = strtok(input, " \t\n");
    while (token != NULL){
        if(strcmp(token, "<") == 0){
            *flag_in = O_RDONLY;
            token = strtok(NULL, " \t\n");
            if (!token) {
                perror("Argument(s) manquant(s) : utilisation CMD < FIC" );
                goto error;
            }
            *FIC_in = token;

        }else if(strcmp(token, ">") == 0 ){
            *flag_out = O_WRONLY | O_CREAT | O_EXCL ;
            token = strtok(NULL, " \t\n");
            if (!token) {
                perror("Argument(s) manquant(s) : utilisation CMD > FIC" );
                goto error;
            }
            *FIC_out = token;
        }
        else if(strcmp(token, ">>") == 0){
            *flag_out = O_WRONLY | O_CREAT | O_APPEND ;
            token = strtok(NULL, " \t\n");
            if (!token) {
                perror("Argument(s) manquant(s) : utilisation CMD >> FIC" );
                goto error;
            }
            *FIC_out = token;
         }
        else if(strcmp(token, ">|") == 0 ){
            *flag_out = O_WRONLY | O_CREAT | O_TRUNC ;
            token = strtok(NULL, " \t\n");
            if (!token) {
                perror("Argument(s) manquant(s) : utilisation CMD >| FIC" );
                goto error;
            }
            *FIC_out = token;
        }
        else if(strcmp(token, "2>") == 0){
            *err_flag = O_WRONLY | O_CREAT | O_EXCL ;
            token = strtok(NULL, " \t\n");
            if (!token) {
                perror("Argument(s) manquant(s) : utilisation CMD 2> FIC" );
                goto error;
            }
            *err_out = token;
        }
        else if(strcmp(token, "2>>") == 0){
            *err_flag = O_WRONLY | O_CREAT | O_APPEND  ;
            token = strtok(NULL, " \t\n");
            if (!token) {
                perror("Argument(s) manquant(s) : utilisation CMD 2>> FIC" );
                goto error;
            }
            *err_out = token;
         }
        else if(strcmp(token, "2>|") == 0){
            *err_flag = O_WRONLY | O_CREAT | O_TRUNC ;
            token = strtok(NULL, " \t\n");
            if (!token) {
                perror("Argument(s) manquant(s) : utilisation CMD 2>| FIC" );
                goto error;
            }
            *err_out = token;
        }
        else{
            args[index] = token;  
            index++; 
        }
        token = strtok(NULL, " \t\n");
    }

    args[index] = NULL;

    return succes;
    error : 
        return erreur_syntaxe;
}