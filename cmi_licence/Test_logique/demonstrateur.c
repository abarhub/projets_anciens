#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>

// on peut decommenter pour debuguer

// test pour verifier des conditions qui doivent toujours etre vrai
//#define _NON_DEBUGAGE_

// affichage d'informations pendant la recherhce de la consistance
//#define _DEBUGAGE_CONSISTANCE_

// affichage d'informations pendant l'analyse du fichier
//#define _DEBUGAGE_ANALYSE_

#include"debugage.h"

#ifdef _DEBUGAGE_CONSISTANCE_
#define PRINT_CONS(X) printf X
#else
#define PRINT_CONS(X)
#endif

#ifdef _DEBUGAGE_ANALYSE_
#define PRINT_ANALYSE(X) printf X
#else
#define PRINT_ANALYSE(X)
#endif

// declaration de basename
#ifdef __BORLANDC__
char *basename(const char*);
#endif

// taille maximale de booleens
#define MAX_BOOL 2000
// taille maximales de clauses
#define MAX_CLAUSES 8000
// taille maximales de la pile
#define MAX_PILE 2000

// la liste des clauses
struct liste_clause{
  int num; // numero de la clause
  struct liste_clause *suiv;
};

// un booleen
typedef struct{
  struct liste_clause *l_c_pos; // liste des litteraux positifs
  unsigned int nb_occ_p; // nombre de litteraux positifs
  struct liste_clause *l_c_neg; // liste des litteraux negatifs
  unsigned int nb_occ_n; // nombre de litteraux negatifs
  unsigned short int vv; // valeur de verite du litteral
  char *nom; // nom du litteral
} booleen; // **BOOL**

// la liste des litteraux
struct litteral{
  unsigned int numero; // numero du booleen dans le tableau BOOL
  struct litteral *suiv; // literal suivant
  char signe; // signe du litteral
};

// une clause
typedef struct{
  int lg; // longueur de la clause
  int date; // date ou la clause est satisfaite
  struct litteral *lit; // liste des litteraux de la clause
  unsigned short int statut; // status de la clause
} clause; // **CLAUSES**

// un element de la pile
typedef struct{
  unsigned int num_lit; // numero du litteral
  enum st {mono,norm} statut; // status du litteral
  enum br {gauche,droite} branche; // branche parcourut
  char signe; // signe du litteral
} pile; // **PILE**

enum vf {vrai, faux};

// tableau des booleens
booleen BOOL[MAX_BOOL];
// tableau des clauses
clause CLAUSES[MAX_CLAUSES];
// la pile
pile PILE[MAX_PILE];


int n_b=0, /* nombre de booleens */
  n_c=0, /* nombre de clauses */
  n_p=-1; /* dernier element de la pile */
int date_etape; /* numero actuel de l'etape */
enum vf tous_models, /* vrai ssi il faut afficher
		       tous les models sans demander
		       a l'utilisateur*/
  cache_modele, /* vrai ssi on n'affiche pas les models */
  consistance_tout; /* vrai ssi on calcul la consistance
		      sans afficher de menu interactif */

/** prototypes des fonctions **/

void affiche_booleens(void);
enum vf pile_vide(void);
enum vf est_vide(void);

/* lit le fihcier nomme fic
et retourne une chaine contenant
 le fichier sans en simplifiant
 les separateurs de mots */
