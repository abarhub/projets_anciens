#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define N (5)

void *f(void* arg)
{
	printf("thread no %d\n",pthread_self());
	sleep(3);
	printf("fin du thread no %d\n",pthread_self());
	return NULL;
}

int main(int argc,char *argv[])
{
	int i,n;
	pthread_t thread[N];
	
	/*if(argc!=2)
	{
		n=5;
	}
	else
	{
		n=atoi(argv[1]);
		if(n<1||n>100) n=5;
	}*/
	//thread=(pthread_t*)malloc(sizeof(pthread_t)*n);
	n=N;
	for(i=0;i<n;i++)
	{
		if(pthread_create(thread+i,NULL,f,NULL))
		{
			printf("Erreur: Impossible de cree de thread"
				" no %d\n",i);
			n=i;
			break;
		}
		else
		{
			printf("thread %d cree\n",i);
		}
	}
	for(i=0;i<n;i++)
	{
		pthread_detach((pthread_t)thread+i);
	}
	sleep(5);
	//free(thread);
	
	return 0;
}
