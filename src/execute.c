#include "execute.h"


#define MAX_ARGS 100

// Fonction pour séparer la ligne de commande selon les arguments
void parse_command(char *input, char **args) {
    int i = 0;
    args[i] = strtok(input, " \t\n");
    while (args[i] != NULL && i < MAX_ARGS - 1) {
        args[++i] = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

// Pour afficher les messages d'erreur
void err_write_exec(const char *message) {
    if( write(STDERR_FILENO, message, strlen(message)) < 0){
        perror("write");
        exit(1);
    }    
}

bool parsePriorite(char **args){
    // DEBUGG
            /*printf("DANS PARSEPRIORITE");
            for (int i = 0; args[i] != NULL; i++) {
                printf("%s ", args[i]); // Print each part followed by a space
            }
            printf("\n");*/
    // si presence d'un ';' en dehors de deux commandes
    int profondeur = 0;
    for (int i = 0; args[i] != NULL; i++){
        if (strcmp(args[i], "{") == 0) {
            profondeur++;
        } else if (strcmp(args[i], "}") == 0) {
            profondeur--;
        } else if (strcmp(args[i], ";") == 0 && profondeur == 0){
            // ALORS au moins une grande commande ;
            return false;
        }
    }
    if(profondeur != 0){
        printf("erreur syntaxe");
    }
    return true;
}

int execute_command(char *command, char **args, int exit_status) {
    // commandes structurées
    // tester si ; est à l'interieur d'une commande structuré
    if(isStruct(args) > 0 ){
         //printf("---STRUCT DETECTEE----\n");
        bool isNested = parsePriorite(args);
        if(isNested == false  ) {
             //printf("---POINT VIRGULE EST LIBRE----\n");
            return commandeStruc(args);
        }
    }
    if(isPipeline(args)){
        return pipeline(args);
    }
    if(strncmp(args[0], "if", 2) == 0 && strlen(args[0]) == 2){
        char inputString[1024];
        recupere_String(args, inputString);
        //printf("---STRING RECONSTRUIT DANS IF EXECUTE---- :\n %s\n", inputString);
        return comm_if(inputString);
    }
   
    if (strncmp(args[0], "for", 3) == 0 && strlen(args[0]) == 3) {
        char inputString[1024];
        recupere_String(args, inputString);
        return comm_for(inputString);  
    }
    else if(isRedir(args)){
        char inputString[1024];
        recupere_String(args, inputString);
        return handle_redirection(inputString);

    }
    // COMMANDES INTERNES
    if (strcmp(command, "pwd") == 0) {
        if (args[1] != NULL) { // Vérifie s'il y a un argument supplémentaire
            err_write_exec("pwd: ");
            err_write_exec(args[1]);
            err_write_exec(": invalid argument\n");
            //fprintf(stderr, "pwd: %s: invalid argument\n", args[1]);
            return 1;
        }
        return pwd();
    } else if (strcmp(command, "cd") == 0) {
        // args[1] est le chemin pour cd
        if (args[2] != NULL) { // Vérifie s'il y a plus d'un argument
            err_write_exec("cd: too many arguments\n");
            //fprintf(stderr, "cd: too many arguments\n");
            return 1;
        }
        return cd(args[1]);
    } else if (strcmp(command, "exit") == 0) {
        if (args[2] == NULL ) { // Vérifie s'il y a plus d'un argument et que c'est pas une redirection
            
            int exit_code = args[1] ? atoi(args[1]) : exit_status; 
            fsh_exit(exit_code);
        }
        else{
            err_write_exec("exit: too many arguements\n");
            //fprintf(stderr, "exit: too many arguments\n");
            return 1;
        }
        
    } else if (strcmp(command, "ftype") == 0) {
        return fsh_ftype(args[1]);
    } else {
        // commande externe
        return  commande_externe(command, args);
    }
    return 1;
}

int find_path(const char*command, char *path_env, char* full_path){
    // path_env est de la forme ".../usr/bin:/usr/bin:/bin:/usr/sbin:/sbin" donc
    char* parsed = strtok(path_env, ":");
    // le résultat
    while(parsed != NULL){
        snprintf(full_path, 1024, "%s/%s", parsed, command);
        // on vérifie si  c'est un chemin valide avec mode execution OK
        if(access(full_path, X_OK)==0){
             return 0;
        }else{
            // on continue le parcours
            parsed = strtok(NULL, ":");
        }
       
    }
    return 1;
}

int commande_externe(char *command, char **args){
    // commandes externes
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            extern char **environ;
            // Processus enfant
            // n'ignore pas les signaux
             struct sigaction action = {0};
            action.sa_handler = SIG_DFL;
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGTERM, &action, NULL);
            // gestion du cas ou la commande externe est déja un full path
            if(strchr(command, '/') != NULL){
                if (access(command, X_OK) == 0){
                    if(execve(command, args, environ) == -1){
                        perror("execve");
                        exit(1);
                    }
                } else{
                    exit(1);
                }
            }else{
                char *path_env = getenv("PATH");
                if(path_env == NULL) {
                    perror("getenv");
                    exit(1);
                }
                char full_path[PATH_MAX];
        
                if(find_path(command, path_env, full_path) == 1){
                    perror("find_path");
                    exit(1);
                }
             
                if(execve(full_path, args, environ) == -1){
                    perror("execve");
                    exit(1);
                }
            }
            exit(0);
                    
        } else {
            // Processus parent
            int statut_enfant;
            waitpid(pid, &statut_enfant, 0);
            if ( WIFEXITED(statut_enfant) ) { 
                return WEXITSTATUS(statut_enfant);
            }
            if (WIFSIGNALED(statut_enfant)){
                return 255;
            }
            return 0;
        }
}
