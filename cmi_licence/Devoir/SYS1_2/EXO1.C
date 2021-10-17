#include<stdlib.h>
#include<stdio.h>
#ifdef __BORLANDC__
#include<conio.h>
#include<io.h>
#endif
#include<fcntl.h>
#include<string.h>
#include<ctype.h>

/*
devoir 2 system
exercice 1
auteur(TP3): Schudel Edwar
	Barret Alain

Ce programme sort des rapport de
 consommation telephonique

*/

/* les noms des 3 autres fichiers */
#define NOM_FICHIER "fiche1.dat"
#define ANNUAIRE "annuaire.dat"
#define TAUX "taux.dat"

#define MAX (25)
#define MAX_EMAIL (50)

/* le taux par defaut pour 1 unite est de 1 (pas tres realiste) */
int taux[26]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
double prix_unite=0.65;/* prix par defaut de l'unite */
double tva=20.6/100;/* taux par defaut de la tva */

/* liste des appels pour un utilisateur */
struct appels
{
   short jour,mois; /* date de l'appel */
   char ratio; /* taux de l'appel */
   short heures,minutes,secondes; /* duree en h,m,s de l'appel */
   long duree; /* duree de l'appel en secondes */
   double prix; /* prix de l'appel */
   struct appels *suivant; /* appel suivant */
};

/* liste des utilisateurs */
struct utilisateur
{
   int no_utilisateur; /* numero de poste telephonique */
   char nom[MAX]; /* prenom nom de l'utilisateur */
   char email[MAX_EMAIL]; /* email de l'utilisateur */
   struct appels *liste; /* liste d'appel de l'utilisateur */
   long duree_totale; /* duree totale des appels en s */
   double prix_total; /* prix total des appels en Francs */
   struct utilisateur *suivant; /* utilisateur suivant */
};

/* retourne 1 si la date de a1 est strictement 
inferieur a a2 et 0 sinon */
int inferieur(const struct appels *a1,const struct appels *a2)
{
	return (a1->mois<a2->mois)||
		( (a1->mois==a2->mois)&&(a1->jour<a2->jour) );
}

/* retourne le prix avec la TVA et en converissant les unites
d'appels en francs */
double taxe(double nb_unite)
{
	return nb_unite*prix_unite*(1.0+tva);
}

/* ajoute un appel a la liste des utilisateurs
si l'utilisateur n'existe pas il est cree */
void ajoute_appel(struct utilisateur **liste_utilisateurs,
			int no_utilisateur,
			int jour,int mois,char ratio,
			short heures,short minutes,short secondes)
{
  int i,j;
  struct utilisateur *p1,*p2;
  struct appels *ap1,*ap2,*ap3;
  
  /* recherche de l'utilisateur */
  p1=*liste_utilisateurs;
  while(p1!=NULL)
  {
  	if(p1->no_utilisateur==no_utilisateur)
  		break;
  	p1=p1->suivant;
  }
  
  if(p1==NULL)
  {/* utilisateur non encore creer */
  	p2=(struct utilisateur *)malloc(sizeof(struct utilisateur));
  	p2->no_utilisateur=no_utilisateur;
  	(p2->nom)[0]=0;
  	p2->liste=NULL;
  	p2->duree_totale=0;
  	p2->prix_total=0.0;
  	p2->suivant=NULL;
  	if(*liste_utilisateurs==NULL)
  	{/* aucun utilisateur */
  		*liste_utilisateurs=p2;
  	}
  	else
  	{/* Il y a des utilisateur */
  		p1=*liste_utilisateurs;
  		while(p1->suivant!=NULL)
  		{
  			p1=p1->suivant;
  		}
  		p1->suivant=p2;
  	}
  	p1=p2;
  }
  
  /* ajout de l'appel a l'utilisateur pointe par p1 */
  
  /* creation du noeud correspondant a l'appel */
  ap1=(struct appels *)malloc(sizeof(struct appels));
  ap1->jour=jour;ap1->mois=mois;
  ap1->ratio=ratio;
  ap1->heures=heures;ap1->minutes=minutes;
  ap1->secondes=secondes;
  ap1->duree=ap1->secondes+ap1->minutes*60+ap1->heures*60*60;
  ap1->prix=taxe((double)ap1->duree/taux[ap1->ratio-'A']);
  ap1->suivant=NULL;
  
  /* ajout du noeud */
  if(p1->liste==NULL)
  {/* l'utilisateur n'a jamais eut d'appel */
  	p1->liste=ap1;
  }
  else
  {/* l'utilisateur a deja eut des appels */
  	/* recherche du dernier noeud */
  	ap2=p1->liste;
  	if(!inferieur(ap2,ap1))
  	{/* ap1 va en premiere position */
  		p1->liste=ap1;
  		ap1->suivant=ap2;
  	}
  	else
  	{/* ajout de l'appel a la bonne place(trie)
  	 suivant la date */
  		ap3=ap2;ap2=ap2->suivant;
  		while( (ap2!=NULL)&& (inferieur(ap2,ap1)) )
  		{
  			
  			ap2=ap2->suivant;
  			ap3=ap3->suivant;
  		}
  		ap3->suivant=ap1;
  		ap1->suivant=ap2;
  	}
  	
  }
  
  /* mise a jour de la duree et du prix total */
  p1->prix_total+=ap1->prix;
  p1->duree_totale+=ap1->duree;
}

