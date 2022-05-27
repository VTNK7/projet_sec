#include <stdio.h>   /* printf */
#include <unistd.h>  /* fork */
#include <stdlib.h>  /* EXIT_SUCCESS */
#include <stdbool.h> /* EXIT_SUCCESS */
#include "readcmd.h"
#include "listejob.h"
#include <string.h>
#include <signal.h> /* traitement des signaux */

liste *pl;

void handler(int signal_num)
{
    
    int pid_fils, wstatus;
    
    while ((pid_fils = (int)waitpid(-1, &wstatus, WUNTRACED | WCONTINUED | WNOHANG)) > 0)
    {
        //kill(pid_fils, 9);
        //del_job(pid_fils);
    }
    
}

int main(int argc, char *argv[])
{

    int status;

    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    init(pl);

    sigaction(SIGCHLD, &sa, NULL);

    while (1)
    {

        printf(">>>");
        struct cmdline *commande_utilisateur;
        
        while((commande_utilisateur = readcmd())==NULL);
        /*
        if (commande_utilisateur->err != 0){
            perror("Erreur commande \n");
        }*/

        if (strcmp(commande_utilisateur->seq[0][0], "exit") == 0)
        {
            exit(0);
        }
        else if (strcmp(commande_utilisateur->seq[0][0], "cd") == 0)
        {
            chdir(commande_utilisateur->seq[0][1]);
        }
        else if (strcmp(commande_utilisateur->seq[0][0], "lj") == 0)
        {
            show_jobs(pl);
        }
        else if (strcmp(commande_utilisateur->seq[0][0], "sj") == 0)
        {
            char *com_id = commande_utilisateur->seq[0][1];
            if (com_id == NULL){
                printf("Veuillez rentrer un identifiant \n");
            } else {
                pid_t pid_job = stop_job(pl, atoi(commande_utilisateur->seq[0][1]));
                kill(pid_job, SIGSTOP);
            }
        }
        else if (strcmp(commande_utilisateur->seq[0][0], "bg") == 0)
        {
            char *com_id = commande_utilisateur->seq[0][1];
            if (com_id == NULL){
                printf("Veuillez rentrer un identifiant \n");
            } else {
                pid_t pid_job = cont_job(pl, atoi(commande_utilisateur->seq[0][1]));
                kill(pid_job, SIGCONT);
            }
        }
        else if (strcmp(commande_utilisateur->seq[0][0], "fg") == 0)
        {
            char *com_id = commande_utilisateur->seq[0][1];
            if (com_id == NULL){
                printf("Veuillez rentrer un identifiant \n");
            } else {
                pid_t pid_job = cont_job(pl, atoi(commande_utilisateur->seq[0][1]));
                kill(pid_job, SIGCONT);
                wait(&status);
                del_job(pid_job);
            }
        }
        else
        {
            
            pid_t pidFils = fork();
            add_job(&pl, new_job(pidFils, commande_utilisateur->seq[0][0]));
            if (pidFils == -1)
            {
                printf(" Erreur fork \n ");
                exit(1);
                /* par convention, renvoyer une valeur > 0 en cas d’erreur */
            }
            if (pidFils == 0)
            { /* fils */
                int s = execvp(commande_utilisateur->seq[0][0], commande_utilisateur->seq[0]);
                if (s == -1)
                    { 
                        printf("%s : Commande inconnue\n", commande_utilisateur->seq[0][0]);
                        exit(s);
                    }
                exit(s); /* bonne pratique : terminer les processus par un exit explicite */
            }
            else
            { /* pere*/
                
                if (commande_utilisateur->backgrounded == 0)
                {
                    int idsuppr = get_id(pidFils);
                    wait(&status);     
                }
            }
        }
    }
}
