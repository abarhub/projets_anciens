#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#ifdef __BORLANDC__
#include<conio.h>
#endif
#define DIM 100000
#define ZOU 6
/*
TP 1 algorithmique
exercice Tri
auteur: Schudel Edwar
	Barret Alain
	Aubanel Vincent
Ce programme trie des tableau de differentes tailles 
avec differentes methodes de tri
*/

typedef int *mat;
void affiche(mat t,int n);

void init(mat t,int n)
{/* remplit le tableau t de nombre aleatoire */
  int i;
  
  srand(time(0));  //initialisation du generateur de nombre aleatoire 
  
  for(i=0;i<n;i++) //remplissage
    t[i]=rand();
}

void permute(int *a,int *b)
{/* permute a et b */
  int c;
  
  c=*a;*a=*b;*b=c;
}

/******** tri a bulle ********/
/* complexite: en test=O(n*n) en permutation=O(n*n) */

void tri_a_bulle(mat t,int n)
{/* tri a bulle de t[0..n-1] */
  int i,j;

  i=0;
  while(i<n-1)
    {
      j=n-1;
      while(j>i)
	{
	  if(t[j]<t[j-1]) permute(t+j,t+j-1);
	  j=j-1;
	}
      i=i+1;
    }
}

/********* tri par selection ********/
/* complexite: en test=O(n*n) en permutation=O(n) */

void tri_par_selection(mat t,int n)
{/* tri par selection de t[0..n-1] */
  int i,j,pos;
  
  i=0;
  while(i<n)
    {
      pos=i;
      for(j=i+1;j<n;j++)
	{
	  if(t[j]<t[pos])
	    pos=j;
	}
      if(i!=pos)
	permute(t+i,t+pos);
      i=i+1;
    }
}

/****** tri par insertion sequentielle ******/
/* complexite: en test=O(n*n) en permutation=O(n*n) */

void tri_insertion_sequentielle(mat t,int n)
{
  int i,j,pos,val;
  
  for(i=1;i<n;i++)
    {
      val=t[i];pos=0;
      while((pos<=i)&&(t[pos]<val))
	pos++;
      
      for(j=i;j>pos;j--)
	t[j]=t[j-1];
      t[pos]=val;
    }
}

/****** tri par insertion dichotomique ******/
/* complexite: en test=O(n*log2(n)) en permutation=O(n*n) */

int position_dicho_iteration(int i,mat t)
{
  int dbt=0,fin;
  fin=i;
  while(dbt<fin)
    {
      if(t[(dbt+fin)/2]>=t[i])
	fin=(dbt+fin)/2;
      else
	dbt=(dbt+fin)/2+1;
    }  
  return fin;
}

void translation(mat t,int debt,int fin)
{/* deplace le tableau t de 1 case:
    t[debt+1..fin+1]=t[debt..fin] */
  int i;
 
 for(i=fin;i>=debt;i--)
  t[i+1]=t[i];
}


void tri_insertion_dichotomique(mat t,int n)
{/* tri par insertion dichotomique de t[0..n-1]*/
  int pos,x,i;
  for(i=1;i<n;i++)
    {
      pos=position_dicho_iteration(i,t);
      x=t[i];
      translation(t,pos,i-1);
      t[pos]=x;
    }
}

/******** tri par fusion ***********/
/* complexite: en test=O(n*log2(n)) en permutation=O(n*log2(n)) */

void fusion(mat t,int debut,int milieu,int fin)
{/*fusion de t[debut..milieu] et t[milieu+1..fin]
   vers t[debut..fin]*/
  mat temp;
  int i,j,k;
  
  j=debut;k=milieu+1;
  if(fin-debut>=0)
    temp=malloc(sizeof(int)*(fin-debut+1));
  else return;
  
  for(i=0;i<=fin-debut;i++)
    {
      if(j>milieu)
	{
	  temp[i]=t[k];
	  k++;
	}
      else if(k>fin)
	{
	  temp[i]=t[j];
	  j++;
	}
      else 
	{
	  if(t[j]<t[k])
	    {
	      temp[i]=t[j];
	      j++;
	    }
	  else
	    {
	      temp[i]=t[k];
	      k++;
	    }
	}
    }
  for(i=0;i<=fin-debut;i++)
      t[i+debut]=temp[i];
  free(temp);
}