char *lecture(char *fic)
{
  char *s,*buff,*s2;
  int i/*=0*/,j;
  FILE *source;

  source=fopen(fic,"r");
  
  CHECK_MSG(source!=NULL,("echec de l'ouverture de \"%s\"\n",basename(fic)));
  printf("lecture du fichier \"%s\"...\n",basename(fic));
  
  s=(char *)malloc(500000*sizeof(char));
  s2=(char *)malloc(500000*sizeof(char));
  buff=(char *)malloc(5000*sizeof(char));
  
  CHECK_MSG(s!=NULL,("Pas assez d'espace en memoire pour la chaine s"));
  CHECK_MSG(s2!=NULL,("Pas assez d'espace en memoire pour la chaine buff"));
  CHECK_MSG(buff!=NULL,("Pas assez d'espace en memoire pour la chaine buff"));

  s[0]=0;
  
  // on met dans la chaine s le contenu du fichier
  while(fgets(buff,500,source))
    strcat(s,buff);
  
  fclose(source);
  free(buff);
  
  /* on remplit s2 de s en simplifiant les separateurs (espaces ou tabulations ou
     retours a la ligne = UN espace )*/
  i=0; j=0;
  while(s[i] != '\0')
    {
      //elimination des commentaires      
      if(s[i]=='{')
	{
	  do {i++;CHECK_MSG(s[i]!=0,("Le commentaire ne se termine pas"));}
	   while(s[i] != '}'); i++;
	  continue;
	}
      //elimination des caracteres de separation (tabulation, fin de ligne...)
      if(isspace(s[i]) && s[i]!=' ')
	{
	  do {i++;/*s[i]=' ';*/} while(isspace(s[i]) && s[i]!=' ');
	  continue;
	}
      //remplissage de s2 avec les caracteres restants
      while(s[i]!='{' && (isspace(s[i])==0 || s[i]==' ') && s[i] != '\0')
	{
	  s2[j]=s[i];
	  if(j>0 && s2[j]==' ' && s2[j-1]==' ') j--;
	  i++; j++;
	}
    }
  
  free(s);
  s2[j]=0;
  
  return s2;
}

int deja_def(char *s/*,int pas*/)
{/*** renvoie l'indice dans BOOL[] ou le booleen s est memorise s'il a deja ete defini
              -1 si le booleen passe en parametre est unique ***/
  int i=0,res=-1;
  
  while(BOOL[i].nom != NULL)
    {
      if(strcmp(s,BOOL[i].nom)==0)
	{ /* l'indice i correspond a booleen cherche */
	  res=i;
	  break;
	}
      i++;
    }
  return res;
}

int deja_present(char *b, char sg, int n_c/*,int pas*/)
{/*** renvoie 
      1 si le booleen b apparait avec le meme signe 
      que sg dans la clause pointee par n_c
      -1 s'il apparait avec le signe oppose
      0 s'il n'apparait pas
 ***/
  int res=0;
  struct litteral *prem=CLAUSES[n_c].lit;
  
  CHECK_MSG(b!=NULL,("b ne doit pas etre NULL"));
  CHECK_MSG(n_c>=0,("Il n'y a pas de clauses negatives(%d)",n_c));
  
  if(CLAUSES[n_c].lg>0)
    while(prem != NULL)
      {
	if(strcmp(BOOL[prem->numero].nom,b)==0)
	  {
	  if(prem->signe==sg)
	    res=1;
	  else
	    res=-1;
	  break;
	}
	prem=prem->suiv;
      }
  
  /* verification du resultat */
  switch(res)
    {
    case 1:
      CHECK_MSG(prem->signe==sg,("Les signes sont opposes"));
      break;
    case -1:
      CHECK_MSG(prem->signe!=sg,("Les signes sont egaux"));
      break;
    case 0:
	CHECK_MSG(prem==NULL,("Le booleen apparait dans la clause"));
	break;
    default:
      STOP(("La valeur retournee n'est pas correcte(=%d)",res));
    }
  
  return res;
}

struct litteral *supprime_clause(struct litteral *p)
{/* libere recursivement la memoire occupee par les elements de la clause p */
  
  CHECK_MSG(p!=NULL,("impossible d'effacer une liste vide"));

  if(p->suiv!=NULL)
  {/* supprime la clause suivante */
    supprime_clause(p->suiv);
  }
 
  if(p->signe=='+')
    {// p est positif
      BOOL[p->numero].l_c_pos=(BOOL[p->numero].l_c_pos)->suiv;
    }
  else
    {// p est negatif
      BOOL[p->numero].l_c_neg=(BOOL[p->numero].l_c_neg)->suiv;
    }
  free(p);
  p=NULL;
  
  return p;
}