/* effacement de tous les utilisateurs et de leurs appels */
void efface_utilisateurs(struct utilisateur *liste_utilisateurs)
{
	struct utilisateur *u1,*u2;
	struct appels *a1,*a2;
	
	u1=liste_utilisateurs;
	while(u1!=NULL)
	{
		u2=u1->suivant;
		a1=u1->liste;
		while(a1!=NULL)
		{
			a2=a1->suivant;
			free(a1);
			a1=a2;
		}
		
		free(u1);
		u1=u2;
	}
}

/* affiche tous les utilisateurs et leurs consommations */
void affichage(struct utilisateur *liste_utilisateurs)
{
	struct utilisateur *u1,*u2;
	struct appels *a1,*a2;
	
	printf("Affichage des utilisateurs:\n");
	u1=liste_utilisateurs;
	while(u1!=NULL)
	{
		if((u1->nom)[0]==0)
		{
			printf("Pas de nom,");
		}
		else
			printf("%s,",u1->nom);
		printf(" %d:\n",u1->no_utilisateur);
		printf("total %ld secondes, %lf Fr\n",
			u1->duree_totale,u1->prix_total);
		a1=u1->liste;
		while(a1!=NULL)
		{
			printf("%d/%d\t",a1->jour,a1->mois);
			printf("taux:%c\t",a1->ratio);
			printf("%d:%d:%d ",a1->heures,
				a1->minutes,a1->secondes);
			printf("prix: %lf\tduree: %ld",
				a1->prix,a1->duree);
			printf("\n");
			
			a1=a1->suivant;
		}
		
		u1=u1->suivant;
	}
}

/* converti la chaine en un nombre qui sera retourne 
et deplace *p vers ce qui suit ce qui a ete converti */
int convint(char **p)
{
  int resultat=0,j;
  char *p1,*p2;
  
  while(isdigit(**p))
  {
  	resultat=10*resultat+(**p)-'0';
  	(*p)++;
  }
  
  return resultat;
}

/* deplace le pointeur *p pour sauter les espaces */
void evite_espace(char **p)
{
  while(isspace(**p))
     (*p)++;
}

/* annalyse le fichier NOM_FICHIER
 retourne la liste des utilisateurs et remplit le tableau taux */
struct utilisateur *annalyse(void)
{
  int i,j,val,jour,mois,heures,minutes,secondes,d2,no_util;
  FILE *fannu,*ftau,*file;
  char buf[500],*p,c,s1[30],s2[30],s3[30];
  struct utilisateur *liste_utilisateurs=NULL,*u1,*u2;
  
  
  ftau=fopen(TAUX,"r");
  
