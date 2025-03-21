#include "commandeStructuree.h"
/* TO-DO  améliorer gestion erreur*/

int isStruct(char **input){
    // 0 == pas de  ;  1 = oui
    for (char **ptr = input; *ptr != NULL; ptr++) {
        for (int j = 0; (*ptr)[j] != '\0'; j++) {
            if ((*ptr)[j] == ';') {
                return 1;
            }
        }
    }
    return 0;
}

int commandeStruc(char ** input){
    // CMD 1 puis CMD 2 jusqu'à CMDN
    // on stockera les commandes dan sun tableu de String
    int exit_status = -1;
    char inputString[1024];
    recupere_String(input, inputString);
    char liste_commandes[100][1024];
  
    unsigned int n= separer_cmd(inputString, ";", liste_commandes);
    // on parcourt input pour remplir liste_commande
    for(unsigned int i = 0 ; i < n ; i++){
        char *args[100] = {0};
        //printf("DEBUG commande struct %s\n" , liste_commandes[i]);
        parse_command(liste_commandes[i], args);
        //printf("DEBUG commande struct args[0] %s\n" , args[0]);
        exit_status = execute_command(args[0], args, exit_status);
	}

    return exit_status;
}

// pour etre sur que dans tous les cas on manipule char *
void recupere_String(char **input, char *result) {
    result[0] = '\0';
    for (int i = 0; input[i] != NULL; i++) {
        strcat(result, input[i]);
        if (input[i + 1] != NULL) { // sinon y a un espace en trop
            strcat(result, " ");
        }
    }
}




int separer_cmd(char* commande, char * separateur, char liste_cmd[100][1024] ){
    int nb_commandes = 0;
    int profondeur = 0;
    char current_commande[1024] = {0};
    int cmd_index = 0;

        for (int i = 0; commande[i] != '\0'; i++) {
            char c = commande[i];
            // il faut qu'on différencie les ';' qui séparent de ceux qui font partie d'une cmd
            if (c == '{') {
                profondeur++;
            } else if (c == '}') {
                profondeur--;
            }
        
            if (c == separateur[0] && profondeur == 0) {
                // on a trouvé une commande qui doit être sép
                current_commande[cmd_index] = '\0';
                strncpy(liste_cmd[nb_commandes], current_commande, 1024);
                nb_commandes++;
                cmd_index = 0; // on remet à 0 car on passe à une autre
                // parce que on a un espace après le ; normalement
                while (commande[i + 1] == ' ') {
                                i++;
                }
                
            } else {
                // on est tjrs dans commande en cours
                current_commande[cmd_index] = c;
                cmd_index++;
            }
        }

    // etre sur qu'on a bien tt ajouter
        if (cmd_index > 0) {
            current_commande[cmd_index] = '\0';
            strncpy(liste_cmd[nb_commandes], current_commande, 1024);
            nb_commandes++;
        }

    return nb_commandes;
}

int comm_if( char * commande ){
    char *test[100] = {0};
    char *ifTrue[100] = {0};
    char *_else[100] = {0};
    int parse = parseIf(commande, test, ifTrue, _else);
    if(parse != 0 ){
        perror("erreur de syntaxe");
        return 2;
    }

    int exit_status = -1;

    exit_status = execute_command(test[0], test, exit_status);
    if(exit_status == 0){
        // DEBUGG
           /* printf("DANS COMM IF  ");
            for (int i = 0; ifTrue[i] != NULL; i++) {
                printf("%s ", ifTrue[i]); // Print each part followed by a space
            }
            printf("\n");*/
        exit_status = execute_command(ifTrue[0], ifTrue, exit_status);
    } else if(exit_status != 0 && _else[0] == NULL){
        // si test = false et pas de else
        exit_status = 0;
    } else if(exit_status != 0){
        // exécution du else 
        exit_status = execute_command(_else[0], _else, exit_status);
    }
    return exit_status;
}

