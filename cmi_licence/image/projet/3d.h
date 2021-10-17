#ifndef H_3D_H
#define H_3D_H

#include"2d.h"

//#include "../lib/lgx.c"

typedef struct { double x,y,z,t; } POINT;
typedef struct { int nor, nex, nat; } ARETE;
typedef int *FACE;
typedef struct 
{ int ns, na, nf;
  POINT *so, *tso;
  ARETE *ar;
  FACE *faso, *faar;
} OBJET;
typedef struct { double xo, yo, zo, phi, psi, theta, r, d, d0;} CAMERA;
typedef double MATRICE [4][4];
typedef struct 
{
  POINT *tab;
  int nb_ligne,nb_col;
} PT_CTL;

void mulmat(MATRICE A, MATRICE B, MATRICE C);
POINT mulmavec(MATRICE M, POINT X);
void changement_repere(OBJET *ob);
void retour_cartesiennes(POINT *P);
void objet_cartesiennes(OBJET *obj);
void trace_arete(POINT A, POINT B);
void visual_transpar(OBJET T);
OBJET *lecture_objet(char *nom_fichier);
OBJET *lecture_objet2(char *nom_fichier);
PT_CTL *lecture_pt_control(char *nom_fichier);

/* pour le debugage */
void affiche_objet(OBJET *obj);
void affiche_mat(MATRICE m,char *s);

/* pour le debugage */
void affiche_objet(OBJET *obj);

/*
void placer3d(POINT m);
void placerptcourant3d(void);
void tracer3d(POINT m1,POINT m2);
void allera3d(POINT m);
void tracervers3d(POINT m);
*/

//extern MATRICE M15;

#endif