void tri_fusion(int debut,int fin,mat t)
{/* tri en fusion de t[debut..fin]  */
  int m;

  if(debut<fin)
    {
      m=(debut+fin)/2;
      tri_fusion(debut,m,t);
      tri_fusion(m+1,fin,t);
      fusion(t,debut,m,fin);
    }
}

/******* tri par tas (non implemente) **********/
/* complexite: en test=O(1) en permutation=O(1)
 (car la fonction ne fait rien et n'implemente pas de tri) */

/* tri par tas non implemente */
void tri_tas(mat t,int n)
{
  return;
}

/******* tri rapide *********/
/* complexite: en test=O(n*n) en permutation=O(n*n) */

int place_pivot(int g,int d,mat t)
{
  int p,i,x;
  
  p=g;i=g+1;x=t[g];
  while(i<=d)
    {
      if(t[i]<x)
	p++;permute(t+p,t+i);
      i++;
    }
  permute(t+g,t+p);
  return p;
}

void rapide(mat t,int d,int f)
{/* tri rapide de [d..f] */
  int pivot;
  
  if(d<f)
    {
      pivot=place_pivot(d,f,t);
      rapide(t,d,pivot-1);
      rapide(t,pivot+1,f);
    }
}

/*************** tri radix ***********************/
/* complexite: en O(n) */

/* un noeud de la liste chainee */
struct elt
{
  int contenu;
  struct elt *suivant;
};

/* affiche les elements du tableau */
void afficher_liste(struct elt *tab[])
{
  struct elt *p1;
  int i;
  
  for(i=0;i<2;i++)
    {
      printf("%d)",i);
      p1=tab[i];
      while(p1!=NULL)
	{
	  printf("%d ",p1->contenu);
	  p1=p1->suivant;
	}
      printf("\n");
    }
}

void ajoute(int element,struct elt *tab[],int position_elt)
{/* ajoute l'element element  au tableau de liste tab
    ce tableau n'a que 2 cellules.
    position_elt est la position en nombre de bit que
    l'on doit annalyser. */
  
  int val_pos;
  struct elt *p1;
  
  val_pos= ( element&(1<<position_elt) ) ==
    (1<<position_elt);
  p1=(struct elt *)malloc(sizeof(struct elt));
  p1->contenu=element;
  p1->suivant=tab[val_pos];
  tab[val_pos]=p1;
}

void efface_tableau(struct elt *tab[],int nb)
{/* efface le tableau de liste tab de nb elements */
  struct elt *p1,*p2;
  int i;
  
  for(i=0;i<nb;i++)
    {
      p1=tab[i];
      while(p1!=NULL)
	{
	  p2=p1->suivant;
	  free(p1);
	  p1=p2;
	}
    }
}

void transforme_liste_tab(int tab[],int n,struct elt *tab2[],
			  int position)
{/* met le tableau de n element dans le tableau de liste tab2
    en le parcourant a l'envers */
  int i;
  
  for(i=n-1;i>=0;i--)
    ajoute(tab[i],tab2,position);
}

void transforme_tab(int tab[],int n,struct elt *tab2[])
{/* met le tableau de liste tab2 dans la tableau tab de taille n */
  int i,pos=0;
  struct elt *p1;
  
  i=0,p1=tab2[0];
  while((i<n)&&(pos<2))
    {
      if(p1==NULL)
	{
	  pos++;
	  p1=tab2[pos];
	}
      else if(p1->suivant==NULL)
	{
	  tab[i]=p1->contenu;
	  pos++;
	  if(pos<2)
	    p1=tab2[pos];
	  i++;
	}
      else
	{
	  tab[i]=p1->contenu;
	  p1=p1->suivant;
	  i++;
	}
    }
}

void tri_radix(mat t,int n)
{/* tri de type radix de t avec n elements */
  int i;
  struct elt *tab[2]={NULL,NULL};
  
  for(i=0;i<sizeof(int)*8-1;i++)
    {
      memset(tab,0,sizeof(tab));
      transforme_liste_tab(t,n,tab,i);
      /*afficher_liste(tab);*/
      transforme_tab(t,n,tab);
      efface_tableau(tab,sizeof(tab)/
		     sizeof(struct elt *));
    }
}