/* annalyse le contenu de la chaine s et construit les tableaux
CLAUSES et BOOL */
void analyse(char *s)
{
  /* n_b: numero du booleen courant  dans BOOL[]
     et n_c: numero de la clause courante dans CLAUSES[] */
  int  ind; 
  int i,j;
  char *p;
  char *bool_tmp;
  struct liste_clause *ct;
  struct litteral *lt;
  char sgn;
  enum {debut_bool=0,lit_bool=1,mise_a_jour=2,
	efface_clause=3,prepare_clause_suivante=4} etat;
  
  bool_tmp=(char *)malloc(500*sizeof(char));

  printf("remplissage des structures de donn%ce...\n",233);
  PRINT_ANALYSE(("chaine lue:\n%s\nlongueur:%d\n",s,strlen(s)));
  CLAUSES[0].statut=0;
  CLAUSES[0].lg=0;
  i=0;
  etat=debut_bool;
  
  /* on parcourt s tant que l'on a pas atteint la fin de la chaine */
  while(s[i]!='\0')
    {
      switch(etat)
	{
	case debut_bool: //  on se positionne au debut du booleen a lire
	  while(s[i]==' ') {i++;}
	  switch(s[i])
	    {
	    case '-': sgn='-'; i++; etat=lit_bool; break;
	    case ';': i++; etat=prepare_clause_suivante; break;
	    default : sgn='+'; etat=lit_bool; break;
	    }
	  break;
	  
	case lit_bool: // on acquiert le booleen
	  j=0;
	  while((isalnum(s[i]) || s[i]=='%' || s[i]=='(' ||
		 s[i]==')' || s[i]==','
		 || s[i]=='_') && (s[i]!=' ' && s[i]!=';'))
	    {
	      bool_tmp[j++]=s[i++];
	    }
	  CHECK_MSG(j>0,("erreur: Le booleen est vide\n"));
	  bool_tmp[j]='\0';
	  /* test si le booleen lu apparait deja dans la clause courante:
	     - avec le meme signe -> on ne le rajoute pas a la clause
	     - avec le signe oppose -> on efface la clause */
	  switch(deja_present(bool_tmp,sgn,n_c))
	    {
	    case 1 : //on ne le rajoute pas dans la clause
	      etat=debut_bool; break;
	    case -1: // on vire la clause
	      etat=efface_clause; break;
	    case 0 : //on va updater BOOL[]
	      etat=mise_a_jour; break;
	    }
	  break;
	  
	case mise_a_jour: // mise a jour de BOOL[] et de CLAUSES[]
	  if((ind=deja_def(bool_tmp))==-1) 
	    {//on rencontre le booleen pour la premiere fois
	      p=(char *)calloc((j+1),sizeof(char));
	      BOOL[n_b].nom=p;
	      strcpy(BOOL[n_b].nom,bool_tmp);
	      BOOL[n_b].vv=2;
	      BOOL[n_b].nb_occ_p=0;
	      BOOL[n_b].nb_occ_n=0;
	      
	      ind=n_b;
	      n_b++;
	    }
	  
	  if(sgn=='+')
	    BOOL[ind].nb_occ_p++;
	  else
	    BOOL[ind].nb_occ_n++;
	  
	  (CLAUSES[n_c].lg)++;
	  lt=(struct litteral *)malloc(sizeof(struct litteral));
	  lt->suiv=CLAUSES[n_c].lit;
	  CLAUSES[n_c].lit=lt;
	  
	  (CLAUSES[n_c].lit)->signe=sgn;
	  (CLAUSES[n_c].lit)->numero=ind;
	  
	  if(sgn=='-') //on va developper la branche des <0
	    {
	      ct=(struct liste_clause *)calloc(1,sizeof(struct liste_clause));
	      ct->suiv=BOOL[ind].l_c_neg;
	      BOOL[ind].l_c_neg=ct;
	      
	      (BOOL[ind].l_c_neg)->num=n_c;
	    }
	  else         //on va developper la branche des >0
	    {
	      ct=(struct liste_clause *)calloc(1,sizeof(struct liste_clause));
	      ct->suiv=BOOL[ind].l_c_pos;
	      BOOL[ind].l_c_pos=ct;
	      
	      (BOOL[ind].l_c_pos)->num=n_c;
	    }     
	  etat=debut_bool;
	  break;
	  
	case efface_clause: // on supprime la clause
	  if(CLAUSES[n_c].lit!=NULL)
	    CLAUSES[n_c].lit=supprime_clause(CLAUSES[n_c].lit);
	  while(s[i]!=';') {i++;}
	  CLAUSES[n_c].lg=0;
	  etat=prepare_clause_suivante;
	  break; 
	  
	case prepare_clause_suivante: // on initialise la clause suivante
	  if(CLAUSES[n_c].lg>0) 
	    { // on augmente le nombre de clauses
	      n_c++;
	    }
	  else
	    CHECK_MSG(CLAUSES[n_c].lit==NULL,
		      ("erreur: CLAUSES[%d].lit!=NULL !\n",n_c));
	  CLAUSES[n_c].lg=0;
	  CLAUSES[n_c].statut=0;
	  CLAUSES[n_c].date=0;
	  etat=debut_bool;
	  break;
	}
    }
  printf("on a lu %d booleens differents et %d clauses non vides\n",n_b,n_c+1);
  free(bool_tmp);
}

