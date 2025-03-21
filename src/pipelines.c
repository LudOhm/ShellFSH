#include "pipelines.h"

bool isPipeline(char **input){
    // 0 == pas de  |  1 = oui
    for (char **ptr = input; *ptr != NULL; ptr++) {
        for (int j = 0; (*ptr)[j] != '\0'; j++) {
            if ((*ptr)[j] == '|') {
                if((*ptr)[j-1] != '>'){
                    // pour distinguer des redirections avec écrasement
                     return true;
                }
               
            }
        }
    }
    return false;
}


int pipeline(char ** input){
    int exit_status = 0;
    char inputString[1024] = "";
    recupere_String(input, inputString);
    char liste_commandes[100][1024] = {0};
    unsigned int n= separer_pipeline(inputString,liste_commandes);
    if(n<0){
        // erreur de syntaxe
        exit_status = 2;
        goto error;
    }
    int fd[2]; // lecture = 0 ecriture = 1
    int fd_precedent = -1; // pour savoir depuis ou l'enfant devra lire
    

    // structure un seul père plusieurs fils
    for(int i=0; i<n; i++){
       
        if(i < n-1 ){
            if (pipe(fd) != 0) { // on cree un tube sauf si derniere cmd
                perror("pipe");
                exit_status = 1;
                goto error;
            }
        }
        pid_t pid = fork();
        if(pid < 0){
            perror("fork");
            exit_status = 1;
            goto error; 
        }
		if(pid==0) {
            // enfant
            // SIGNAL
            struct sigaction action = {0};
            action.sa_handler = SIG_DFL;
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGTERM, &action, NULL);

            if(fd_precedent != -1){ // si c pas la 1ere cmd
                dup2(fd_precedent , 0);  // on redirige stdin
                close(fd_precedent);
            }

             if(i < n-1 ){ // si c pas la derniere cmd
                // on redirige stdout
               
                dup2(fd[1], 1);
                close(fd[1]);
                close(fd[0]); 
                
             }
            char *args[100] = {0};
            //printf("DEBUG pipeline %s\n" , liste_commandes[i]);
            char *copie = strdup(liste_commandes[i]);
            parse_command(copie, args);
            //printf("DEBUG pipeline args[0] %s\n" , args[0]);
            
            // on regarde si il y a redirection
            if(isRedir(args)){
                bool premiere = (i==0);
                bool derniere = (i==n-1);
                if (!redirectionPipelineValide(args, premiere, derniere)){
                        exit_status = 2;
                        exit(exit_status);
                }
                char *redirection[100]; // répétitif doit donner même res que args
                 // pour redirections sortie standard 
                char * FIC_out = NULL;
                int flag_out = -1;
                // pour redirection entrée standard
                char * FIC_in = NULL ;
                int flag_in = -1;
                // pour redirections  sortie erreur standard
                char * erreur_output = NULL;
                int erreur_flags = -1;

                // TEST
                /*if(i == n-1 ) {

                    printf("liste commande %s\n", liste_commandes[i]);
                    
                }*/
                char *copie = strdup(liste_commandes[i]);
                if(parse_redirection(copie, redirection, &FIC_out, &flag_out, &FIC_in, &flag_in, &erreur_output, &erreur_flags) != 0){
                    exit_status = 2; 
                    exit(exit_status);
                    free(copie);
                } 
                 // TEST 
                /* if(i == n-1 ) {
                    for(int j = 0; redirection[j]!=NULL; j++){
                        printf("redir args %s\n", redirection[j]);
                    }
                 }*/

                if(pipelineRedirection(FIC_out,flag_out, FIC_in, flag_in, erreur_output, erreur_flags) !=0){
                    perror("redirection pipeline");
                    exit_status = 1; 
                    exit(exit_status);
                }

                // faut retirer la redirection de la commande
               
                exit_status= execvp(redirection[0], redirection);
                exit(exit_status);


            }
            else if(strcmp(args[0], "ftype") == 0 || strcmp(args[0], "cd") == 0 || strcmp(args[0], "pwd") == 0 || strcmp(args[0], "exit") == 0){
                exit_status= execute_command(args[0], args, exit_status);
                 exit(exit_status);

            }
			else{ // commande externe

                exit_status = execvp(args[0], args);
                if(exit_status == -1){
                    perror("execvp");
                    exit_status = 1;
                    exit(exit_status);
                }
            }
          
            exit(exit_status);
           
	    }
        else{
            // parent
            
            if(fd_precedent != -1){
                close(fd_precedent); // fermer le lecteur 
            }
            // tant qu'on est pas à la derniere commande parce que ducp pas besoin d'ecrire
            if(i < n-1 ){
                close(fd[1]);
                fd_precedent = fd[0];
            }else {
                close(fd[0]);
                close(fd[1]); 
            }
           
        }
    }
    while(wait(NULL)>=0){}  // le père doit attendre tous ses fils
    return exit_status;

 error :
    return exit_status;
    if(fd_precedent != -1){
        close(fd_precedent); //on sait jamais
    }
    close(fd[1]);
    close(fd[0]); 
}

int separer_pipeline(char* commande, char liste_cmd[100][1024]){
    int nb_commandes = 0;
    char current_commande[1024] = {0};
    int cmd_index = 0;
    for (int i = 0; commande[i] != '\0'; i++){
        char c = commande[i];
        if(c == '|'){
            // erreurs de syntaxe 
            if(i== 0 || commande[i+1] == '\0') return -1;

            // on ajoute cmd que si c pas un | de redirection
            if(commande[i-1]=='>') {
                // on est tjrs dans commande en cours
                current_commande[cmd_index] = c;
                cmd_index++;
            }else{
                if(commande[i-1]==' ') {
                 // on a trouvé une commande qui doit être sép
                    current_commande[cmd_index] = '\0';
                    strncpy(liste_cmd[nb_commandes], current_commande, 1024);
                    nb_commandes++;
                    cmd_index = 0; // on remet à 0 car on passe à une autre
                    // parce que on a un espace après le | normalement
                    while (commande[i + 1] == ' ') {
                        i++;
                    }
                }else{
                    // erreur de syntaxe
                    return -1;
                }
            }
        }else{
            // on est tjrs dans commande en cours
            current_commande[cmd_index] = c;
            cmd_index++;
        }
    }

    if (cmd_index > 0) {
        current_commande[cmd_index] = '\0';
        strncpy(liste_cmd[nb_commandes], current_commande, 1024);
        nb_commandes++;
    }

    return nb_commandes;
}
bool redirectionPipelineValide(char **args, bool premiere, bool derniere){
    for (int i = 0; i < 4; i++) {
        if(strcmp(args[i], "<") == 0 && !premiere){
            return false;
        }
        bool redirSTDout = strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], ">|") == 0;
        if(redirSTDout && !derniere){
            return false;
        }
    }
    return true;
}

int pipelineRedirection(char * FIC_out, int flag_out, char *FIC_in, int flag_in, char * err_out, int err_flag){
    int old_fd; int new_fd = -1;
    int res = 0;
     // redirections > >> >|
    if(FIC_out != NULL){
        old_fd = open(FIC_out, flag_out, 0666) ;
     
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
    if(err_out != NULL){
 
        if (stderr < 0) {
            perror("dup");
            res = 1;
            goto error;
        }
        old_fd = open(err_out, err_flag, 0666) ;
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

    return res;
    error : 
        if(old_fd >=0) close(old_fd);
        return res;
}