/********* arbre binaire de recherche *************/
/* complexite: en O(n) */

struct noeud 
{/* structure d'un noeud de l'arbre */
  int elt;
  struct noeud *d,*g;
};

typedef struct noeud *arbre;

void add_elt(arbre *racine,int elt)
{/* ajoute la valeur elt dans l'arbre */
  if(*racine==NULL)
    {
      *racine=(arbre)malloc(sizeof(struct noeud));
      (*racine)->d=NULL;
      (*racine)->g=NULL;
      (*racine)->elt=elt;
    }
  else
    {
      if( (*racine)->elt<elt) add_elt( &((*racine)->g),elt);
      else add_elt( &((*racine)->d),elt);
    }
}

void efface_arbre(arbre racine)
{/* efface l'arbre dont la racine est racine
    la fonction est recursive */
  if(racine==NULL)
    return;
  else
    {
      efface_arbre(racine->d);
      efface_arbre(racine->g);
      free(racine);
    }
}


void tri_arbre_vers_tableau(arbre racine,mat t,int **pos)
{/* parcour avec un ordre infixe l'arbre binaire "racine"
    et met les elements dans le tableau t a partir de
    la position *pos et *pos sera a la fin du tri
    la prochaine non encore utilise  le 3eme parametre
    doit etre l'adresse d'un pointeur sur pointeur qui pointe 
    vers le debut du tableau */
  if(racine!=NULL)
    {
      tri_arbre_vers_tableau(racine->d,t,pos);
      **pos=racine->elt;
      (*pos)++;
      tri_arbre_vers_tableau(racine->g,t,pos);
    }
}

void tri_arbre_binaire(mat t,int n)
{/* trie le tableau t[0..n-1] en utilisant un arbre binaire */
  arbre racine=NULL;
  int i,*p;
  
  for(i=0;i<n;i++)
    add_elt(&racine,t[i]);
  p=&t[0];
  tri_arbre_vers_tableau(racine,t,&p);
  
  efface_arbre(racine);
  return;
}

/****** fonctions annexes *******/

void affiche(mat t,int n)
{/* affichage */
  int i;
  
  printf("\n");
  for(i=0;i<n;i++)
    printf("%*d = %10ld\n",-3,i,t[i]);
}

int tri(int no,mat tab,int n)
{/* trie le tableau tab de taille n suivant la methode de tri no
    retourne la duree du tri en nombre de clocks*/
  int debut=clock(),fin;
  
  switch(no)
    {
    case 1:tri_a_bulle(tab,n);break;
    case 2:tri_par_selection(tab,n);break;
    case 3:tri_insertion_sequentielle(tab,n);break;
    case 4:tri_insertion_dichotomique(tab,n);break;
    case 5:tri_radix(tab,n);break;
    case 6:tri_arbre_binaire(tab,n);break;
    case 7:tri_fusion(0,n-1,tab);break;
    case 8:rapide(tab,0,n-1);break;
    case 9:tri_tas(tab,n);break;

    default:break;
    }
  fin=clock();
  return fin-debut;
}


void test(int methode)
{/* teste pour une methode de tri plusieurs tailles de tableau */
  mat tab;
  int duree,n,duree_totale,i,max,no_taille;
  int taille[]={10,50,100,200,500,1000,2000,5000,7000,
		10000,50000,100000,500000,1000000};
  switch(methode)
    {
    case 1: printf("Tri a bulle:\n");break;
    case 2: printf("Tri par selection:\n");break;
    case 3: printf("Tri par insertion sequentielle:\n");break;
    case 4: printf("Tri par insertion dichotomique:\n");break;
    case 5: printf("Tri par fusion:\n");break;
    case 6: printf("Tri par tas(non implemente):\n");break;
    case 7: printf("Tri rapide:\n");break;
    case 8: printf("Tri radix:\n");break;
    case 9: printf("Tri arbre binaire de recherche:\n");break;
    default: printf("Erreur : pas de methode\n");break;
    }
  
  for(no_taille=0;no_taille<(sizeof(taille)/sizeof(int));
      no_taille++)
    {
      n=taille[no_taille];
      duree_totale=0;max=0;
      tab=(mat)malloc(sizeof(int)*n);
      for(i=0;i<50;i++)
	{
	  duree=tri(methode,tab,n);
	  duree_totale+=duree;
	  if(max<duree)
	    max=duree;
	}
      printf("Pour une taille = %8d : ",n);
      printf("moyenne=%lf secondes ; ",
	     (double)duree_totale/i/CLOCKS_PER_SEC);
      printf("max=%lf secondes\n",(double)max/CLOCKS_PER_SEC);
    }
}


