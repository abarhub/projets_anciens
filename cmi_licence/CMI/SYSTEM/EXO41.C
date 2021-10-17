#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>

void f(int a)
{
	printf("fin du fils\n");
	exit(0);
}

int main()
{
	pid_t pid;
	pid=fork();
	switch(pid)
	{
		case -1: printf("Erreur");break;
		case 0:/* fils1 */
			signal(SIGUSR1,f);
			sleep(10);
			break;
		default:/* pere */
			sleep(5);
			kill(pid,SIGUSR1);
			wait(NULL);
			printf("fin du pere\n");
			break;
	}
	
	return 0;
}