#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<errno.h>

#define N (10)

#define CONDENCE(x,y) ((x)<<16+(y))
#define EXPLOSEX(v)  (v>>16)
#define EXPLOSEY(v)  (v&65535)

pthread_mutex_t mutex;
int *tab;

void quicksort_pth(int tmp)
{
  int h,b,milieu,temp;
  pthread_t pth1,pth2;
  int inf=EXPLOSEX(tmp),sup=EXPLOSEY(tmp);
  
  b=inf;
  h=sup;
  
  pthread_mutex_lock(&mutex);
  milieu=tab[(inf+sup)/2];
  pthread_mutex_unlock(&mutex);
  do {
    pthread_mutex_lock(&mutex);
    while(tab[b]<milieu) b++;
    while(milieu<tab[h]) h--;
    pthread_mutex_unlock(&mutex);
    if(b<=h)
      {
	pthread_mutex_lock(&mutex);
	temp=tab[b];
	tab[b]=tab[h];
	tab[h]=temp;
	pthread_mutex_unlock(&mutex);
	h--;b++;
      }
  } while(b<=h);
  
  if(inf<h) 
    pthread_create(&pth1,NULL,(void*(*)(void*))quicksort_pth,
		   (void*)CONDENCE(inf,h));
  //quicksort_pth(CONDENCE(inf,h));
  if(b<sup)
     pthread_create(&pth2,NULL,(void*(*)(void*))quicksort_pth,
		    (void*)CONDENCE(b,sup));
    //quicksort_pth(CONDENCE(b,sup));
  pthread_detach(pth1);
  pthread_detach(pth2);
}


void quicksort(int tab[],int inf,int sup)
{
  int h,b,milieu,temp;
  
  b=inf;
  h=sup;
  milieu=tab[(inf+sup)/2];
  do {
    while(tab[b]<milieu) b++;
    while(milieu<tab[h]) h--;
    if(b<=h)
      {
	temp=tab[b];
	tab[b]=tab[h];
	tab[h]=temp;
	h--;b++;
      }
  } while(b<=h);
  
  if(inf<h)
    quicksort(tab,inf,h);
  if(b<sup)
    quicksort(tab,b,sup);
}

void affiche(int tab[],int taille)
{
  int i;
  
  for(i=0;i<taille;i++)
    {
      printf("%d ",tab[i]);
    }
  printf("\n");
}

int main(int argc,char *argv[])
{
  int *tab,i;
  pthread_t pth;
  
  tab=(int*)calloc(N,sizeof(int));
  for(i=0;i<N;i++)
    {
      tab[i]=rand()%10;
    }
  
  printf("Tableau non trie:\n");
  affiche(tab,N);
  
  //quicksort_pth(tab,0,N-1);
  
  pthread_mutex_init(&mutex,NULL);
  pthread_create(&pth,NULL,(void*(*)(void*))quicksort_pth,
		 (void*)CONDENCE(0,N-1));
		 
  pthread_detach(pth);
  pthread_mutex_destroy(&mutex);
  sleep(10);
  printf("Tableau trie:\n");
  affiche(tab,N);
  
  return 0;
}
