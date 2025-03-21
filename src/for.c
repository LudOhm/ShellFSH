#include "for.h"

#define MAX_CMD_LENGTH 1024
#define OPT_A (1 << 0) // Inclure fichiers cachés
#define OPT_R (1 << 1) // Parcours récursif

// Pour afficher les messages d'erreur
void err_write(const char *message) {
    if( write(STDERR_FILENO, message, strlen(message)) < 0){
        perror("write");
        exit(1);
    }    
}

int parse_for_command(char *input, char **variable, char **rep, char **options, char** cmd) {
    int erreur_syntaxe = 2;
    int succes = 0;
    int cmd_index = 0, opt_index = 0/*, rep_index*/;
    int profondeur = 0; // Pour gérer les blocs avec des accolades imbriquées
    bool /*dans_rep = false, */dans_commands = false;

    *options = 0;  // Initialisation des options à 0
    

    char *copie = strdup(input);
    char *token = strtok(copie, " \t\n"); // on est a for
    
    // Lecture de la variable
    token = strtok(NULL, " \t\n");
    if (!token || strlen(token) != 1) {
        perror("erreur de syntaxe : variable invalide");
        goto error;
    }
    *variable = strdup(token);
    
    // Vérification du mot-clé "in"
    token = strtok(NULL, " \t\n");
    if(strcmp(token, "in")!= 0){
        perror("erreur de syntaxe");
        goto error;
    }
    
    // recupération du rep
    token = strtok(NULL, " \t\n");
    if(!token ||token[0] == '-' ||  strcmp(token, "{") == 0 || strcmp(token, "}") == 0){
        perror("erreur de syntaxe");
        goto error;
    }
    //dans_rep = true;
    rep[0] = strdup(token);
    
    //dans_rep = false;
    
    
    // Lecture du répertoire ou de la liste de fichiers
    /*if (!token || strcmp(token, "{") == 0 || strcmp(token, "}") == 0) {
        perror("erreur de syntaxe : répertoire manquant");
        goto error;
    }
    *rep = strdup(token);*/
    // Passage au bloc de commandes
    
    /*token = strtok(NULL, " \t\n");
    if (!token || strcmp(token, "{") != 0) {
        perror("erreur de syntaxe : attendu '{'");
        goto error;
    }
    dans_rep = false;
    profondeur = 1; */
    
    /* on passe aux options ( dans parse on recupere juste un tableau avec ttes les options et c'est une autre fonctions qui s'occupera de les activer
     attention : le tab options contiens egalment les filtres
     
     */
    token = strtok(NULL, " \t\n");
    while(token != NULL && token[0] == '-') {
        options[opt_index] = strdup(token);  // Copier l'option
        opt_index++;
        if (token[1] == 'e') {
            token = strtok(NULL, " \t\n");
            if (!token) {
                perror("option -e: extension manquante");
                goto error;
            }
            options[opt_index] = strdup(token);  // Copier l'extension
            opt_index++;
        } else if (token[1] == 't') {
            token = strtok(NULL, " \t\n");
            if (!token || strlen(token) != 1 ||
                (token[0] != 'f' && token[0] != 'd' && token[0] != 'l' && token[0] != 'p')) {
                perror("option -t: attendu 'f', 'd', 'l' ou 'p'");
                goto error; // j'ai un doute si dans ce cas la c pas 1 la valeur de retour attendue et non 2
            }
            options[opt_index] = strdup(token);  // Type de filtre
            opt_index++;
        } else if (token[1] == 'p') {
            token = strtok(NULL, " \t\n");
            if (!token || !isdigit(token[0])) {
                perror("option -p: valeur MAX manquante ou invalide");
                goto error;
            }
            // Vérification robuste de la valeur de MAX
            char *endptr;
            long max_value = strtol(token, &endptr, 10);
            if (*endptr != '\0' || max_value <= 0) {
                perror("option -p: valeur MAX invalide (doit être un entier strictement positif)");
                goto error;
            }

            // Stocker uniquement la valeur de MAX
            char max_value_str[20];
            snprintf(max_value_str, sizeof(max_value_str), "%ld", max_value);
            options[opt_index] = strdup(max_value_str);
            opt_index++;
        }
        
        // else  option invalide à gerer dans une fonction a part dediee aux options
        
        // on continue d'avancer
        token = strtok(NULL, " \t\n");
    }
    // Terminer le tableau d'options
    options[opt_index] = NULL;

    if(strcmp(token, "{") != 0 ){
        perror("erreur de syntaxe : attendu '{'");
        goto error;
    }
    profondeur ++;
    token = strtok(NULL, " \t\n");
    dans_commands = true;
    // Extraction des commandes entre accolades
    while (token != NULL) {
        if (strcmp(token, "{") == 0) {
            if(profondeur > 0){
                cmd[cmd_index++] = strdup(token);
            }
            profondeur++;
        } else if (strcmp(token, "}") == 0) {
            profondeur--;
            if (profondeur == 0) {
                dans_commands = false;
                break;
            }else{
                cmd[cmd_index++] = strdup(token);
            }
        } else if (dans_commands) {
            cmd[cmd_index++] = strdup(token);
        } else {
            perror("erreur de syntaxe : argument inattendu");
            goto error;
        }
        token = strtok(NULL, " \t\n");
    }

    // Terminer les tableaux
    cmd[cmd_index] = NULL;

    // Vérification finale
    if (profondeur != 0) {
        perror("erreur de syntaxe : accolades non équilibrées");
        goto error;
    }

    free(copie);
    return succes;

error:
    return erreur_syntaxe;
}