  while(fscanf(ftau,"%c %d\n",&c,&val)!=-1)
  {
  	if((c>='A')&&(c<='Z'))
  	{
  		taux[c-'A']=val;
  	}
  }
  fclose(ftau);
  
#ifdef DEBUGAGE  
  /* affichage du taux */
  for(i=0;i<26;i++)
  {
  	if(taux[i]!=0)
  	   printf("%c=%d\n",i+'A',taux[i]);
  }
#endif
  
  file=fopen(NOM_FICHIER,"r");
  while(fgets(buf,sizeof(buf),file)!=NULL)
  {
  	/* pour enlever le \n a la fin */
  	if( (buf[0]!=0)&&(buf[strlen(buf)-1]=='\n') )
  	   buf[strlen(buf)-1]=0;
  	
  	p=buf;
  	evite_espace(&p);
  	/* lit le jour */
  	jour=convint(&p);
  	evite_espace(&p);
  	
  	if(*p=='/') p++;
  	evite_espace(&p);
  	
  	/* lit le mois */
  	mois=convint(&p);
  	evite_espace(&p);
  	
  	/* lit le taux */
  	if(isalpha(*p))
  	{
  	   c=*p;
  	   if(islower(c))
  	   	c+='A'-'a';
  	   p++;
  	}
  	else
  	{
  	   printf("Erreur: Il manque un taux\n");
  	}
  	
  	evite_espace(&p);
  	
  	/* lit la date */
  	i=convint(&p);
  	evite_espace(&p);
  	if(*p==':')
  	{
  		p++;
  		evite_espace(&p);
  		j=convint(&p);
  		evite_espace(&p);
  		if(*p==':')
  		{/* il y a les secondes, les minutes,
  		 et les heures */
  			p++;
  			secondes=convint(&p);
  			evite_espace(&p);
  			minutes=j;
  			heures=i;
  		}
  		else
  		{/* il n'y a que les secondes
  		 et les minutes */
  			secondes=j;
  			minutes=i;
  			heures=0;
  		}
  	}
  	else
  	{/* il ny a que les secondes */
  		secondes=i;
  		minutes=0;
  		heures=0;
  	}
  	
  	/* lit le numero de poste */
  	no_util=convint(&p);
  	
  	/* ajout de l'appel */
  	ajoute_appel(&liste_utilisateurs,no_util,jour,mois,
  			c,heures,minutes,secondes);
  	
  }
  fclose(file);
  
  
  /* lit l'annuaire  */
  fannu=fopen(ANNUAIRE,"r");
  while(fscanf(fannu,"%s %s %d %s\n",s1,s3,&val,s2)!=-1)
  {
  	if(liste_utilisateurs==NULL)
  	{/* la liste est vide */
  		liste_utilisateurs=(struct utilisateur *)
  				malloc(sizeof(struct utilisateur));
  		liste_utilisateurs->suivant=NULL;
  		liste_utilisateurs->liste=NULL;
  		liste_utilisateurs->no_utilisateur=val;
  		strcpy(liste_utilisateurs->nom,s1);
  		strcat(liste_utilisateurs->nom," ");
  		strcat(liste_utilisateurs->nom,s3);
  		strcpy(liste_utilisateurs->email,s2);
  		liste_utilisateurs->duree_totale=0;
  		liste_utilisateurs->prix_total=0.0;
  	}
  	else
  	{/* la liste n'est pas vide */
  		u1=liste_utilisateurs;
  		while((u1->suivant!=NULL)&&
  			(u1->no_utilisateur!=val))
  		   u1=u1->suivant;
  		
  		if(u1->no_utilisateur!=val)
  		{/* l'utilisateur n'est pas dans la liste */
  			u1->suivant=(struct utilisateur *)
  				malloc(sizeof(struct utilisateur));
  			u1=u1->suivant;
  			u1->suivant=NULL;
  			u1->liste=NULL;
  			u1->no_utilisateur=val;
  			u1->duree_totale=0;
  			u1->prix_total=0.0;
  		}
  		strcpy(u1->nom,s1);
  		strcat(u1->nom," ");
  		strcat(u1->nom,s3);
  		strcpy(u1->email,s2);
  	}
  }
  fclose(fannu);
  
  return liste_utilisateurs;
}

/* affichage des appels de l'utilisateur util
   et augmente le tableau total_categorie
    qui represente le prix total de chaque taux */
