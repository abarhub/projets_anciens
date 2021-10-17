#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>

#define VALSIG (SIGINT)

void f(int a)
{
	if(a==VALSIG)
	{
		printf("le fils a recu le signal %d du pere\n"
			"attente de 5 seconde\n",a);
		sleep(5);
		printf("fin du fils\n");
		exit(0);
	}
	
	signal(VALSIG,f); // pour attraper les autres fois
}

int main()
{
	pid_t pid;
	
	pid=fork();
	switch(pid)
	{
	case -1: 
		printf("Erreur: impossible de creer un processus fils\n");break;
	case 0:
		signal(VALSIG,f);
		printf("fils creer\n");
		while(1);
		printf("fin du fils\n");
		break;
	default:
		printf("Le pere envoie le signal SIGINT au fils\n");
		kill(pid,SIGINT);
		printf("Le pere attend le fils\n");
		wait(NULL);
		printf("Le pere a attendu la mort du fils\n");
		break;
	}
	
	return 0;
}