/* affiche les clauses */
void affiche_clauses(void)
{
  int i;
  struct litteral *l;
  
  printf("---- Ensemble des clauses : -----------------\n");
  for(i=0;i<=n_c;i++)  
    { /* pour chaque clauses */
      printf("%d %d %d %d  ",i,CLAUSES[i].lg,CLAUSES[i].statut,CLAUSES[i].date);
      
      l=CLAUSES[i].lit;
      while(l != NULL)
	{ /* pour chaque litteral: on l'affiche */
	  if(l->signe =='-')
	    printf("%c",l->signe);
	  printf("%s ",BOOL[l->numero].nom);
	  l=l->suiv;
	}
      printf("\n");
    }
  printf("---------------------------------------------\n");
}

/*
affiche l'ensemble des booleens
*/
void affiche_booleens(void)
{
  int i=0;
  struct liste_clause *lp, *ln;
       
  printf("---- Dictionnaire des %d booleens : ------------\n",n_b);
  while(BOOL[i].nom != NULL)
    {/* pour chaque booleen */
      printf("%s (n%c%d) vv=%d p:%d n:%d\n",
	     BOOL[i].nom,176,i,BOOL[i].vv,BOOL[i].nb_occ_p,BOOL[i].nb_occ_n);
      lp=BOOL[i].l_c_pos;
      if(lp != NULL)
	{/* affichage des clauses le contenant positivement */
	  printf("  apparait positivement dans ");
	  if(lp->suiv != NULL) printf("les clauses ");
	  else printf("la clause   ");
	  while(lp != NULL)
	    {
	      printf("%d ",(lp)->num);
	      lp=lp->suiv;
	    }
	  printf("\n");
	}

      ln=BOOL[i].l_c_neg;
      if(ln != NULL)
	{/* affichage des clauses le contenant negativement */
	  printf("  apparait n%cgativement dans ",233);
	  if(ln->suiv != NULL) printf("les clauses ");
	  else printf("la clause   ");
	  while(ln != NULL)
	    {
	      printf("%d ",(ln)->num);
	      ln=ln->suiv;
	    }
	  printf("\n");
	}
      i++;
    }
  printf("---------------------------------------------\n");
}

enum vf pile_vide(void)
{/* retourne vrai ssi la pile des booleens est vide */
  if(n_p==-1) return vrai;
  else return faux;
}

enum vf est_vide(void)
{/* retourne vrai ssi l'ensemble des clauses est vide */
  int i;
  
  for(i=0;i<=n_c;i++)
    if(CLAUSES[i].lg>0)
      return faux;
  return vrai;
}

enum vf contient_vide(void)
{/* retourne vrai ssi l'ensemble des clauses
    contient au moins une clause vide */
  int i;
  
  for(i=0;i<=n_c;i++)
    if(CLAUSES[i].lg==0 && CLAUSES[i].statut==0)
      return vrai;
  return faux;
}

