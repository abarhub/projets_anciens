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
	char *arg[]={"grep","\"toto\"",NULL};
	
	if(execvp(arg[0],arg)==-1)
	{
		printf("Erreur: impossible d'executer le fichier %s\n",arg[0]);
	}
}

void pere(void)
{
	char *arg[]={"ls","-l",NULL};
	
	if(execvp(arg[0],arg)==-1)
	{
		printf("Erreur: impossible d'executer le fichier %s\n",arg[0]);
	}
}

int main()
{
	pid_t pid;
	
	pipe(tube);
	printf("out=%d,in=%d\n",stdout,stdin);
	pid=fork();
	switch(pid)
	{
	case -1: 
		printf("Erreur: impossible de creer un processus fils\n");break;
	case 0:
		//close(tube[0]);
		close(1);
		dup(tube[1]);
		//close(tube[1]);
		fils();
		printf("fin du fils\n");
		break;
	default:
		//close(tube[1]);
		close(0);
		dup(tube[0]);
		//sleep(1);
		//close(tube[0]);
		pere();
		printf("fin du programme\n");
		break;
	}
	
	return 0;
}
