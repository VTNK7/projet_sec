#ifndef LISTEJOB
#define LISTEJOB
#include <unistd.h>  /* fork */

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

void init(liste *l);

void add_job(liste **l, job j);

void del_job(pid_t i);

void stop_job(liste *l, int i);

void show_jobs(liste *l);

job new_job(pid_t pidFils, char *cmd);


extern int id_global;
extern liste liste_global;


#endif