pile *choisir(void)
{/* retourne le litteral contenu dans la clause la plus courte
suppose qu'il servira dans la branche gauche
il ne doit pas y avoir de clause vide*/

  pile *res;
  int i,no,n_b_tmp;
  struct litteral *l;

  res=(pile *)malloc(sizeof(pile));

  no=-1;
  n_b_tmp=n_b;

  for(i=0;i<=n_c;i++) 
  { /* recherche dans n_b_tmp de la clause la plus petite */
	  if(CLAUSES[i].lg>0 && CLAUSES[i].lg<n_b_tmp)
		  {
			n_b_tmp=CLAUSES[i].lg; no=i; 
		  } 
  }

  CHECK_MSG(CLAUSES[no].lit!=NULL,("erreur: la clause %d est vide !\n",no));
  CHECK_MSG(CLAUSES[no].lit!=NULL,("erreur: la clause %d est vide !\n",no));
  
  l=CLAUSES[no].lit;
  while(BOOL[l->numero].vv!=2)
    { /* recherche un litteral indetermine */
      l=l->suiv;
    }

  res->signe=l->signe;
  res->num_lit=l->numero;
  res->statut=(CLAUSES[no].lg==1)?mono:norm;
  res->branche=gauche;

  return res;
}

void empiler(pile *p)
{/* empile le litteral au sommet de la pile
on ne doit pas depasser la taille maximale de la pile MAX_PILE */
  
  CHECK_MSG(n_p<MAX_PILE,("erreur: les dimensions de la pile"
		" sont depassees(n_b=%d)\n",n_b));

  n_p++;
  PILE[n_p]=*p;

  PRINT_CONS(("empilage de %c%s ",PILE[n_p].signe,
	BOOL[PILE[n_p].num_lit].nom));
  PRINT_CONS(("%s",(PILE[n_p].statut==mono)?("mono "):("normal ")));
  
  date_etape++;
}

void interpreter(void)
{ /* interpreter le litteral au sommet de la pile */
  int i,nb,*no_pos,*no_neg;
  struct liste_clause *c1,*c2;
  struct litteral *l;
  
  CHECK_MSG(pile_vide()!=vrai,("erreur: interpretation d'une pile vide\n"));
  
  nb=PILE[n_p].num_lit;

  BOOL[nb].vv=(PILE[n_p].signe=='+')?1:0;
  
  if(PILE[n_p].signe=='+')
    { /* le litteral est positif */
      c1=BOOL[nb].l_c_pos;
      c2=BOOL[nb].l_c_neg;
      no_pos=(int *)&(BOOL[nb].nb_occ_p);
      no_neg=(int *)&(BOOL[nb].nb_occ_n);
    } 
  else
    { /* le litteral est negatif */
      c2=BOOL[nb].l_c_pos;
      c1=BOOL[nb].l_c_neg;
      no_neg=(int *)&(BOOL[nb].nb_occ_p);
      no_pos=(int *)&(BOOL[nb].nb_occ_n);
    }
  
  while(c1!=NULL)
    { /* mise a jour des clauses qui devinennent satifaites */
      i=c1->num;
      if(CLAUSES[i].statut==0)
	{
	  CLAUSES[i].statut=1; //la clause est satisfaite
	  
	  (*no_pos)--;
	  l=CLAUSES[i].lit;
	  while(l!=NULL)
	    { /* mise a jour des booleens qui sont dans la clause */
	      if(BOOL[l->numero].vv==2)
		  { /* le booleen est indetermine */
		    if(l->signe=='+')
			  BOOL[l->numero].nb_occ_p--;
		    else
			  BOOL[l->numero].nb_occ_n--;
		  }
	      
	      l=l->suiv;
	    }
	  
	  CLAUSES[i].date=date_etape;
	}
      CLAUSES[i].lg=0;
      c1=c1->suiv;
    }
  
  while(c2!=NULL)
    {/* mise a jour des clauses qui contienent
	le booleen de signe oppose */
      i=c2->num;
      if(CLAUSES[i].lg>0)
	{	
	  CLAUSES[i].lg--;
	  (*no_neg)--;
	}
      c2=c2->suiv;
    }
}

