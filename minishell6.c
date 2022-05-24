#include <stdio.h>   /* printf */
#include <unistd.h>  /* fork */
#include <stdlib.h>  /* EXIT_SUCCESS */
#include <stdbool.h> /* EXIT_SUCCESS */
#include "readcmd.h"
#include <string.h>
#include <signal.h> /* traitement des signaux */

struct job
{
    int identifiant;
    pid_t pid;
    int etat;
    char *commande;
};

typedef struct job job;

typedef struct liste liste;

struct liste
{
    job job;
    liste *suivant;
};

int id = 0;
liste l;
liste *pl = &l;

void init(liste *l)
{
    l = (liste*)malloc(sizeof(liste));
    l->suivant = NULL;
}

void add_job(liste **l, job j)
{

    liste *element = malloc(sizeof(liste));
    if (!element)
    {
        exit(EXIT_FAILURE);
    }
    element->job = j;
    element->suivant = *l;

    *l = element; /* Le pointeur pointe sur le dernier élément. */
}

void del_job(pid_t i)
{
    printf("bonspoir");
    liste *l_temp = pl;
    if (l_temp->job.pid == i)
    {
        liste *tmp = l_temp->suivant;
        l_temp = tmp;
    }
    else
    {
        while (l_temp->suivant != NULL)
        {
            if (l_temp->suivant->job.pid == i)
            {
                liste *tmp = l_temp->suivant->suivant;
                l_temp->suivant = tmp;
            }
        }
    }
    pl = l_temp;
};

void stop_job(liste *l, int i)
{
    liste *l_temp = l;

    while (l_temp->suivant != NULL)
    {
        if (l_temp->job.identifiant == i)
        {
            l_temp->job.etat = 0;
        }
        l_temp = l_temp->suivant;
    }
};

void show_jobs(liste *l)
{
    liste *l_temp = l;
    printf("id   pid     etat   cmd\n");
    while (l_temp->suivant != NULL)
    {
        printf("%d    %d   %d      %s  \n", l_temp->job.identifiant, l_temp->job.pid, l_temp->job.etat, l_temp->job.commande);

        l_temp = l_temp->suivant;
    }
}

job new_job(pid_t pidFils, char *cmd)
{
    job new_job;
    id++;
    new_job.identifiant = id;
    new_job.pid = pidFils;
    new_job.etat = 1;
    new_job.commande = malloc(strlen(cmd));
    strcpy(new_job.commande, cmd);
    return new_job;
}

void handler(int signal_num)
{
    
    int pid_fils, wstatus;
    
    while ((pid_fils = (int)waitpid(-1, &wstatus, WUNTRACED | WCONTINUED | WNOHANG)) > 0)
    {
        kill(pid_fils, 9);
        del_job(pid_fils);
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
            stop_job(pl, atoi(commande_utilisateur->seq[0][1]));
        }
        else
        {
            pid_t pidFils = fork();

            if (pidFils == -1)
            {
                printf(" Erreur fork \n ");
                exit(1);
                /* par convention, renvoyer une valeur > 0 en cas d’erreur */
            }
            if (pidFils == 0)
            { /* fils */
                int s = execvp(commande_utilisateur->seq[0][0], commande_utilisateur->seq[0]);

                exit(s); /* bonne pratique : terminer les processus par un exit explicite */
            }
            else
            { /* pere*/
                add_job(&pl, new_job(pidFils, commande_utilisateur->seq[0][0]));
                if (commande_utilisateur->backgrounded == 0)
                {
                    pause();
                }
            }
        }
    }
}
