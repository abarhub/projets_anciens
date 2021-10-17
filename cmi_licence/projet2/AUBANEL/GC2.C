#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define _NON_DEBUGAGE_

#include"debugage.h"

/* taille qui isole le contenu d'un maillon du reste */
#define TAILLE_SECURITE (3)

#define TAILLE_BLOC(X) ((X)+2*TAILLE_SECURITE+sizeof(void*)+sizeof(int)*2)

enum VF {vrai,faux};

/* cette structure est un maillon de la liste
des blocs memoires disponibles
sa taille est variable:
le contenu est entoure pour eviter (et detecter)
les erreurs d'ecriture */
typedef struct MEMORY
{
  struct MEMORY *suivant; /* maillon suivant */
  int longueur;     /* longueur totale du maillon */
  enum VF marque;    /* maillon marque ou non */
  char tmp[TAILLE_SECURITE];  /* pour eviter qu'une erreur dans l'utilisation
				 de contenu ne pertube les autres champs */
  char contenu;  /* ce qui est utilise par le programmeur */
} MEMORY;

//enum VF {vrai,faux};

static int *debut_pile=NULL,*fin_pile=NULL;
static MEMORY *debut_mem=NULL,*fin_mem=NULL;
static void **var_global=NULL;
static int taille_var_global=0,dernier_var_global=0;
static int nb_maillon=0;
static int *fin_static,*debut_static=NULL;

void *gc_cherche(void *pt);

#define INVARIANT CHECK_MSG((nb_maillon>=0)/*&&(nb_maillon<=dernier_var_global)*/, \
			("nb_maillon incorrect(%d)",nb_maillon))

/* retourne le nobre de maillons */
int nb_elt(void)
{
  return nb_maillon;
}

/* marque le bloc m et tous les blocs vers lesquel il pointe */
static void marque(MEMORY *m)
{
  MEMORY *m1;
  int i;
  int *p;
	
  INVARIANT;
  CHECK_MSG(m!=NULL,("Marquage d'un maillon NULL"));
  
  if(m->marque==vrai)
    return;
  
  PRINT(("Marquage de %p\n",m));
  m->marque=vrai;
  
  for(p=(int*)&(m->contenu),i=0;/*i<m->longueur*/
      p<(int*)((char*)m+sizeof(char)*m->longueur-sizeof(char)*TAILLE_SECURITE)
	;i++,p++)
    {// marquage des blocs vers lesquel il pointe 
      
      // l'invariant de boucle
      CHECK_MSG(i>=0&&i<=m->longueur,("i n'a pas une bonne valeur"));
      
      //m1=gc_cherche((void*)*((int*)p));
      //m1=gc_cherche((void*)*p);
      m1=gc_cherche((void*)*p);
      if(m1!=NULL)
	marque(m1);
    }
  INVARIANT;
}

/* efface le bloc mde la liste des blocs memoires */
static void gc_efface(MEMORY *m)
{
  MEMORY *m1;
  
  /*if(m==NULL)
    return;*/
  INVARIANT;
  CHECK_MSG(m!=NULL,("Effacement d'un maillon NULL"));
  CHECK_MSG(nb_maillon>0,("Il n'y a pas de maillon"));
  CHECK_MSG(NULL!=gc_cherche(m),("Ce maillon n'existe pas"));
  
  PRINT(("Effacement de %p\n",m));
  if(m==debut_mem)
    { /* le bloc est au debut */
      debut_mem=debut_mem->suivant;
    }
  else
    {
      // recherche dans m1 de la cellule precedante
      m1=debut_mem;
      while((m1!=NULL)&&(m1->suivant!=m))
	m1=m1->suivant;
      
      if(m1!=NULL)
	{ /* actualisation du pointeur sur la fin de la liste */
	  m1->suivant=m->suivant;
	  if(m==fin_mem)
	    {
	      fin_mem=m1;
	    }
	}
      else
	{ /* le bloc n'est pas dans la liste (donc pb du GC) */
	  printf("Erreur du gc: Impossible de trouver le bloc %p\n",m);
	}
    }
  
  if(debut_mem==NULL)
    fin_mem=NULL;
  
  PRINT(("free(%p)\n",m));
  nb_maillon--;
  
  memset(m,0,m->longueur);
  
  free(m);
  
  CHECK_MSG(nb_maillon>=0,("La fonction gc_efface ne remplit pas sont contrat"));
  INVARIANT;
}

