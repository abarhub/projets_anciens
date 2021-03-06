#include<stdio.h>
#include<stdarg.h>
//#include"erreur.h"
#include"lexer.h"
#include "debugage.h"
#include"gc.h"


/* ce fichier gere les erreurs
 (affichage et comptage)
 le fichier qui exporte les fonctions est "erreur.h" */

int nb_erreurs=0,pause_touch=0;

void affiche_erreur(char *s, ...)
{// affiche la chaine s (meme format que printf)
  va_list arg;

  va_start(arg,s);
  if(no_ligne>=0)
    printf("Erreur (ligne %d) : ",no_ligne);
  else
    printf("Erreur:");

  vfprintf(stdout,s,arg);
  //vprintf(s,arg);
  va_end(arg);
  nb_erreurs++;
  if(pause_touch)
     getchar();
}

void affiche_erreur_ligne(int no,char *s,/*...*/ va_list arg)
{
  //va_list arg;
  int tmp=no_ligne;
  
  //va_start(arg,s);
  no_ligne=no;
  affiche_erreur(s,arg);
  no_ligne=tmp;
  //va_end(arg);
}