void affiche_utilisateur(struct utilisateur *util,
			int *total_categorie)
{
	struct appels *a1,*a2;
	int nb,total_cate[26],i,j;
	
	memset(&total_cate,0,sizeof(total_cate));
	
	printf(" * %s ( %d %s ):\n",util->nom,
		util->no_utilisateur,util->email);
	printf("duree totale: %5ld sec.\tprix total:"
		" %7.2lf Francs\n",util->duree_totale,
		util->prix_total);
	a1=util->liste;
	while(a1!=NULL)
	{
		nb=a1->duree/taux[a1->ratio-'A'];
		printf("date: %2d/%02d  duree: %5d unite%s, "
			"prix: %7.2lf Francs\n",a1->jour,a1->mois,
			nb,(nb==0)? "":"s",
			a1->prix);
		total_cate[a1->ratio-'A']+=a1->duree;
		
		a1=a1->suivant;
	}
	
	printf("Consomation en seconde des categories non nulles:\n");
	for(i=0;i<sizeof(total_cate)/sizeof(int);i++)
	{
		if(total_cate[i]!=0)
		{
			printf("%c=%d,  ",i+'A',total_cate[i]);
			total_categorie[i]+=total_cate[i];
			if(i>=26)
			{
				printf("Erreur: taux invalide %c!!!!"
					,i+'A');
			}
		}
	}
	printf("\n");
}

/* compare a et b en supposant que a et b sont
des doubles pointeurs vers des utilisateurs
cette fonction est utilise par la fonction qsort */
int compare(const void *a,const void *b)
{
	struct utilisateur **u1,**u2;
	
	u1=(struct utilisateur **)a;
	u2=(struct utilisateur **)b;
	
	return ((*u1)->duree_totale - (*u2)->duree_totale);
}

/* trie la liste suivant le champs duree_totale */
void trie(struct utilisateur **liste_utilisateurs)
{
	struct utilisateur **u1,*u2,**u3,*u4,*u5;
	int i,nb;
	
	/* si la liste est vide ou n'a qu'un seul element, 
	la liste est deja trie, donc rien a faire */
	if((*liste_utilisateurs==NULL)||
		((*liste_utilisateurs)->suivant==NULL))
		return;
	
	/* calcul du nombre de noeud */
	u2=*liste_utilisateurs;i=0;
	while(u2!=NULL)
	{
		u2=u2->suivant;
		i++;
	}
	nb=i;
	
	/* creation d'un tableau de pointeur */
	u1=(struct utilisateur **)malloc(
		sizeof(struct utilisateur *)*nb);
	
	/* chaque element du tableau doit pointer vers un noeud */
	u2=*liste_utilisateurs;
	u3=u1;
	while(u2!=NULL)
	{
		*u3=u2;
		
		(u3)++;
		u2=u2->suivant;
	}
	
#ifdef DEBUGAGE	
	u2=*u1;i=0;u3=u1;
	while(i<nb)
	{
		printf("%d) %s\n",i,(*u3)->nom);
		
		i++;/*u2+=sizeof(struct utilisateur *);*/
		u3++;
	}
#endif
	
	/* trie du tableau de pointeurs */
	qsort(u1,nb,sizeof(u1),compare);
	
#ifdef DEBUGAGE	
	u2=*u1;i=0;u3=u1;
	while(i<nb)
	{
		printf("%d) %s\n",i,(*u3)->nom);
		
		i++;
		u3++;
	}
#endif	
	/* creation d'une liste trie u4 a partir du tableau trie */
	u4=NULL;u2=*liste_utilisateurs;u3=u1;i=0;
	while(i<nb)
	{
		if(u4==NULL)
		{
			u4=(struct utilisateur *)malloc(
				sizeof(struct utilisateur));
			u2=u4;
		}
		else
		{
			u2->suivant=(struct utilisateur *)
				malloc(sizeof(struct utilisateur));
			u2=u2->suivant;
		}
		
		memcpy(u2,*u3,sizeof(struct utilisateur));
		u2->suivant=NULL;
		
		i++;u3++;
	}
	
	/* effacement de la liste non trie */
	u2=*liste_utilisateurs;
	while(u2!=NULL)
	{
		u5=u2->suivant;
		free(u2);
		u2=u5;
	}
	
	/* remplacement de la liste non trie
	par la liste trie */
	*liste_utilisateurs=u4;
	
	/* effacement du tableau intermediaire */
	free(u1);
}

