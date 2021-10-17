#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main()
{
	pid_t pid;
	char *argv[]={"./exo10",NULL};
	
	printf("Mon pid est %d\n",getpid());
	if(execv(argv[0],argv)==-1)
	{
		printf("Erreur:impossible de lancer la commande %s\n",
			argv[0]);
		return 1;
	}
}
