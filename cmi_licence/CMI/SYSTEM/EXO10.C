#include<stdio.h>

int main()
{
	printf("bonjour\n");
	printf("Mon pid est %d est mon ppid est %d\n",getpid(),getppid());
	
	return 0;
}
