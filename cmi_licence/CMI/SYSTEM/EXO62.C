#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>

#define VALSIG (SIGINT)
#define N (5)

int tube[N][2];

void fils(int t[2],int i)
{
  close(t[1]);
  // redirection de t[0] vers l'entree standard 
  close(0);
  dup(t[0]);
  close(t[0]);
  
  if(execlp("cat","cat",NULL)==-1)
    {
      printf("Erreur: impossible d'executer cat\n");
      return;
    }
}

void pere(void)
{
	char buf[50];
	int nb,i;
	
	printf("Entrez des caracters qui seront lu"
	       " par le pere:\n");
	nb=read(0,buf,sizeof(buf));
	
	// ecriture dans tous les fils
	for(i=0;i<N;i++)
	  {
	    write(tube[i][1],buf,nb);
	    close(tube[i][1]);
	  }
	printf("fin du pere\n");
}

int main()
{
	pid_t pid;
	int i;
	
	printf("Creation des %d fils\n",N);
	// creation des N fils
	for(i=0;i<N;i++)
	  {
	    pipe(tube[i]);
	    pid=fork();
	    switch(pid)
	      {
	      case -1: 
		printf("Erreur: impossible de creer un processus fils\n");
		break;
	      case 0:
		fils(tube[i],i);
		printf("fin du fils %d\n",i);
		return 0;
		break;
	      default:
		close(tube[i][0]);
		break;
	      }
	  }
	
	// traitement du pere
	pere();
	for(i=0;i<N;i++)
	  {
	    wait(NULL);
	  }
	printf("fin du programme\n");
	
	return 0;
}
