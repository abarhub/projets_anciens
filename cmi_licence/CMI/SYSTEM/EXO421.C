#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>

#define VALSIG (SIGCHLD)

int signal_recu=0;

void f(int a)
{
	if(a==VALSIG)
	{
		printf("le pere a recu le signal %d du fils\n",a);
		signal_recu=1;
	}
	
	signal(VALSIG,f); // pour attraper les autres fois
}

int main()
{
	signal(VALSIG,f);
	
	switch(fork())
	{
	case -1: 
		printf("Erreur: impossible de creer un processus fils\n");break;
	case 0:
		printf("fils creer\n");
		printf("fils: tache 1\n");
		printf("fils: tache 2\n");
		printf("fils: tache 3\n");
		printf("fin du fils\n");
		break;
	default:
		printf("Le pere attend le fils\n");
		wait(NULL);
		printf("Le pere a attendu le fils\n");
		break;
	}
	
	return 0;
}
