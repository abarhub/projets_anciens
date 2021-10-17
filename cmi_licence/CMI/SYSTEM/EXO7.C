#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>

//#define EXPLICITE

void fils(int entree,int premier)
{
  int tube[2],pid,i,a_fils=0;
  
  printf("%d ",premier);fflush(stdout);
  pid=-1;
  while(read(entree,&i,sizeof(int)))
    {
#ifdef EXPLICITE
      printf("fils %d lit %d\n",premier,i);
#endif
      if(i%premier!=0)
      {
	if(pid==-1)
	{ // il faut creer un processus fils
	  pipe(tube);
	  pid=fork();
	  switch(pid)
	  {
	  case -1:
	    printf("Erreur:impossible de creer un processus\n");
	    return;
	  case 0:
	    close(tube[1]);
	    fils(tube[0],i);
	    close(tube[0]);
	    exit(0);// le processus doit ce terminer
	  }
	  a_fils=1;
	}
	write(tube[1],&i,sizeof(int));
      }
    }
#ifdef EXPLICITE
  printf("fils %d a lut eof\n",premier);
#endif
  
  if(pid!=-1) 
    {// le processus a des fils -> fermeture du tube et attente
      close(tube[1]);
      wait(NULL);
    }
}

int main(int argc,char *argv[])
{
  int tube[2];
  int pid,i,n;
  
  if(argc!=2)
    {
      printf("Erreur:Il faut un parametre\n");
      return 1;
    }
  
  n=atoi(argv[1]);
  if(n<2)
    {
      printf("Erreur: le parametre doit etre un nombre >=2\n");
      return 1;
    }
  
  pipe(tube);
  
  pid=fork(); // creation du 1er fils ou premier=2
  switch(pid)
    {
    case -1:
      printf("Erreur: impossible de creer un processus\n");
      return 1;
    case 0:
      close(tube[1]);
      fils(tube[0],2);
      close(tube[0]);
      return 0;
    default:
      close(tube[0]);
    }

  // parcourt des entiers et envoi au 1er fils
  for(i=2;i<=n;i++)
    {
#ifdef EXPLICITE
      printf("pere i=%d\n",i);
#endif
      write(tube[1],&i,sizeof(int));
    }
  close(tube[1]);
  wait(NULL); // attente de la fin du 1er fils
  printf("\nFin du programme\n");
  
  return 0;
}
