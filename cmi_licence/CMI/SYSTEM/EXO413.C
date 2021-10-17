#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>

// fonction qui gere le signal
void f(int a)
{
	if(a==SIGKILL)
	{
		printf("signal SIGKILL(%d) recu\n",a);
		signal(SIGKILL,f);
	}
	if(a==SIGSTOP)
	{
		printf("signal SIGSTOP(%d) recu\n",a);
		signal(SIGSTOP,f); // pour attraper les autres fois
	}
}

int main()
{
	signal(SIGKILL,f);
	signal(SIGSTOP,f);
	
	while(1);
	
	return 0;
}