void affiche_modele(void)
{/* affiche le model qui est sur la pile */
  int i,j;
  
  printf("modele: ");
  for(i=0;i<=n_p;i++)
    {
      j=PILE[i].num_lit;
	if(PILE[i].signe=='+')
	    printf(" %s ",BOOL[j].nom);
	else
	    printf("-%s ",BOOL[j].nom);
    }
  printf("\n");
}

void inverser_signe(pile *p)
{/* inverse le signe de p */
  CHECK_MSG(p!=NULL,("Inversion d'un booleen vide"));
  p->signe=(p->signe=='+')? '-':'+';
  p->branche=droite;
}

pile *depiler(void)
{/* depile le sommet de la pile et le retourne */
  pile *p;
  
  CHECK_MSG(n_p>=0,
  	("erreur: on essaye de depiler alors que la pile est vide (n_p=%d)\n",n_p));
  
  p=(pile*)malloc(sizeof(pile));
  *p=PILE[n_p];
  n_p--;
  
  // ce qui suis est utile pour le debugage
  PRINT_CONS(("depilage de %c%s ",
	 p->signe,BOOL[p->num_lit].nom));
  PRINT_CONS(("%s",(p->statut==mono)?"mono ":"normal "));
  PRINT_CONS(("dans branche "));
  PRINT_CONS(("%s",(p->branche==droite)?("droite\n"):("gauche\n")));
  
  CHECK_MSG(p!=NULL,("On depile un element vide"));

  return p;
}

int autre_bool_faux(int i)
{/* renvoie 1 si tous les litteraux
    ont ete interpretes a FAUX dans la clause i */
  int res=1;
  struct litteral *l;

  l=CLAUSES[i].lit;
  
  while(l!=NULL)
    {
      if( (l->signe=='+' && BOOL[l->numero].vv==1)
	  || (l->signe=='-' && BOOL[l->numero].vv==0) )
	{
	  res=0;
	  break;
	}
      l=l->suiv;	
    }

  return res;
}

void desinterpreter(pile *p)
{/* on desinterprete p dans le tableau de booleens */
  int i,no,*no_pos,*no_neg;
  struct liste_clause *l1,*l2;
  struct litteral *toto;
  
  CHECK_MSG(p!=NULL,("desinterpretation d'un element vide"));

  no=p->num_lit;
  
  BOOL[no].vv=2; //on remet a indetermine la valeur de verite du booleen
  
  if(p->signe=='+')
  {/* l'element desinterprete est positif */
    l1=BOOL[no].l_c_pos;
    l2=BOOL[no].l_c_neg;
    no_pos=(int *)&(BOOL[no].nb_occ_p);
    no_neg=(int *)&(BOOL[no].nb_occ_n);
  }
  else
  {/* l'element desinterprete est negatif */
    l2=BOOL[no].l_c_pos;
    l1=BOOL[no].l_c_neg;
    no_neg=(int *)&(BOOL[no].nb_occ_p);
    no_pos=(int *)&(BOOL[no].nb_occ_n);
  }
  
  while(l1!=NULL)
  { /* pour chaque clause qui le contient de meme signe */
    i=l1->num;
    
    if(CLAUSES[i].date==date_etape || autre_bool_faux(i)==1)
      { /* la clause etait satisfaite => on met la clause a insatisfaite */
	CLAUSES[i].statut=0;
	toto=CLAUSES[i].lit;
	while(toto!=NULL)
	  {
	    if(BOOL[toto->numero].vv==2)
	      {
		CLAUSES[i].lg++;
		if(toto->signe=='+')
		  BOOL[toto->numero].nb_occ_p++;
		else
		  BOOL[toto->numero].nb_occ_n++;
	      }
	    toto=toto->suiv;
	  }
      }
    l1=l1->suiv;
  }
  
  while(l2!=NULL)
  { /* les clauses qui le contienent de signe oppose */
    i=l2->num;
    if((autre_bool_faux(i)==1) )
      {
	CLAUSES[i].lg++;
	(*no_neg)++;
      }
    l2=l2->suiv;
  }
  
}

