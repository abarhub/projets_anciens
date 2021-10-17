#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>

int main()
{
  int rbind;
  int sock;
  struct sockaddr_in *addr;
  
  // creation de la socket
  sock=socket(PF_INET,SOCK_DGRAM,0);
  if(sock==-1)
    {// erreur
      printf("Erreur: impossible de creer le socket\n");
      return 1;
    }
  else
    {
      addr=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
      if(addr==NULL)
	{
	  printf("Erreur: plus de memoire\n");
	  return 1;
	}
      addr->sin_family=AF_INET;
      addr->sin_port=htons(6000); // port 6000
      addr->sin_addr.s_addr=htonl(INADDR_ANY);
      
      // fourni un nom a la socket
      rbind=bind(sock,(struct sockaddr*)addr,sizeof(struct sockaddr_in));
      if(rbind==-1)
	{
	  printf("Erreur: echec du bind\n");
	  perror("bind");
	  return 1;
	}
      
      printf("socket cree. bind renvoie %d\n",rbind);
    }
}
