#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
//#include<semaphore.h>
#include<errno.h>

#define N (5)

/* nouvelle semaphore */

typedef struct sem2_t
{
  int nb_max,nb;
  pthread_mutex_t *tab;
  pthread_mutex_t mutex;
} sem2_t;

int sem2_init(sem2_t *s,int nb)
{
  int i;
  
  if(s==NULL)
    {
      printf("Erreur:semaphore nulle\n");
      return -1;
    }
  
  s->nb_max=nb;
  s->nb=0;
  s->tab=(pthread_mutex_t*)calloc(sizeof(pthread_mutex_t),nb);
  for(i=0;i<nb;i++)
    {
      pthread_mutex_init(&(s->tab[i]),NULL);
    }
  
  pthread_mutex_init(&(s->mutex),NULL);
		     
  return 0;
}

int sem2_destroy(sem2_t *s)
{
  int i;
  
  if(s==NULL)
    {
      printf("Erreur:semaphore nulle\n");
      return -1;
    }
  
  // unlock les mutex
  for(i=0;i<s->nb_max;i++)
    {
      if(pthread_mutex_trylock(&(s->tab[i]))!=EBUSY)
	{
	  pthread_mutex_unlock(&(s->tab[i]));
	}
    }
  // detruit les mutex
  for(i=0;i<s->nb_max;i++)
    {
      pthread_mutex_destroy(&(s->tab[i]));
    }
  free(s->tab);
  s->tab=NULL;
  s->nb_max=s->nb=0;
  
  if(pthread_mutex_trylock(&(s->mutex))!=EBUSY)
    {// verifie que mutex est unlocke
      pthread_mutex_unlock(&(s->mutex));
    }
  pthread_mutex_destroy(&(s->mutex));
			
  return 0;
}

int sem2_wait(sem2_t *s)
{
  int i;
    
  if(s==NULL)
    {
      printf("Erreur:semaphore nulle\n");
      return -1;
    }
  
  //pthread_mutex_lock(&(s->mutex));
  i=0;
  while(pthread_mutex_trylock(&(s->tab[i]))==EBUSY)
    {
      i=(i+1)%(s->nb_max);
    }
  s->nb++;
  //pthread_mutex_unlock(&(s->mutex));
  
  return 0;
}

int sem2_post(sem2_t *s)
{
  int i;
    
  if(s==NULL)
    {
      printf("Erreur:semaphore nulle\n");
      return 1;
    }
  
  //pthread_mutex_lock(&(s->mutex));
  // recherche d'un mutex deja locke
  for(i=0;i<s->nb_max;i++)
    {
      if(pthread_mutex_trylock(&(s->tab[i]))!=EBUSY)
	{// mutex non deja locke
	  pthread_mutex_unlock(&(s->tab[i]));
	}
      else
	{// mutex deja locke
	  pthread_mutex_unlock(&(s->tab[i]));
	  break;
	}
    }
  if(i==s->nb_max)
    {
      printf("Erreur:aucune semaphore locke");
      pthread_mutex_unlock(&(s->mutex));
      return -1;
    }
  s->nb--;
  //pthread_mutex_unlock(&(s->mutex));
  
  return 0;
}

int sem2_getvalue(sem2_t *s,int *val)
{
  if(s==NULL)
    {
      printf("Erreur:semaphore nulle\n");
      return -1;
    }
  
  pthread_mutex_lock(&(s->mutex));
  *val=s->nb;
  pthread_mutex_unlock(&(s->mutex));
  
  return 0;
}

/* test de la semaphore */

sem2_t sema;

void *f(void* arg)
{
  sem2_wait(&sema);
  printf("thread no %d\n",(int)arg);
  sleep(3);
  printf("fin du thread no %d\n",(int)arg);
  sem2_post(&sema);
  pthread_exit(NULL);
  return NULL;
}

int main(int argc,char *argv[])
{
  int i,n;
  pthread_t thread[N];
  
  printf("Debut\n");
  n=N;
  sem2_init(&sema,3);
  printf("D2\n");
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
	  printf("thread %d cree\n",i);
	}
    }
  printf("D1\n");
  for(i=0;i<n;i++)
    {
      //pthread_join((pthread_t)thread+i,NULL);
      pthread_join(thread[i],NULL);
    }
  //sleep(25);
  sem2_destroy(&sema);
  printf("Fin\n");
  
  return 0;
}
