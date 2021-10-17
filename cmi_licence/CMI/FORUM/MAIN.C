#include<pthread.h>

#define TAILLE_MAX (20)
#define TEXT_MAX (512)
#define PORT_SERVEUR (6000)

char text[TEXT_MAX],host[TAILLE_MAX],user[TAILLE_MAX],
	variable[TAILLE_MAX],valeur[TAILLE_MAX];
int port;

enum COMMANDE {Econnect,Eaccept,Esay,Emsg,Eset,Eerreur};

int port_serveur;

//sautr_blanc

//void remplit(char *entre,char )

int lecture(void)
{
	char c,buf[512],*p;
	int i=0;
	
	do {
		if(read(0,&c,sizeof(char))==0)
		{ // fin de fichier
			i++;
			break;
		}
		buf[i]=c;
		i++;
	} while (c!='\n');
	
	buf[i-1]='\0';
	
	if(strncasecmp(buf,"connect ",8)==0)
	{// commande CONNECT
		sscanf(buf+8,"%s %d",host,&port);
		return Econnect;
	}
	else if(strncasecmp(buf,"accept ",7)==0)
	{// commande ACCEPT
		sscanf(buf+7,"%d",&port);
		return Eaccept;
	}
	else if(strncasecmp(buf,"say ",4)==0)
	{// commande SAY
		//sscanf(buf+4,"%s",text);
		strcpy(text,buf+4);
		return Esay;
	}
	else if(strncasecmp(buf,"msg ",4)==0)
	{// commande MSG
		sscanf(buf+4,"%s",user);
		p=buf+4;
		while(isspace(*p)&& *p!='\0') p++;
		p+=strlen(user);
		while(isspace(*p)&& *p!='\0') p++;
		strcpy(text,p);
		return Emsg;
	}
	else if(strncasecmp(buf,"set ",4)==0)
	{// commande SET
		sscanf(buf+4,"%s %s",variable, valeur);
		return Eset;
	}
	return Eerreur;
}

void menu(void)
{
	enum COMMANDE cmd;
	int fd=-1;
	
	while(1)
	{
		cmd=lecture();
		switch(cmd)
		{
		case Econnect:
			printf("connect %s %d\n",host,port);
			if(fd!=-1) close(fd);
			fd=connect_client2(host,port);
			if(fd==0)
			break;
		case Eaccept:
			printf("accept %d\n",port);
			break;
		case Esay:
			printf("say\n%s\n",text);
			if(fd==-1) 
			{
				printf("Erreur: il n'y a pas de connection\n");
			}
			else
			{
				write(fd,text,strlen(text)+1);
			}
			break;
		case Emsg:
			printf("msg %s (%s)(%d)\n",user,text,fd);
			if(fd==-1) 
			{
				printf("Erreur: il n'y a pas de connection\n");
			}
			else
			{
				write(fd,text,strlen(text)+1);
			}
			break;
		case Eset:
			printf("set %s %s\n",variable,valeur);
			break;
		case Eerreur:
			printf("Erreur: commande inconnue\n");
			if(fd!=-1) close(fd);
			return;
			break;
		default:
			printf("Erreur: cmd incorrecte\n");
			if(fd!=-1) close(fd);
			return;
			break;
		}
	}
}

void *f_serveur(void *arg)
{
	printf("serveur\n");
	connect_serveur(port_serveur);
	pthread_exit(0);
}

void *f_menu(void *arg)
{
	printf("menu\n");
	menu();
	pthread_exit(0);
}

int main(int argc,char *argv[])
{
	pthread_t serveur,menu;
	if(argc==2)
	{
		port_serveur=atoi(argv[1]);
	}
	else
	{
		port_serveur=PORT_SERVEUR;
	}
	/*switch(fork())
	{
	case 0:
		connect_serveur(port_serveur);
		break;
	default:
		menu();
		break;
	}*/
	printf("creation\n");
	pthread_create(&serveur,NULL,f_serveur,NULL);
	pthread_create(&menu,NULL,f_menu,NULL);
	printf("destruction\n");
	pthread_join(menu,NULL);
	pthread_join(serveur,NULL);
	
	return 0;
}
