#include<pthread.h>

#define N (20)

pthread_mutex_t mutex_thread;

void *f(void *c)
{
  // pour que les signals ne soient pas envoyer dans un interval tres court
  // je met un mutex et un sleep (comme cela, les signaux seraont 
  // separe de 2 secondes, en theorie)
  pthread_mutex_lock(&mutex_thread);
  
  sleep(2);
  
  printf("fin du processus %d\n",pthread_self());
  printf("Le fils %d envoi le signal\n",pthread_self());
  
  pthread_cond_signal(c); // envoi du signal
  
  pthread_mutex_unlock(&mutex_thread);
  
  return;
}

int main()
{
  int i;
  pthread_mutex_t mut;
  pthread_cond_t condition;
  pthread_t threads[N];
  
  pthread_mutex_init(&mut,NULL);
  pthread_mutex_init(&mutex_thread,NULL);
  pthread_cond_init(&condition,NULL);
  
  // creation des fils
  for(i=0;i<N;i++)
    {
      if(pthread_create(threads+i,NULL,f,(void*)&condition)!=0)
	{
	  printf("Erreur: impossible de creer le thread no %d\n",i);
	  return 1;
	}
    }
  
  // attente de la mort de tous les fils dans n'importe quel ordre
  for(i=0;i<N;i++)
    {
      pthread_mutex_lock(&mut);
      // attente d'un signal
      pthread_cond_wait(&condition,&mut);
      pthread_mutex_unlock(&mut);
      printf("Le processus pere a recu le signal de la mort d'un fils\n");
    }
  
  printf("Tous les threads ont envoye leur signal\n");
  
  pthread_cond_destroy(&condition);
  pthread_mutex_destroy(&mut);
  pthread_mutex_destroy(&mutex_thread);
  
  printf("Fin du programme\n");
  
  return 0;
}