void replace_all_placeholders(char *cmd_with_f, const char *cmd_template, const char *path, const char *variable, const char *ext_filter, bool amputate_extension) {
    const char *placeholder = cmd_template;
    char *write_ptr = cmd_with_f;

    while ((placeholder = strstr(placeholder, "$")) != NULL) {
        if (placeholder[1] && ((placeholder[1] >= 'A' && placeholder[1] <= 'Z') || (placeholder[1] >= 'a' && placeholder[1] <= 'z'))) {
            if (variable && placeholder[1] == *variable) {
                size_t prefix_length = placeholder - cmd_template;
                memcpy(write_ptr, cmd_template, prefix_length);
                write_ptr += prefix_length;

                // Remplacement de $F avec ou sans amputation d'extension
                if (amputate_extension && ext_filter) {
                    const char *dot = strrchr(path, '.');
                    if (dot && strcmp(dot + 1, ext_filter) == 0) {
                        size_t base_length = dot - path;
                        memcpy(write_ptr, path, base_length);
                        write_ptr += base_length;
                    } else {
                        strcpy(write_ptr, path);  // Pas d'extension correspondante
                        write_ptr += strlen(path);
                    }
                } else {
                    strcpy(write_ptr, path);  // Pas d'amputation nécessaire
                    write_ptr += strlen(path);
                }

                cmd_template = placeholder + 2;  // Passer $ et la lettre
            } else {
                size_t prefix_length = (placeholder + 1) - cmd_template;
                memcpy(write_ptr, cmd_template, prefix_length);
                write_ptr += prefix_length;
                cmd_template = placeholder + 1;
            }
        } else {
            *write_ptr = *placeholder;
            write_ptr++;
            cmd_template = placeholder + 1;
        }
        placeholder = cmd_template;
    }

    strcpy(write_ptr, cmd_template);
}