/* fait un cycle de marque and sweep */
void gc(void)
{
  int i=0;
  MEMORY *m1,*m2;
  int *p;
	
  INVARIANT;
  fin_pile=&i;
  PRINT(("Gc:%p,%p\n",debut_mem,fin_mem));
	
  /* marque les blocs utilise */
  for(p=fin_pile;p<=debut_pile;p++,i++)
    {
      
      CHECK_MSG(p>=fin_pile&&p<=debut_pile+1,
		("p n'a pas une bonne valeur(%p<%p<%)",fin_pile,p,debut_pile));
      
      //if(*p==/*0x662693*/0x66269B)
      {
	//printf("trouve\n");//getch();
      }
      /*if(i>80)
	{
	STOP(("Attention!!\n"));
	break;
	}*/
      //printf("%d=%d(%p)\n",i,*p,*((int*)p));
      m1=gc_cherche((void*)*p);
      if(m1!=NULL)
	{/* le bloc m1 est utilise */
	  marque(m1);
	}
    }
  
  /* marquage des variables globales */
  if(var_global!=NULL)
    {
      PRINT(("Marquage des var globales\n"));
      for(i=0;i<=dernier_var_global;i++)
	{
	  //printf("%d:%x\n",i,(int*)(((int*)*var_global)[i]));
	  
	  //m1=gc_cherche((void*)*var_global);
	  //m1=gc_cherche((void*)(var_global[i]));
	  m1=gc_cherche((int*)(((int*)*var_global)[i]));
	  if(m1!=NULL)
	    {/* le bloc m1 est utilise */
				//printf("trouve!(%p)",m1);getch();
	      marque(m1);
	    }
	}
      PRINT(("Fin de marquage des var globales\n"));
    }
  
  /* efface les blocs non utilises
     et demarque les blocs utilises */
  m1=debut_mem;
  while(m1!=NULL)
    {
      if(m1->marque==faux)
	{
	  m2=m1->suivant;
	  gc_efface(m1);
	  m1=m2;
	}
      else
	{
	  m1->marque=faux;
	  m1=m1->suivant;
	}
    }
  
  PRINT(("fin du gc\n"));
  INVARIANT;
}

/* ajoute une variable globale */
void gc_ajoute_global(void **p)
{
  INVARIANT;
  if(var_global==NULL)
    {
      taille_var_global=10;dernier_var_global=0;
      var_global=(void**)malloc(sizeof(void*)*taille_var_global);
      var_global[0]=p;
    }
  else
    {
      //if(dernier_var_global<taille_var_global)
      {
	dernier_var_global++;
      }
      //else
      if(dernier_var_global>=taille_var_global)
	{
	  char *tmp=(char*)var_global;
	  
	  taille_var_global+=10;
	  var_global=(void**)malloc(sizeof(void*)*taille_var_global);
	  memset(var_global,0,sizeof(void*)*taille_var_global);
	  memcpy(var_global,tmp,taille_var_global-10);
	}
    }
  var_global[dernier_var_global]=p;
  INVARIANT;
}

/* efface le tableau des variables globales */
void efface_var_global(void)
{
  INVARIANT;
  if(var_global!=NULL)
    free(var_global);
  dernier_var_global=taille_var_global=0;
  INVARIANT;
}

/* netoi toute la memoire utilise
Cette fonction est appele automatiquement
 a la fin du programme */
