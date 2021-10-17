#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<errno.h>

pthread_mutex_t mutex;
int *nombre;

int lire_tab(int no)
{
  int val;
  pthread_mutex_lock(&mutex);
  val=nombre[no];
  pthread_mutex_unlock(&mutex);
  return val;
}

void ecrire_tab(int no,int val)
{
  pthread_mutex_lock(&mutex);
  nombre[no]=val;
  pthread_mutex_unlock(&mutex);
}

void *fils(int no)
{
  int premier,no_nombre=no,tmp,a_fils=0;
  pthread_t pth;

  //printf("thread %d cree\n",no);
  premier=lire_tab(no_nombre);
  printf("%d ",premier);fflush(stdout);
  while(1)
    {
      tmp=lire_tab(no_nombre);
      
      if(tmp==-1)
	{
	  if(a_fils)
	    pthread_join(pth,NULL);
	  pthread_exit(0);
	}
      
      if(tmp%premier!=0)
	{
	  ecrire_tab(no_nombre+1,tmp);
	  if(!a_fils)
	    {
	      a_fils=1;
	      pthread_create(&pth,NULL,(void*(*)(void*))fils,
			     (void*)(no_nombre+1));
	    }
	}
    }
  return NULL;
}

int main(int argc,char *argv[])
{
  int n,i,j;
  pthread_t pth;
  
  if(argc!=2)
    {
      printf("Erreur: Il faut un entier >=2\n");
      return 1;
    }
  
  n=atoi(argv[1]);
  if(n<2)
    {
      printf("Erreur: Il faut un entier >=2\n");
      return 1;
    }
  
  nombre=(int*)calloc(n,sizeof(int));
  pthread_mutex_init(&mutex,NULL);
  //printf("A\n");
  //nombre[0]=2;
  ecrire_tab(0,2);
  pthread_create(&pth,NULL,(void*(*)(void*))fils,(void*)0);
  //printf("B\n");
  for(i=2;i<n;i++)
    {
      ecrire_tab(0,i);
      // temporisation
      //sleep(1);
      //for(j=0;j<10000000;j++);
    }
  //printf("C\n");
  //pthread_detach(pth);
  pthread_join(pth,NULL);
  
  free(nombre);
  pthread_mutex_destroy(&mutex);
  
  printf("\nfin de traitement\n");
  
  return 0;
}
