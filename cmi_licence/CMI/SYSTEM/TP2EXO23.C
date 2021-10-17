#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define N (4)

pthread_mutex_t* mutex[N];

int tirage()
{
	return (random()%9)+1;
}

void *f(void* arg)
{
	int i,no=(int)arg;
	int t_penser,t_manger;
	
	for(i=0;i<3;i++)
	{	
		
		// penser
		t_penser=tirage();
		printf("philosophe %d pense\n",no);
		sleep(t_penser);
		
		// manger
		pthread_mutex_lock(mutex[(no+1)%N]);
		printf("fouchettes droite %d pour philo %d\n",(no+1)%N,no);
		pthread_mutex_lock(mutex[no]);
		printf("fouchettes gauche %d pour philo %d\n",no,no);
		t_manger=tirage();
		printf("philosophe %d mange\n",no);
		sleep(t_manger);
		pthread_mutex_unlock(mutex[no]);
		printf("fouchettes gauche %d rendue par philo %d\n",no,no);
		pthread_mutex_unlock(mutex[(no+1)%N]);
		printf("fouchettes droite %d rendue par philo %d\n",(no+1)%N,no);
	}
	printf("Mort du philosophe %d\n",no);
	return NULL;
}

int main(int argc,char *argv[])
{
	int i,n;
	pthread_t thread[N];
	
	for(i=0;i<n;i++)
	{
		mutex[i]=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(mutex[i],NULL);
		printf("fouchette %d\n",i);
	}
	n=N;
	for(i=0;i<n;i++)
	{
		if(pthread_create(thread+i,NULL,f,(void*)i))
		{
			printf("Erreur: Impossible de cree de thread"
				" no %d\n",i);
			n=i;
			break;
		}
		else
		{
			printf("philosophe %d:\n",i);
		}
	}
	for(i=0;i<n;i++)
	{
		//if(
		pthread_detach((pthread_t)thread+i);
		/*)
		{
			printf("Erreur dans le join %d",i);
			return 0;
		}*/
	}
	sleep(N*N+N);
	for(i=0;i<N;i++)
	{
		pthread_mutex_destroy(mutex[i]);
	}
	
	return 0;
}
