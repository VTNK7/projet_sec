#include <string.h>
#include "listejob.h"
#include <stdio.h>   /* printf */
#include <unistd.h>  /* fork */
#include <stdlib.h>  /* EXIT_SUCCESS */
#include <stdbool.h> /* EXIT_SUCCESS */
#include <signal.h> 



int id_global = 0;
liste liste_global;
liste *pl = &liste_global;


void init(liste *l)
{
    l = (liste*)malloc(sizeof(liste));
    l->suivant = NULL;
}


void add_job(job j)
{

    liste *element = malloc(sizeof(liste));
    if (!element)
    {
        exit(EXIT_FAILURE);
    }
    element->job = j;
    element->suivant = pl;

    pl = element; /* Le pointeur pointe sur le dernier élément. */
}

void del_job(int i)
{
    liste *l_temp = malloc(sizeof(liste));
    l_temp = pl;
    if (l_temp->job.identifiant == i)
    {
        liste *tmp = l_temp->suivant;
        l_temp = tmp;
    }
    else
    {
        while (l_temp->suivant != NULL)
        {
            if (l_temp->suivant->job.identifiant == i)
            {
                liste *tmp = l_temp->suivant->suivant;
                l_temp->suivant = tmp;
            }
        }
    }
    pl = l_temp;
};

// retourne le pid du job et affiche 0
pid_t stop_job(int i)
{
    liste *l_temp = malloc(sizeof(liste));
    l_temp = pl;

    while (l_temp->suivant != NULL)
    {
        if (l_temp->job.identifiant == i)
        {
            l_temp->job.etat = 0;
            return l_temp->job.pid;
        }
        l_temp = l_temp->suivant;
    }
    return -1;
};

pid_t cont_job(int i)
{
    liste *l_temp = malloc(sizeof(liste));
    l_temp = pl;

    while (l_temp->suivant != NULL)
    {
        if (l_temp->job.identifiant == i)
        {
            l_temp->job.etat = 1;
            return l_temp->job.pid;
        }
        l_temp = l_temp->suivant;
    }
    return -1;
};

void show_jobs()
{
    liste *l_temp = malloc(sizeof(liste));
    l_temp = pl;
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
    id_global++;
    new_job.identifiant = id_global;
    new_job.pid = pidFils;
    new_job.etat = 1;
    // 255
    new_job.commande = malloc(strlen(cmd));
    strcpy(new_job.commande, cmd);
    return new_job;
}

int get_id(pid_t pid){
    liste *l_temp = malloc(sizeof(liste));
    l_temp = pl;

    while (l_temp->suivant != NULL)
    {
        if (l_temp->job.pid == pid)
        {
            return l_temp->job.identifiant;
        }
        l_temp = l_temp->suivant;
    }
    return -1;
}
pid_t get_pid(int id){
    liste *l_temp = malloc(sizeof(liste));
    l_temp = pl;

    while (l_temp->suivant != NULL)
    {
        if (l_temp->job.identifiant == id)
        {
            return l_temp->job.pid;
        }
        l_temp = l_temp->suivant;
    }
    return -1;
}


