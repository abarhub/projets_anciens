#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define N (5)

pthread_mutex_t* mutex;

void *f(void* arg)
{
	int i;
	
	//pthread_mutex_lock(mutex);
	printf("thread no %d\n",pthread_self());
	for(i=0;i<3;i++)
	{
		printf("thread %d =%d\n",pthread_self(),i);
		sleep(1);
	}
	printf("fin du thread no %d\n",pthread_self());
	//pthread_mutex_unlock(mutex);
	return NULL;
}

int main(int argc,char *argv[])
{
	int i,n;
	pthread_t thread[N];
	
	mutex=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex,NULL);
	printf("%d %p\n",mutex,mutex);
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
	sleep(N*3+1);
	//free(thread);
	pthread_mutex_destroy(mutex);
	
	return 0;
}
