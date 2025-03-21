#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include "commandeInterne.h"
#include "commandeStructuree.h"
#include "execute.h"
#include "for.h"
#include "redirections.h"


#define MAX_ARGS 64
#define MAX_PROMPT_LENGTH 30



void formatagePrompt(int lastStatut, char* prompt, const char* cwd){
    // gerer couleur statut
    const char * couleur_statut = (lastStatut == 0) ? "\001\033[32m\002": "\001\033[91m\002";
    char statut[10];
    //if(lastStatut == 255) snprintf(statut, 10, "[SIG]"); // A CORRIGER PLUS TARD PB signal vs code entre à la main
    /*else*/ snprintf(statut, 10, "[%d]",lastStatut); 
        // Gestion partie path
    // basculer vers du cyan
    const char *couleur_path= "\001\033[36m\002";
    
    // reserver 2 char pour "$ "
    size_t max_path  = MAX_PROMPT_LENGTH - (strlen(statut) + 2);
    char path[max_path];
    size_t length_path = strlen(cwd);
    if(length_path <= max_path){
        snprintf(path, length_path+2, "%s",  cwd);
    }else{
        size_t indexDebut = (length_path- max_path)+3; 
        memmove((void *)cwd, cwd+indexDebut, max_path);
        snprintf(path, max_path+2, "...%s", cwd);
    }
    // on repasse à une couleur normale
    const char * couleur_normale = "\001\033[00m\002";
    snprintf(prompt, MAX_PROMPT_LENGTH+50, "%s%s%s%s%s$ ", couleur_statut, statut, couleur_path, path,couleur_normale);
}

int main() {
    char *command = NULL; // Ligne de commande saisie
    char *args[MAX_ARGS];
    int exit_status = 0;
    char cwd[1024]; 

    rl_outstream = stderr;
    // fsh ignore les signaux SIGTERM et SIGINT
    struct sigaction action = {0};
    action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            exit_status = 1;
            break;
        } // donc après ça cwd est non null 
        char prompt[64];
        formatagePrompt(exit_status, prompt, cwd);
        //snprintf(prompt, sizeof(prompt), "fsh > ");

        command = readline(prompt);        
        if (!command) { // On sort si Ctrl+D (EOF)
            if (write(STDOUT_FILENO, "\n", 1) == -1) {
                perror("write");
                exit_status = 1;
            }
            break;
        }


        // ajoute que si pas vide
        if (*command != '\0') {
            add_history(command);
        }
        

      /*  // Réinitialiser les arguments avant d'exécuter la commande
            memset(args, 0, sizeof(args)); 
             // pour séparé la commande des arguments
            parse_command(command, args);
            // continue
            if (args[0] == NULL) {
                free(command);
                continue;
            }

            exit_status = execute_command(args[0], args, exit_status);
       */

      
        // exécute la commande et récupère le statut de retour

          // Réinitialiser les arguments avant d'exécuter la commande
            memset(args, 0, sizeof(args)); 
             // pour séparé la commande des arguments
            parse_command(command, args);
            
            // DEBUGG
            /*printf("DANS MAIN ");
            for (int i = 0; args[i] != NULL; i++) {
                printf("%s ", args[i]); // Print each part followed by a space
            }
            printf("\n");*/
            // continue
            if (args[0] == NULL) {
                free(command);
                continue;
            }

            exit_status = execute_command(args[0], args, exit_status);
       /* if(strncmp(command, "if ", 3) == 0){
           
            exit_status =  comm_if(command);
          
        }
        else if (strncmp(command, "for ", 4) == 0) {
           
            exit_status = comm_for(command);
            
        }else if(strchr(command, ';') != NULL){
            exit_status =  commandeStruc(&command);
        }
        
        else if(strchr(command, '>') != NULL || strchr(command, '<') != NULL){
            exit_status = handle_redirection(command);

        }
        else{
             // Réinitialiser les arguments avant d'exécuter la commande
            memset(args, 0, sizeof(args)); 
             // pour séparé la commande des arguments
            parse_command(command, args);
            // continue
            if (args[0] == NULL) {
                free(command);
                continue;
            }

            exit_status = execute_command(args[0], args, exit_status);
            
        }*/
        
        free(command);
    }

    // dernier status du shell
    return exit_status;
}
