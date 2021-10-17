#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<fcntl.h>
#include<sys/types.h>

#define N (5)

sem_t sema;

void *f(void* arg)
{
  sem_wait(&sema);
  printf("thread no %d\n",pthread_self());
  sleep(3);
  printf("fin du thread no %d\n",pthread_self());
  sem_post(&sema);
  pthread_exit(NULL);
  return NULL;
}

int main(int argc,char *argv[])
{
	int i,n;
	pthread_t thread[N];
	
	n=N;
	sem_init(&sema,0,3);
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
	  //pthread_join((pthread_t)thread+i,NULL);
	  pthread_join(thread[i],NULL);
	}
	//sleep(15);
	sem_destroy(&sema);
	
	return 0;
}
