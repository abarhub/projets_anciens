#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>

void fils() 
{
  char *argv[]={"ls","-l",NULL};
  if(execvp(argv[0],argv)==-1)
    printf("impossible d'executer %s\n",argv[0]);
}

void pere()
{
  char *argv[]={"grep","22",NULL};
  if(execvp(argv[0],argv)==-1)
    printf("impossible d'executer %s\n",argv[0]);
}

int t[2];

int main()
{
  pipe(t);
  
  switch(fork())
    {
    case -1: perror("erreur\n"); break;
    case 0:
      close(t[0]);
      close(1);
      dup(t[1]);
      close(t[1]);
      close(0);
      fils();
      exit(0);
      break;

    default:
      close(t[1]);
      close(0);
      dup(t[0]);
      close(t[0]);
      pere();
      wait(NULL);
      
      break;
    }
  
  return 0;
}