/* affichage des appels de tous les utilisateurs
   et trie les utilisateurs suivant leur consommation
   si ordonner est a 1 */
void affiche_total(struct utilisateur **liste_utilisateurs,
		int ordonner)
{
	int i,j,taux1[26]={0};
	
	memset(&taux1,0,sizeof(taux1));
	
	if(ordonner==1)
	{/* il faut trier */
		trie(liste_utilisateurs);
	}
	
	while(*liste_utilisateurs!=NULL)
	{
		affiche_utilisateur(*liste_utilisateurs,taux1);
		*liste_utilisateurs=(*liste_utilisateurs)->suivant;
	}
	printf(" ** Total en secondes des categories"
			" non nulles de tous les appels :\n");
	for(i=0;i<sizeof(taux1)/sizeof(int);i++)
	{
		if(taux1[i]!=0)
			printf("%c=%d,  ",i+'A',taux1[i]);
	}
	printf("\n");
	printf("Les autres categories sont nulles\n");
}

/* affiche l'utilisateur avec pour nom nom
 et affiche une erreur si il n'existe pas */
void affiche_util(struct utilisateur *liste_utilisateurs,char nom[])
{
	struct utilisateur *u1,*u2;
	char s1[20]="",s2[20];
	int i,taux1[26]={0};
	
	memset(&taux1,0,sizeof(taux1));
	u1=liste_utilisateurs;
	while(u1!=NULL)
	{
		/* annalyse la chaine u1->nom */
		sscanf(u1->nom,"%s %s",s1,s2);
#ifdef __BORLANDC__
		if(stricmp(s2,nom)==0)
#else
		if(strcasecmp(s2,nom)==0)
#endif
		{/* utilisateur trouve */
			
			affiche_utilisateur(u1,taux1);
			break;
		}
		u1=u1->suivant;
	}
	if(u1==NULL)
	{/* l'utilisateur n'est pas repertorie */
		printf("L'utilisateur n'existe pas\n");
		exit(1);
	}
}

int main(int argc,char *argv[])
{
  int i,j,nf,annuaire,taux;
  struct utilisateur *liste_utilisateurs=NULL;
  char buf1[20]="",*p;
  double d;
  
  if(argc>2)
  {
  	printf("Erreur: Il y a trop de parametres");
  	exit(1);
  }

  /* si une variable d'environnement PRIXUNITE est
  definie sa valeur remplace la valeur par defaut de prix_unite */
  p=getenv("PRIXUNITE");
  if(p!=NULL)
  {
  	strcpy(buf1,p);
  	if(buf1[0]!=0)
  	{
  		prix_unite=atof(buf1);
  	}
  }

  /* si une variable d'environnement PRIXTVA est
  definie sa valeur remplace la valeur par defaut de tva */
  p=getenv("PRIXTVA");
  if(p!=NULL)
  {
  	strcpy(buf1,p);
  	if(buf1[0]!=0)
  	{
  		tva=atof(buf1);
  	}
  }
  printf("prix de l'unite=%lf Fr, tva=%lf\n",prix_unite,tva);
  liste_utilisateurs=annalyse();
  
#ifdef DEBUGAGE
  affichage(liste_utilisateurs);
#endif
  
  if((argc>1)&&(strcmp(argv[1],"ordonner")!=0))
  {/* il ne faut afficher qu'un seul utilisateur */
  	affiche_util(liste_utilisateurs,argv[1]);
  }
  else
  {/* il faut afficher tous les utilisateurs */
  	affiche_total(&liste_utilisateurs,argc==2);
  }
  
  /* effacement de tous les utilisateurs */
  efface_utilisateurs(liste_utilisateurs);

#ifdef __BORLANDC__
  getch();
#endif
  
  return 0;
}