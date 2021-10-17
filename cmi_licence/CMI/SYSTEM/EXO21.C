#include<pthread.h>

#define LIMIT (20)
#define N (10)

char buf[N]={0};
pthread_rwlock_t verrou;

void *thread_lecture(void *arg)
{
	int i;
	
	for(i=0;i<LIMIT;i++)
	{
		sleep(1);
		pthread_rwlock_rdlock(&verrou);
		printf("lecture de %s (thread no %d)\n",buf,pthread_self());
		pthread_rwlock_unlock(&verrou);
	}
}

void *thread_ecriture(void *arg)
{
	int i;
	
	for(i=0;i<LIMIT;i++)
	{
		sleep(1);
		pthread_rwlock_wrlock(&verrou);
		sprintf(buf,"%d",i);
		printf("ecriture de %d\n",i);
		pthread_rwlock_unlock(&verrou);
	}
}

int main()
{
	pthread_t wri,rea[5];
	int i;
	
	pthread_rwlock_init(&verrou,NULL);
	
	// creation du thread d'ecriture
	if(pthread_create(&wri,NULL,thread_ecriture,NULL)!=0)
	{
		printf("Erreur:impossible de creer le thread d'ecriture\n");
	}
	
	// creation des threads de lectures
	for(i=0;i<5;i++)
	{
		if(pthread_create(rea+i,NULL,thread_lecture,NULL)!=0)
		{
			printf("Erreur:impossible de creer le thread de lecture %d\n",i);
		}
	}
		
	// nettoiment des threads de lecture
	for(i=0;i<5;i++)
	{
		pthread_join(rea[i],NULL);
	}
	
	// nettoiment des threads d'ecriture
	pthread_join(wri,NULL);
	
	pthread_rwlock_destroy(&verrou);
	printf("Il y a eu les %d ecritures\nFin du programme\n",LIMIT);
	
	return 0;
}
