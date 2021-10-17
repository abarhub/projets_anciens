#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>

#define VALSIG (SIGINT)

int tube[2];

void fils(void)
{
	char buf[50];
	int nb;
	
	while(nb=read(tube[0],buf,sizeof(buf)))
	{
		printf("le fils a recu du pere:");fflush(stdout);
		write(1,buf,nb);
	}
}

void pere(void)
{
	char buf[50];
	int nb;
	
	printf("Entrez des caracters qui seront lu par le pere(^D pour finir):\n");
	while(nb=read(0,buf,sizeof(buf)))
	{
		write(tube[1],buf,nb);
	}
}

int main()
{
	pid_t pid;
	
	pipe(tube);

	pid=fork();
	switch(pid)
	{
	case -1: 
		printf("Erreur: impossible de creer un processus fils\n");break;
	case 0:
		close(tube[1]);
		fils();
		close(tube[0]);
		printf("fin du fils\n");
		break;
	default:
		close(tube[0]);
		pere();
		close(tube[1]);
		wait(NULL);
		printf("fin du programme\n");
		break;
	}
	
	return 0;
}
