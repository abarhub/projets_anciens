#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <netdb.h>

/* client UDP */

int main(int argc,char *argv[])
{
  int rbind,rsend;
  int sock;
  struct sockaddr_in addr;
  struct hostent *hp;
  
  if(argc!=4)
    {
      printf("Erruer: il faut 3 argument (nom_machine  port msg_envoye)\n");
      return 1;
    }
  
  // creation de la socket
  sock=socket(PF_INET,SOCK_DGRAM,0);
  if(sock==-1)
    {// erreur dans l'ouverture de la socket
      printf("Erreur: impossible d'ouvrir la socket\n");
      perror("socket");
      return 1;
    }
  else
    {
      addr.sin_family=AF_INET;
      addr.sin_port=htons(atoi(argv[2]));
      addr.sin_addr.s_addr=htonl(INADDR_ANY);
     
      // transformation du nom symbolique en nom physique
      hp=(struct hostent*)gethostbyname(argv[1]);
      memcpy(&addr.sin_addr.s_addr,hp->h_addr,hp->h_length);
      // envoi du message
      rsend=sendto(sock,(void*)argv[3],strlen(argv[3]),0,
	     (struct sockaddr*)&addr,sizeof(struct sockaddr_in));
      
      if(rsend==-1)
	{// erreur dans le sendto
	  printf("Erreur dans le sendto\n");
	  perror("sendto");
	  return 1;
	}
      else
	{
	  printf("Le sendto a reussi\n");
	  printf("sendto a envoyer %d caractere\n",rsend);
	}
    }
}
