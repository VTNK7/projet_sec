#include <stdio.h>   /* printf */
#include <unistd.h>  /* fork */
#include <stdlib.h>  /* EXIT_SUCCESS */
#include <stdbool.h> /* EXIT_SUCCESS */
#include "readcmd.h"
#include "listejob.h"
#include <string.h>
#include <signal.h> /* traitement des signaux */
#include <fcntl.h>    /* opérations sur les fichiers */

liste *pl;
pid_t pid_fork;
int tube[2];
void handler(int signal_num)
{

    int pid_fils, wstatus;

    while ((pid_fils = (int)waitpid(-1, &wstatus, WUNTRACED | WCONTINUED | WNOHANG)) > 0)
    {
        if (WIFSTOPPED(wstatus))
        {
            stop_job(get_id(pid_fils));
        }
        else if (WIFCONTINUED(wstatus))
        {
            cont_job(get_id(pid_fils));
        }
        else if (WIFEXITED(wstatus))
        {
            del_job(get_id(pid_fils));
        }
        else if (WIFSIGNALED(wstatus))
        {
            del_job(get_id(pid_fils));
        }
    }
}

void handler_z(int sig)
{
    kill(pid_fork, SIGSTOP);
}

void handler_c(int sig)
{
    printf("\n");
    kill(pid_fork, SIGTERM);
}

int main(int argc, char *argv[])
{

    int status, desc_fic, dup_err,n_pipe,sortie,pipe_fork;


    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    // j'ai utilisé signal car j'ai pas réussi a utiliser sigaction pour ces questions.
    // Il y avait des zsh: segmentation fault
    signal(SIGINT, &handler_c);
    signal(SIGTSTP, &handler_z);

    /*
    struct sigaction sa_z;
    sa_z.sa_handler = handler_z;
    sigemptyset(&sa_z.sa_mask);

    struct sigaction sa_c;
    sa_c.sa_handler = handler_c;
    sigemptyset(&sa_c.sa_mask);
    */

    init(pl);

    sigaction(SIGCHLD, &sa, NULL);
    //sigaction(SIGTSTP, &sa_z, NULL);
    //sigaction(SIGINT, &sa_c, NULL);

    while (1)
    {

        printf(">>>");
        struct cmdline *commande_utilisateur;
        commande_utilisateur = readcmd();

        if (commande_utilisateur->seq[0] != NULL)
        {

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
                if (com_id == NULL)
                {
                    printf("Veuillez rentrer un identifiant \n");
                }
                else
                {
                    pid_t pid_job = stop_job(atoi(commande_utilisateur->seq[0][1]));
                    kill(pid_job, SIGSTOP);
                }
            }
            else if (strcmp(commande_utilisateur->seq[0][0], "bg") == 0)
            {
                char *com_id = commande_utilisateur->seq[0][1];
                if (com_id == NULL)
                {
                    printf("Veuillez rentrer un identifiant \n");
                }
                else
                {
                    pid_t pid_job = cont_job(atoi(commande_utilisateur->seq[0][1]));
                    kill(pid_job, SIGCONT);
                }
            }
            else if (strcmp(commande_utilisateur->seq[0][0], "fg") == 0)
            {
                char *com_id = commande_utilisateur->seq[0][1];
                if (com_id == NULL)
                {
                    printf("Veuillez rentrer un identifiant \n");
                }
                else
                {
                    pid_t pid_job = cont_job(atoi(commande_utilisateur->seq[0][1]));
                    kill(pid_job, SIGCONT);
                    wait(&status);
                    del_job(get_id(pid_job));
                }
            }
            else
            {

                pid_fork = fork();
                add_job(new_job(pid_fork, commande_utilisateur->seq[0][0]));
                if (pid_fork == -1)
                {
                    printf(" Erreur fork \n ");
                    exit(1);
                    /* par convention, renvoyer une valeur > 0 en cas d’erreur */
                }
                if (pid_fork == 0)
                { /* fils */
                    if (commande_utilisateur->out != NULL)
                    {
                        desc_fic = open(commande_utilisateur->out, O_CREAT | O_WRONLY | O_TRUNC, 0777);
                        dup_err = dup2(desc_fic, 1);
                        if (dup_err == -1) {
                            perror("Erreur out");
                            exit(2);
                        }
                        close(desc_fic);
                    }

                    /* redirection in */
                    if (commande_utilisateur->in != NULL)
                    {
                        desc_fic = open(commande_utilisateur->in, O_CREAT | O_RDONLY, 0777);
                        dup_err = dup2(desc_fic, 0);
                        if (dup_err == -1) {
                            perror("Erreur in");
                            exit(3);
                        }
                        close(desc_fic);
                    }

                    /* execution avec pipes */

                    n_pipe=0;
                    sortie=0;

                    while (commande_utilisateur->seq[n_pipe+1]!=NULL) {
                        pipe(tube);
                        pipe_fork=fork();
                        if (pipe_fork==0){ 
                            dup2(sortie,0);
                            close(sortie);
                            dup2(tube[1],1);
                            close (tube[1]);
                            execvp(commande_utilisateur->seq[n_pipe][0],commande_utilisateur->seq[n_pipe]);                                
                        }
                        n_pipe++;
                        sortie=tube[0];
                        close(tube[1]);
                    }
                    dup2(sortie,0);
                    execvp(commande_utilisateur->seq[n_pipe][0],commande_utilisateur->seq[n_pipe]);
                
                }
                else
                { /* pere*/

                    if (commande_utilisateur->backgrounded == 0)
                    {
                        int idsuppr = get_id(pid_fork);
                        wait(&status);
                        del_job(idsuppr);
                    }
                }
            }
        }
    }
}