void consistance(void)
{ /* calcul de la consistance de la clause mis dans les tableau BOOL
	et CLAUSES */
  int nb_modeles=0,date_trouve=-1;
  enum {initial,succes,backtrack,final} etat=initial;
  enum vf {vrai,faux} trouve_modele=faux ,fini=faux;
  char c;
  pile *p;
  float t1,t2;

  date_etape=0;;
  if(tous_models==vrai)
  { /* calcul du temps de traitement */
	t1=clock();
  }

  while(fini==faux)
    { /* parcourt de l'arbre tant que fini!=vrai */
      switch(etat)
	{
	case initial: /* initial */
	  if(contient_vide()==vrai) etat=backtrack;
	  else 
	    {
	      if(est_vide()==vrai) etat=succes;
	      else 
		{
		  p=choisir();
		  empiler(p);
		  free(p);p=NULL;
		  interpreter();
		  etat=initial;
		}
	    }
	  break;
	case succes: /* succes */
	  if(cache_modele==faux)
	     affiche_modele();
	  trouve_modele=vrai;
	  fflush(stdin);
	  nb_modeles++;
	  if(date_trouve==-1)
	    date_trouve=date_etape;
	  
	  if(tous_models==vrai)
	    c='o';
	  else
	  { /* demande d'autres modeles */
	    printf("Voulez-vous un autre modele? (o/n): ");
	    fflush(stdin);
	    scanf("\n%c",&c);
	  }
	  if(c=='n') fini=vrai;
	  else etat=backtrack;
	  break;
	case backtrack: /* backtrack */
	  if(pile_vide()==vrai) etat=final;
	  else
	    {
	      p=depiler();
	      desinterpreter(p);
	      if((p->statut==mono) || (p->branche==droite))
		  etat=backtrack;
	      else {
		inverser_signe(p);
		empiler(p);
		free(p);p=NULL;
		interpreter();
		etat=initial;
	      }
	    }
	  break;
	case final: /* fin du parcourt de l'arbre */
	  if(trouve_modele==vrai)
	    printf("pas d'autre modele\n");
	  else
	    {
	      date_trouve=date_etape;
	      printf("ensemble inconsistant\n");
	    }
	  fini=vrai;
	  break;
	default:
	  STOP(("erreur interne dans consistance() %d\n",etat));
	  break;
	}
    }
  printf("Il y a eut %d modeles en %d etapes\n",nb_modeles,date_trouve);
  if(tous_models==vrai)
  { /* affichage de la duree */
	t2=clock();
      printf("La consistance a duree %.2f secondes\n",
	     (t2-t1)/CLOCKS_PER_SEC);
  }
}

void menu(void)
{/* affichage et selection du menu */
  int choix=0;

 while(choix!=4)
 {
  printf("Menu:\nvous pouvez :\n");
  printf(" 1- afficher l'ensemble des clauses\n");
  printf(" 2- afficher le dictionnaire des booleens\n");
  printf(" 3- tester la consistance\n");
  printf(" 4- quitter le programme\n ");
  scanf("%d",&choix);
  switch(choix)
    {
    case(1):
      affiche_clauses();
      break;
    case(2):
      affiche_booleens();
      break;
    case(3):
      consistance();
      break;
    case(4):
      printf("fin du programme\n");
      break;
    }
 }
}

/* effacement des structures */
void efface(void)
{
  clause *cl1;
  int i;
  struct litteral *lit1,*lit2;
  booleen *b1;
  struct liste_clause *c1,*c2;
  
  /* effacement du tableau CLAUSES */
  for(i=0;i<n_c;i++)
    {
      cl1=&(CLAUSES[i]);
      lit1=cl1->lit;
      while(lit1!=NULL)
	{
	  lit2=lit1->suiv;
	  free(lit1);
	  lit1=lit2;
	}
    }
  
  /* effacement du tableau BOOL */
  for(i=0;i<n_b;i++)
    {
      b1=&(BOOL[i]);
      if(b1!=NULL)
	free(b1->nom);
      
      /* effacement de la liste des clauses positives */
      c1=b1->l_c_pos;
      while(c1!=NULL)
	{
	  c2=c1->suiv;
	  free(c1);
	  c1=c2;
	}
      /* effacement de la liste des clauses negatives */
      c1=b1->l_c_neg;
      while(c1!=NULL)
	{
	  c2=c1->suiv;
	  free(c1);
	  c1=c2;
	}
    }
}

