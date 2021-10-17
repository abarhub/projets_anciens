#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define N (14)

pthread_mutex_t *mut[N];

int tirage()
{
  return (random()%3)+1;
}

void *f(void *arg)
{
  int i,no=(int)arg;
  
  for(i=0;i<10;i++){
  printf("%d pense\n",no+1);
  sleep(tirage());

  pthread_mutex_lock(mut[(no+1)%N]);
  printf("%d prend la fourchette droite:\t   %d >\n",no+1,(no+1)%N+1);
  pthread_mutex_lock(mut[no]);
  printf("%d prend la fourchette gauche:\t   %d >\n",no+1,no+1);
  printf("%d mange\n",no+1);
  sleep(tirage());
  pthread_mutex_unlock(mut[no]);
  printf("%d pose la fourchette gauche:\t > %d\n",no+1,no+1);
  pthread_mutex_unlock(mut[(no+1)%N]);
  printf("%d pose la fourchette droite:\t > %d\n",no+1,(no+1)%N+1);
  }
  printf("%d sort de table\n",no+1);
  
  return NULL;
}

int main()
{
  int i;
  pthread_t th[N];
  
  for(i=0;i<N;i++)
    {
      mut[i]=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
      pthread_mutex_init(mut[i],NULL);
      printf("il y a %d fourchettes a table\n",i+1);
    }

  for(i=0;i<N;i++)
    {
      if(pthread_create(th+i,NULL,f,(void *)i) != 0)
	{
	  printf("impossible de creer le thread %d\n",i);
	  exit(0);
	}
      else
	{
	  printf("le philosophe %d prend place\n",i+1);
	}
    }

  for(i=0;i<N;i++)
    {
      pthread_detach((pthread_t)th+i);
    }
  
  sleep(260);
  
  for(i=0;i<N;i++)
    {
      pthread_mutex_destroy(mut[i]);
    }
  
}