static void gc_nettoie(void)
{
  MEMORY *p,*p2;
  
  INVARIANT;
  PRINT(("Nettoyage de la memoire...\n"));
  
  p=debut_mem;
  while(p!=NULL)
    {
      p2=p->suivant;
      gc_efface(p);
      p=p2;
    }
  
  efface_var_global();
  
  PRINT(("Fin du GC\n"));
  INVARIANT;
}

/* initialise la gestion de la memoire */
void gc_init(int *p/*,int *p2*/)
{
  debut_pile=p;
  atexit(gc_nettoie);
  INVARIANT;
}

/* ajoute le bloc de memoire p a la liste
des bloc memoires utilises */
static void gc_ajoute(MEMORY *p)
{
  INVARIANT;
  p->suivant=NULL;
  if(debut_mem==NULL)
    {
      debut_mem=fin_mem=p;
      nb_maillon=1;
    }
  else
    {
      fin_mem->suivant=p;
      fin_mem=p;
      nb_maillon++;
    }
  INVARIANT;
}

/* cree un bloc memoire, le met dans la liste
et retourne la portion du bloc qui peut etre utilise */
void *gc_malloc(int lg)
{
  int j;
  MEMORY *p2;
  
  INVARIANT;
  CHECK_MSG(lg>0,("gc_malloc avec une longueur(=%d) <=0",lg));
  //CHECK_MSG(nb_maillon>=0,(""));
  
  j=TAILLE_BLOC(lg);
  
  p2=(MEMORY*)malloc(j);
  memset(p2,0,j);
  
  p2->longueur=j;
  p2->marque=faux;
  gc_ajoute(p2);
  
  INVARIANT;
  return &(p2->contenu);
}

void *gc_realloc(void *p,int lg)
{
  MEMORY *p2,*p3;
  
  INVARIANT;
  CHECK_MSG(lg>0,("gc_realloc avec une longueur(=%d) <=0",lg));
  
  p3=gc_cherche(p);
  if(p3==NULL)
    {
      INVARIANT;
      return gc_malloc(lg);
    }
  else
    {
      p2=(MEMORY *)malloc(lg);
      /* remplit le bloc par 0 */
      memset(p2,0,TAILLE_BLOC(lg));
      p2->longueur=TAILLE_BLOC(lg);
      p2->marque=faux;
      gc_ajoute(p2);
      INVARIANT;
      return p2;
    }
}

/* copie la chaine de caractere p et la retourne */
char *gc_strdup(char *p)
{
  char *str;
  int len;
  
  INVARIANT;
  len=strlen(p)+1;
  str=gc_malloc(len);
  strcpy(str,p);
  INVARIANT;
  return str;
}

/* affiche des information sur la liste
des blocs memoires */
void gc_affiche(void)
{
  int i;
  MEMORY *p;
  
  INVARIANT;
  printf("Affichage de la memoire:\n");
  printf("static: debut=%p, fin=%p\n",debut_static,fin_static);
  if(debut_mem==NULL)
    {
      printf("Vide\n");
		return;
    }
  
  
  i=0;
  for(p=debut_mem;p!=NULL;p=p->suivant)
    {
      printf("%d=%d\n",i,p->longueur);
      i++;
    }
  printf("Fin affichage memoire\n");
  INVARIANT;
}

/* cherche si le pointeur pt pointe sur un bloc
de la liste et le retourne. 
Sinon retourn NULL */
void *gc_cherche(void *pt)
{
  MEMORY *p,*p2;
  
  INVARIANT;
  p=debut_mem;
  while(p!=NULL)
    {
      p2=(MEMORY*)((char*)p+sizeof(char)*(p->longueur));
      if(((MEMORY*)pt>=p)&&((MEMORY*)pt<p2))
	{
	  INVARIANT;
	  return p;
	}
      
      p=p->suivant;
	}
  INVARIANT;
  return NULL;
}

/* retourne 1 ssi p a ete allouee par le GC */
int gc_bloc_valide(void *p)
{
  INVARIANT;
  return (gc_cherche(p)==NULL)?0:1;
}