void print_syntaxe(char *nom_fichier)
{ /* affiche la syntaxe des arguments du programme et termine le programme */
      printf("Syntaxe: %s [-tous] [-cache] [-consistane] nom_fichier\n",
	    nom_fichier);
      printf("-tous :\tne questionne pas l'utilisateur si"
	     " il faut afficher le model suivant\n");
	printf("-cache :\tn'affiche pas les models\n");
      printf("-consistance :\tcalcul la consistance et termine le programme"
	     " en affichant des informations\n");
	exit(1);
}

char *analyse_param(int argc,char *argv[])
{ /* annalyse les parametres de la ligne de commande et
	retourne le fichier a analyser 
	s'il y a des erreurs, le programme est arrete */
  int i;
  char *nom_fichier;

  if(argc<=1)
    {
	printf("Erreur: Il faut au moins un nom de fichier\n");
	print_syntaxe(argv[0]);
    }
  
  /* valeurs par defaut */
  tous_models=faux;
  cache_modele=faux;
  consistance_tout=faux;
  nom_fichier=NULL;

  for(i=1;i<argc;i++)
    {
	if(strcmp(argv[i],"-tous")==0)
	  { /* affiche tous les models sans
	    questionner l'utilisateur */
	    tous_models=vrai;
	  }
	else if(strcmp(argv[i],"-cache")==0)
	  { /* n'affiche pas les models trouves */
	    cache_modele=vrai;
	  }
	else if(strcmp(argv[i],"-consistance")==0)
	{ /* calcul la consistance sans interaction 
	     avec l'utilisateur */
	    consistance_tout=vrai;
	    cache_modele=vrai;
	    tous_models=vrai;
	}
	else
	{ /* le nom du fichier a analyser */
	    if(nom_fichier!=NULL)
	    { /* c'est le 2eme fichier dans la ligne de commande */
		  printf("Erreur: Il ne doit y avoir qu'un seul nom de fichier\n");
		  print_syntaxe(argv[0]);
	    }
	    else
	    { /* il faut verifier que c'est un fichier accessible */
		  FILE *f;
		  
		  nom_fichier=argv[i];
		  f=fopen(nom_fichier,"r");
		  if(f==NULL)
		  { /* le fichier ne peut pas etre ouvert */
			printf("Erreur: Impossible d'ouvrir le fichier %s\n",
			    nom_fichier);
			exit(1);
		  }
		  fclose(f);
	    }
	}
  }

  if(nom_fichier==NULL)
    { /* il n'y a pas eut de fichier dans la ligne de commande */
	printf("Erreur:Il faut un nom de fichier\n");
	print_syntaxe(argv[0]);
    }

  return nom_fichier;
}

int main(int argc, char **argv)
{ /* fonction principale */
  float t1,t2;
  char *s,*nom_fichier;
  
  /* annalyse de la ligne de commande */
  nom_fichier=analyse_param(argc,argv);

  /* annalyse du fichier */
  t1=clock();
  s=lecture(nom_fichier);
  t2=clock();
  printf("la lecture a dur%c %.2f secondes\n",233,(t2-t1)/CLOCKS_PER_SEC);
  analyse(s);
  t1=clock();
  printf("le remplissage des sdd  a dur%c %.2f secondes\n",
	 233,(t1-t2)/CLOCKS_PER_SEC);
  free(s);
  
  if(consistance_tout==vrai)
    { /* calcul de la consistance */
      consistance();
    }
  else
    { /* menu interactif */
      menu();
    }
  
  /* effacement des tableaux */
  efface();
  
  return 0;
}
