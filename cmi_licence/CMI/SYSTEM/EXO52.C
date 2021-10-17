#include<stdio.h>
#include<unistd.h>

int main(int argc,char *argv[])
{
	pid_t pid;
		
	printf("Mon pid est %d\n",getpid());
	if(execv(argv[1],argv+1)==-1)
	{
		printf("Erreur:impossible de lancer la commande %s\n",
			argv[0]);
		return 1;
	}
}