// version bis pour essayer de mieux gérer les cas avec if et for dans la même commande.
int parseIf(char * commande, char** test , char** ifTrue  , char ** _else){
    int erreur_syntaxe = 2; int succes = 0;
    int test_index = 0, ifTrue_index=0, else_index=0;
    int profondeur = 0; // pour savoir si on a une accolade qui appartient au if ou à un for à l'interieur 
    bool dans_test=false, dans_else=false, dans_ifTrue=false; 

    char *copie = strdup(commande);
    char *token = strtok(copie, " \t\n"); // on est au if 

    token = strtok(NULL, " \t\n");
    dans_test = true; 
    // premiere erreur de syntaxe possible : on veut pas accepter "if {}"
    if(strcmp(token, "{") == 0 || strcmp(token, "}") == 0 ){
        perror("erreur de syntaxe : argument manquant");
        goto error;
    }
    

    while(token != NULL){
        // on isole le test => jusqu'à ue accolade ouvrante
        if(dans_test){
           if(strcmp(token, "{") == 0){
                dans_test = false;
                dans_ifTrue= true;
                profondeur = 1;
            }
            else{
                test[test_index] =token /*strdup(token)*/;
                test_index++; 
            }
        }
        
        else if(dans_ifTrue){
            if(strcmp(token, "{") == 0){
                // par exemple si on a un for dans cette partie
                if(profondeur > 0){
                    // n'appartien pas au bloc originel donc on ajoute
                    ifTrue[ifTrue_index] = token /*strdup(token)*/;
                    ifTrue_index++;
                }
                profondeur ++;

            }
            else if(strcmp(token, "}") == 0){
                profondeur --;
                if(profondeur == 0){
                    dans_ifTrue = false;
                    // on fait attention si y a un else ou non sinon on peut break;
                    token = strtok(NULL, " \t\n");
                    if(token != NULL && strcmp(token, "else") == 0 ){
                        dans_else = true;
                        // Ajout controle erreur de syntaxe mot en trop apres le else
                        token = strtok(NULL, " \t\n");
                        if(token != NULL && strcmp(token, "{") == 0 ){
                             if(profondeur > 0){
                                 // n'appartien pas au bloc originel donc on ajoute
                                _else[else_index] = token /*strdup(token)*/;
                                else_index++;
                            }
                            profondeur ++;
                        }else{
                            perror("erreur de syntaxe : '{' attendue après else");
                            goto error;
                        }
                    }else if(token != NULL  && strcmp(token, ";") != 0 && strcmp(token, "|") != 0 ){
                        // y 'a un argument en trop si toke non nul et pas pipeline ou commande struct
                        perror("erreur de syntaxe : argument en trop");
                        goto error;
                    }
                }else{
                    ifTrue[ifTrue_index] = token /*strdup(token)*/;
                    ifTrue_index++;
                }
                
            }else{
                ifTrue[ifTrue_index] = token /*strdup(token)*/;
                ifTrue_index++;
            }
            
        }
        else if(dans_else){
            if(strcmp(token, "{") == 0){
                if(profondeur > 0){
                    // n'appartien pas au bloc originel donc on ajoute
                    _else[else_index] = token /*strdup(token)*/;
                    else_index++;
                }
                profondeur ++;
            }
            else if(strcmp(token, "}") == 0){
                profondeur --;
                if(profondeur == 0){
                    dans_else = false;
                    break;
                } else{
                    _else[else_index] =token /*strdup(token)*/;
                    else_index++;
                }
            }else{
                _else[else_index] =token /*strdup(token)*/;
                else_index++;
            }
        }
        else{
            // il y ' a eu une erreur de syntaxe qqpart
            perror("erreur de syntaxe");
            goto error;
        }

        token = strtok(NULL, " \t\n");
    }

    // vérification supp 
    if(profondeur != 0 || dans_else == true || dans_ifTrue  == true || dans_test  == true ){
            perror("erreur de syntaxe");
            goto error;
    }

    test[test_index] = NULL;
    ifTrue[ifTrue_index] = NULL;
    _else[else_index] = NULL;
    
    return succes;

    error :
        return erreur_syntaxe;
}
