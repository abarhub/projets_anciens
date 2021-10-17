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
		printf("signal %d recu\n",a);
	
	signal(VALSIG,f); // pour attraper les autres fois
}

int main()
{
	signal(VALSIG,f);
	
	while(1);
	
	return 0;
}
