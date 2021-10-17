#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/wait.h>
#include<setjmp.h>

/*
devoir 2 system
exercice 3
auteur(TP3): Schudel Edwar
	     Barret Alain

Ce programme calcul le produit d'une matrice
 par un vecteur

*/


/* taille maximale du vecteur */
#define N (10)

int PF1[N][2],PF2[N+1][2]; /* tube 1 et 2 */
int taille_matrice; /* taille utilise de la matrice */
int pid[N]; /* numero de processus des fils */
int no_fils=-1; /* numero de creation du fils
		 = numero de colonne traite par le fils */
jmp_buf env;

/* lecture du vecteur et de la matrice,
 envoi des valeurs, et affichage du resultat */
void saisie_et_envoi(void)
{
  int i,j,val;

  printf("Entrez les Xi:\n");

  /* lecture du vecteur X */
  for(i=0;i<taille_matrice;i++)
  {
     printf("X[%d] = ",i);
     scanf("%d",&j);
     write(PF1[i][1],&j,sizeof(int));
  }

  /* lectures de la matrice A */
  printf("Entrez les Ai,j par colonne\n");
  for(i=0;i<taille_matrice;i++)
  {
     printf("Entree la colonne %d:\n",i);
     for(j=0;j<taille_matrice;j++)
     {/* lectures */
        printf("A[%d][%d] = ",j,i);
        scanf("%d",&val);
        write(PF1[i][1],&val,sizeof(int));
     }
  }

  /*sleep(10);*/
  /* affichage des resultats Y */
  printf("Resultat:\n");
  for(i=0;i<taille_matrice;i++)
  {
     read(PF2[taille_matrice-1][0],&val,sizeof(int));
     printf("Y[%d] = %d\n",i,val);
  }

  /* attend la mort des fils */
  while(wait(NULL)!=-1);
  printf("Fin des processus fils\n");
}

/* traitement de la colonne numero no_fils */
void traitement_fils(void)
{
   int i,resultat_col_precedant,resultat,v,v2;

   /*sleep(10);*/
   read(PF1[no_fils][0],&v,sizeof(int));

   for(i=0;i<taille_matrice;i++)
   {
      if(no_fils!=0)
      {	
      	 read(PF2[no_fils-1][0],&resultat_col_precedant,sizeof(int));
      	 printf("Le processus numero %d recupere"
      	 	" le resultat partiel %d de la ligne %d\n",
      	 		no_fils,resultat_col_precedant,i);
      }
      else
         resultat_col_precedant=0;

      /* lecture de A[i][no_fils] */
      read(PF1[no_fils][0],&v2,sizeof(int));

      /* calcul de Y[i] */
      resultat=resultat_col_precedant+v*v2;

      /* envoi du resultat */
      write(PF2[no_fils][1],&resultat,sizeof(int));

   }
   printf("C'est fini pour le processus fils %d\n",no_fils);
}

/* programme principal */
int main()
{
  int i,except,nb_essai=0;

  /* saut inconditionnel */
  except=setjmp(env);

  if(nb_essai<3)
  {
    /* affichage du nombre de tentative */
    if(nb_essai>0) printf("Essai no %d\n",nb_essai+1);
    /* lecture de la taille de la matrice */
    printf("Quelle est la taille de la matrice ? :");
    scanf("%d",&taille_matrice);
    if( (taille_matrice<2) || (taille_matrice>=N) )
    { /* la taille ne convient pas */
      printf("Erreur: la taille ne convient pas,"
      	" elle doit etre >=2 et <%d\n",N);
      /* refait un essai */
      nb_essai++;fflush(stdin);
      longjmp(env,1);
    }

    for(i=0;i<taille_matrice;i++)
    {/* overture des pipe */
      pipe(PF1[i]);
      pipe(PF2[i]);
    }

    for(i=0;i<taille_matrice;i++)
    {/* creation des fils */
      if((pid[i]=fork())==0)
      {
         close(PF1[i][1]);
         break;
      }
    }

    if(i==taille_matrice)
    {/* traitement du pere */
        printf("Je suis le pere %d\n",getpid());
        for(i=0;i<taille_matrice;i++)
        {
           close(PF1[i][0]);
        }
        printf("\n");
        /* lecture des valeurs  */
        saisie_et_envoi();
    }
    else
    {/* traitement des fils */
        no_fils=i;
        /* calculs des fils */
        traitement_fils();
    }
    return 0;
  }
  else
  {/* nombre d'essai depasse */
      printf("Il y a eu trop d'erreurs\n");
      return except;
  }

}
