#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <netdb.h>
#include<errno.h>
#include<pthread.h>

#define MAX_CONNECT (5)

void *fonct(void *arg)
{
	int fd=*((int*)arg),len;
	char buf[512];
	
	printf("debut d'une nouvelle connection\n");
	while(len=read(fd,buf,sizeof(buf))>0)
	{
		printf("serveur: buf=%s\n",buf);
	}
	printf("Fin d'une connection\n");
}

connect_serveur(int port)
{
  int rbind;
  int sock,i;
  struct sockaddr_in addr;
  struct hostent *hp;
  pid_t pid;
  int res,len,fd;
  char buf[500]={0};
  pthread_t th/*[MAX_CONNECT]={0}*/;
  
  switch(fork())
    {
    case -1:
      printf("Erreur dans le fork\n");
      break;
    case 0:
      // detachement du processus par rapport au terminal
      setsid();
      sock=socket(PF_INET,SOCK_STREAM /* TCP */,0);
      if(sock==-1)
	{// erreur de creation de socket
	  printf("Erreur: impossible de creer un socket\n");
	  return 1;
	}
      else
	{
	  addr.sin_family=AF_INET;
	  addr.sin_port=htons(port);
	  addr.sin_addr.s_addr=htonl(INADDR_ANY);
	  
	  // bind
	  rbind=bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	  if(rbind==-1)
	    {
	      printf("Erreur dans le bind (%d)\n",rbind);
	      switch(errno)
		{
		case EACCES:
		  printf("EACCES");
		  break;
		case EADDRINUSE:
		  printf("EADDRINUSE");
		  break;
		case EADDRNOTAVAIL :
		  printf("EADDRNOTAVAIL");
		  break;
		case EBADF:
		  printf("EBADF");
		  break;
		case EINVAL:
		  printf("EINVAL");
		  break;
		case ENOSR:
		  printf("ENOSR");
		  break;
		case ENOTSOCK:
		  printf("ENOTSOCK");
		  break;
		case EIO:
		  printf("EIO");
		  break;
		case EISDIR:
		  printf("EISDIR");
		  break;
		case ELOOP:
		  printf("ELOOP");
		  break;
		case ENOENT:
		  printf("ENOENT");
		  break;
		case ENOTDIR:
		  printf("ENOTDIR");
		  break;
		case EROFS:
		  printf("EROFS");
		  break;
		default:
		  printf("Autre erreur\n");
		  break;
		}
	      return -1;
	    }
	  else
	    printf("socket cree. bind renvoie %d\n",rbind);
	  
	  res=listen(sock,MAX_CONNECT);
	  if(res!=0)
	    {
	      printf("Erreur dans le listen(%d)\n",res);
	      switch(errno)
		{
		case EBADF:
		  printf("EBADF");
		  break;
		case ENOTSOCK:
		  printf("ENOTSOCK");
		  break;
		case EOPNOTSUPP:
		  printf("EOPNOTSUPP");
		  break;
		default:
		  printf("Autre erreurs(%d)\n",errno);
		  break;
		}
	      return -1;
	    }
	  else
	    {
	      printf("listen reussi\n");
	    }
	  len=sizeof(addr);
	  if((fd=accept(sock,(struct sockaddr*)&addr,&len))!=-1)
	    {
	      do {
	      //for(i=0;i<)
	      pthread_create(&th,NULL,fonct,&fd);
	      pthread_detach(&th);
		/*switch(fork())
		  {
		  case -1:
		    printf("Erreur dans le fork\n");
		    break;
		  case 0:
		    printf("debut d'une nouvelle connection\n");
		    while(len=read(fd,buf,sizeof(buf))>0)
		      {
			printf("serveur: buf=%s\n",buf);
		      }
		    printf("Fin d'une connection\n");
		    return 0;
		  default: 
		    break;
		  }*/
	      } while((fd=accept(sock,(struct sockaddr*)&addr,&len))!=-1);
	    }
	  else
	    {
	      printf("Erreur pour accepter un client\n");
	      perror("accept");
	    }
	  printf("Fin du serveur\n");
	}
      break;
    default:
      // sleep pour attendre que le fils se soit detache
      sleep(1);
      // pas de wait
      break;
      }
}


int connect_client(char *host,int port)
{
  int rbind;
  int sock;
  struct sockaddr_in addr;
  struct hostent *hp;
  pid_t pid;
  int res,len,fd;
  char buf[500]={0};
  
  sock=socket(PF_INET,SOCK_STREAM/* TCP */,0);
  if(sock==-1)
    {
      printf("Erreur\n");
      return 1;
    }
  else
    {
      addr.sin_family=AF_INET;
      addr.sin_port=htons(port);
      addr.sin_addr.s_addr=htonl(INADDR_ANY);
      
      hp=(struct hostent*)gethostbyname(host);
      //perror("gethostbyname");
      memcpy(&addr.sin_addr.s_addr,hp->h_addr,hp->h_length);
      
      fd=connect(sock,(struct sockaddr*)&addr,sizeof(addr));
      if(fd==-1)
	{
	  printf("Erreur: impossible de se connecter\n");
	  perror("connect");
	  return 1;
	}
      else
	{
	  printf("Connection reussi\nEntrez un texte:\n");
	  while((len=read(0,buf,sizeof(buf)))>0)
	    {
	      write(fd,buf,len);
	    }
	}
      printf("Fin du client\n");
      
    }
}

int connect_client2(char *host,int port)
{
  int rbind;
  int sock;
  struct sockaddr_in addr;
  struct hostent *hp;
  pid_t pid;
  int res,len,fd;
  char buf[500]={0};
  
  sock=socket(PF_INET,SOCK_STREAM/* TCP */,0);
  if(sock==-1)
    {
      printf("Erreur\n");
      return -1;
    }
  else
    {
      addr.sin_family=AF_INET;
      addr.sin_port=htons(port);
      addr.sin_addr.s_addr=htonl(INADDR_ANY);
      
      hp=(struct hostent*)gethostbyname(host);
      //perror("gethostbyname");
      memcpy(&addr.sin_addr.s_addr,hp->h_addr,hp->h_length);
      
      fd=connect(sock,(struct sockaddr*)&addr,sizeof(addr));
      if(fd==-1)
	{
	  printf("Erreur: impossible de se connecter\n");
	  perror("connect");
	  return -1;
	}
      else
	{
	  printf("Connection reussi\nEntrez un texte:(%d)\n",fd);
	  /*while((len=read(0,buf,sizeof(buf)))>0)
	    {
	      write(fd,buf,len);
	    }*/
	    return sock;
	}
    }
    return -1;
}