void test_moyenne(void)
{/* affiche les moyennes des differentes 
    methodes de tri suivant differentes tailles*/
  
  int taille_tableau,j,i,duree,no_methode;
  mat tab,copie;
  int taille[]={10,50,100,200,500,1000,2000,5000,
		  7000,10000,50000,100000,500000,1000000,10000000,100000000};
  double moyenne[10];
  int nb_max=10,nb_test,no_taille;
  
  for(i=0;i<9;i++) moyenne[i]=0.0;
  
  for(no_taille=0;no_taille<sizeof(taille)/sizeof(int);
      no_taille++)
    { /* pour chaque taille de tableau */
      taille_tableau=taille[no_taille];
      tab=(mat)malloc(sizeof(int)*taille_tableau);
      copie=(mat)malloc(sizeof(int)*taille_tableau);
      
      /* il faut faire nb_max testes */
      for(nb_test=0;nb_test<nb_max;nb_test++)
	{
	  init(tab,taille_tableau);
	  
	  for(no_methode=ZOU;no_methode<9;no_methode++)
	    { /* pour chaque methode */
	      
	      /* copie de tab dans copie */
	      for(i=0;i<taille_tableau;i++)
		copie[i]=tab[i];
	      
	      /* tri de copie */
	      duree=tri(no_methode+1,copie,taille_tableau);
#ifdef DEBUGAGE	
	      printf("Duree=%lf secondes\n",
		     (double)duree/CLOCKS_PER_SEC);
#endif
	      moyenne[no_methode]+=(double)duree/CLOCKS_PER_SEC;
	      
	    }
	  
	}
      
      /* affichage des resultats */
      for(i=ZOU;i<9;i++)
	printf("methode %d, de taille %d = "
	       "%lf secondes en moyenne\n",
	       i+1,taille_tableau,moyenne[i]);
	
      free(copie);
      free(tab);
    }
}

void menu(void)
{/* menu pour choisir un tri */
  int var,n;
  mat tab;
  int duree;
  
  printf("choisir dans le menu:\n");
  printf("1 tri a bulle \n"); 
  printf("2 tri par selection\n");
  printf("3 tri par insertion sequentielle \n");
  printf("4 tri par insertion dichotomique \n"); 
  printf("5 tri radix\n");
  printf("6 tri arbre binaire de recherche\n");
  printf("7 tri par fusion\n");
  printf("8 tri rapide\n"); 
  printf("9 tri par tas \n");
  printf("0 calcul de moyenne des methodes\n");
  
  scanf("%d",&var);
  if(var<-9||var>9) return;
  
  if(var==0) 
    {
      test_moyenne();
      return;
    }
  
  if(var<0) 
    {
      test(-var);
      return;
    }
  
  printf("entrer la dimension du tableau : ");
  scanf("%d",&n);
  fflush(stdin);
  if(n<=0)
    {
      printf("Erreur: la taille doit etre strictement"
	     " superieur a zero !\n");
      return ;
    }
  tab=(int *)malloc(sizeof(int)*n);
  init(tab,n);
#ifdef DEBUGAGE
  affiche(tab,n);
#endif  
  duree=tri(var,tab,n);
#ifdef DEBUGAGE
  affiche(tab,n);
#endif
  free(tab);
  
  printf("Duree du tri : %d (%lf secondes)\n",
	 duree,(double)duree/CLOCKS_PER_SEC);
}

int main()
{/* fonction principale */

  menu();
#ifdef __BORLANDC__
  getch();
#endif
  return 0;
}
