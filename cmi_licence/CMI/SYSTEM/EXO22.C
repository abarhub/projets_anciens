#include<pthread.h>

typedef struct 
{
  pthread_mutex_t mutex_ecriture,mutex_lecture;
  pthread_cond_t condition_ecriture,condition_lecture;
} thread_rwlock;

int pthread_rwlock_init1(thread_rwlock *rwlock)
{
  pthread_mutex_init(&rwlock->mutex_ecriture,NULL);
  pthread_mutex_init(&rwlock->mutex_lecture,NULL);
  pthread_cond_init(&rwlock->condition_ecriture,NULL);
  pthread_cond_init(&rwlock->condition_lecture,NULL);
}

int pthread_rwlock_destroy1(thread_rwlock *rwlock)
{
  pthread_mutex_destroy(&rwlock->mutex_ecriture);
  pthread_mutex_destroy(&rwlock->mutex_lecture);
  pthread_cond_destroy(&rwlock->condition_ecriture);
  pthread_cond_destroy(&rwlock->condition_lecture);
}

int pthread_rdlock(thread_rwlock *rwlock)
{
  pthread_mutex_t mut;
  
  if(pthread_mutex_trylock(rwlock->mutex_ecriture)==EBUSY)
    {// on est en ecriture
      pthread_mutex_init(&mut,NULL);
      pthread_lock(&mut);
      pthread_rwlock_wait(rwlock->condition_ecriture,);
    }
  else
    {// on n'est pas en ecriture
      pthread_mutex_unlock(rwlock->mutex_ecriture);
      pthread_mutex_trylock(rwlock->mutex_lecture); //on bloc en lecture
      
    }
}

int pthread_wrlock(thread_rwlock *rwlock)
{
  
}

int pthread_unlock(thread_rwlock *rwlock)
{
  
}

//#define INIT()

/* test des fonctions */

#define LIMIT (5)
#define N (10)

char buf[N]={0};
pthread_rwlock_t verrou;

void *f1(void*arg)
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

void *f2(void*arg)
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
  pthread_t pth_lecture[2],pth_ecriture;
  int i;
  
  pthread_rwlock_init(&verrou,NULL);
  
  for(i=0;i<2;i++)
    {
      if(pthread_create(&pth_lecture[i],NULL,f1,NULL)!=0)
	{
	  printf("Erreur: impossible de creer un thread\n");
	}
    }
  
  if(pthread_create(&(pth_ecriture),NULL,f2,NULL)!=0)
	{
	  printf("Erreur: impossible de creer un thread\n");
	}
    
  for(i=0;i<2;i++)
    {
      pthread_join(pth_lecture[i],NULL);
    }
  pthread_join(pth_ecriture,NULL);
  
  pthread_rwlock_destroy(&verrou);
  
  printf("Fin de programme\n");
  
  return 0;
}
