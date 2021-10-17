#ifndef H_3D_H
#define H_3D_H

#include"2d.h"

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
void lecture_pt_control(char *nom_fichier);
void visual_peintre(OBJET *objet);
void transformation_orthogonale(OBJET *obj);
void transformation_perpective(OBJET *obj);
void remplir_M15(double xo, double yo, double zo, double theta, double phi,
		 double psi);
void remplir_Mt(double d, double r);

OBJET *triangulation(double(*f)(double a,double b), 
       double(*g)(double a,double b), 
       double(*h)(double a,double b),
       double umin,double umax,double vmin,
       double vmax,int nu,int nv);

/* pour le debugage */
void affiche_objet(OBJET *obj);
void affiche_objet_ds_fichier(OBJET *obj);
void affiche_mat(MATRICE m,char *s);
void efface_objet(OBJET *objet);

double fonc_pt_contr1(double u,double v);
double fonc_pt_contr2(double u,double v);
double fonc_pt_contr3(double u,double v);

/* pour le debugage */
void affiche_objet(OBJET *obj);

extern MATRICE M15,Mt;

/* pour z_buffer */
void remplit_couleurs(OBJET *obj);
//POINT normale2(FACE f, OBJET *obj);
POINT normale(FACE f, OBJET *obj);
//POINT normale(FACE f, OBJET *obj,char c);
int eclairement(FACE f,OBJET *obj);
void z_buffer(OBJET *obj);

#endif

