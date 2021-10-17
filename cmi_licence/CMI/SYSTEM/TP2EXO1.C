#include<stdio.h>

int main()
{
  int rbind;
  int sock;
  struct sockaddr_in addr;
  
  sock=socket(PF_INET,SOCK_DGRAM,0);
  if(sock==-1)
    {
      printf("Erreur\n");
      return 1;
    }
  else
    {
      addr.sin_family=AF_INET;
      addr.sin_port=htons(6000);
      addr.sin_addr.s_addr=htonl(INADDR_ANY);
      
      rbind=bind(sock,&addr,sizeof(addr));
      printf("socket cree. bind renvoie %d\n",rbind);
    }
}