int process_directory(const char *rep, const char *cmd_template, int options, const char *ext_filter, char type_filter,const char *variable) {
    DIR *dir = opendir(rep);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    struct stat st;
    int max_exit_status = 0;

    while ((entry = readdir(dir)) != NULL) {
        //printf("Débogage : Entrée trouvée : %s\n", entry->d_name);
        // Ignorer les fichiers cachés sauf si -A est activé
        if (!(options & OPT_A) && entry->d_name[0] == '.') continue;
        if ((options & OPT_A) && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) continue;  // Exclure "." et ".." si -A est activé

        // Construire le chemin complet
        size_t path_length = strlen(rep) + strlen(entry->d_name) + 2;
        char *path = malloc(path_length);
        if (path == NULL) {
            perror("malloc");
            closedir(dir);
            return 1;
        }
        snprintf(path, path_length, "%s/%s", rep, entry->d_name);

        if (lstat(path, &st) == -1) {
            perror("lstat");
            free(path);
            continue;
        }

        // Filtrage par type pour le -t
        //printf("Debug: Processing %s with type_filter=%c\n", path, type_filter);
        if (type_filter) {
            if ((type_filter == 'f' && !S_ISREG(st.st_mode)) ||
                (type_filter == 'd' && !S_ISDIR(st.st_mode)) ||
                (type_filter == 'l' && !S_ISLNK(st.st_mode)) ||
                (type_filter == 'p' && !S_ISFIFO(st.st_mode))) {
                    if ((options & OPT_R) && S_ISDIR(st.st_mode) &&
                        strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        int recursive_status = process_directory(path, cmd_template, options, ext_filter, type_filter, variable);
                        if (recursive_status > max_exit_status) {
                            max_exit_status = recursive_status;
                        }
                    }
                free(path);
                continue;
            }
        }

        // Préparer la commande avec le placeholder remplacé
        char cmd_with_f[MAX_CMD_LENGTH];
        //memset(cmd_with_f, 0, sizeof(cmd_with_f));

        // Filtrage et modification de la variable pour -e EXT
        if (ext_filter) {
            const char *dot = strrchr(entry->d_name, '.');
            if (!dot || strcmp(dot + 1, ext_filter) != 0) {
                free(path);
                continue; // Fichier ignoré
            }

            // Construire la commande
            //printf("Débogage : Commande modèle avant remplacement : %s\n", cmd_template);
            //printf("Débogage : Chemin remplacé : %s\n", path);
            //printf("Débogage : variable process_directory : %s\n", variable);
            replace_all_placeholders(cmd_with_f, cmd_template, path, variable , ext_filter, 1);
        } else {
            // Pas de filtrage ou modification nécessaire
            //printf("Débogage : Commande modèle avant remplacement : %s\n", cmd_template);
            //printf("Débogage : Chemin remplacé : %s\n", path);
            //printf("Débogage : variable process_directory : %s\n", variable);
            replace_all_placeholders(cmd_with_f, cmd_template, path, variable, ext_filter, 0);
        }

        /*
        // Vérifier que la commande a bien été construite
        if (strlen(cmd_with_f) == 0) {
            err_write("Erreur : commande vide \n");
            free(path);
            continue;
        }*/
        //printf("Débogage : Commande avec $ remplacé : %s\n", cmd_with_f);

        // Découper la commande modifiée en arguments
        char *args[MAX_CMD_LENGTH / 2];
        int arg_count = 0;
        char *token = strtok(cmd_with_f, " ");
        while (token && arg_count < (MAX_CMD_LENGTH / 2 - 1)) {
            args[arg_count++] = strdup(token);
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        //printf("Débogage : Construction du chemin : %s/%s\n", rep, entry->d_name);
        // Exécuter la commande
        int exit_status = execute_command(args[0], args, max_exit_status);
        if (exit_status > max_exit_status) {
            max_exit_status = exit_status;
        }

        // Nettoyage des arguments
        for (int i = 0; i < arg_count; i++) {
            free(args[i]);
        }

        // Gestion récursive si -r est activé
        if ((options & OPT_R) && S_ISDIR(st.st_mode) &&
            strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            int recursive_status = process_directory(path, cmd_template, options, ext_filter, type_filter, variable);
            if (recursive_status > max_exit_status) {
                max_exit_status = recursive_status;
            }
        }

        free(path);
    }

    closedir(dir);
    return max_exit_status;
}


int comm_for(char *command) {
    char *rep[100] = {0}, *cmd[100] = {0}, *options[100] = {0};
    char *variable = NULL;
    int max_tour = 1 ; //Valeur de base si pas -p

    // Analyse de la commande for
    int parse_result = parse_for_command(command, &variable, rep, options, cmd);
    if (parse_result != 0) {
        err_write("Erreur de syntaxe dans la commande for\n");
        return parse_result;
    }

    // Concaténation des commandes entre accolades
    char cmd_template[MAX_CMD_LENGTH] = "";
    for (int i = 0; cmd[i] != NULL; i++) {
        strcat(cmd_template, cmd[i]);
        if (cmd[i + 1] != NULL) {
            strcat(cmd_template, " ");
        }
    }

    // Analyse des options
    int opt_flags = 0;
    const char *ext_filter = NULL;
    char type_filter = 0;

    for (int i = 0; options[i] != NULL; i++) {
        if (strcmp(options[i], "-A") == 0) {
            opt_flags |= OPT_A;
        } else if (strcmp(options[i], "-r") == 0) {
            opt_flags |= OPT_R;
        } else if (strcmp(options[i], "-e") == 0 && options[i + 1] != NULL) {
            ext_filter = strdup(options[++i]);
        } else if (strcmp(options[i], "-t") == 0 && options[i + 1] != NULL) {
            type_filter = options[++i][0];
        } else if (strcmp(options[i], "-p") == 0 && options[i + 1] != NULL) {
            max_tour = atoi(options[++i]);
            if (max_tour <= 0) {
                err_write("Valeur invalide pour -p MAX\n");
                return 2;
            }
        } else {
            err_write("Option invalide dans la commande for\n");
            return 2;
        }
    }

    /*printf("Débogage : variable comm_for : %s\n", variable);
    printf("Débogage : Commande analysée : %s\n", command);
    printf("Débogage : Commande complète générée : %s\n", cmd_template);

    for (int i = 0; rep[i] != NULL; i++) {
        printf("Débogage : Répertoire[%d] = %s\n", i, rep[i]);
    }

    for (int i = 0; options[i] != NULL; i++) {
        printf("Débogage : Option[%d] = %s\n", i, options[i]);
    }*/


    // Appeler la fonction process_directory
    int max_exit_status = process_directory(rep[0], cmd_template, opt_flags, ext_filter, type_filter,variable);

    // Nettoyage
    free(variable);
    for (int i = 0; rep[i] != NULL; i++) free(rep[i]);
    for (int i = 0; cmd[i] != NULL; i++) free(cmd[i]);
    if (ext_filter) free((void *)ext_filter);

    return max_exit_status;
}
