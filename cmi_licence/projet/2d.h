#ifndef IMAGE_2D
#define IMAGE_2D

#include "lgx.h"
/*#include*/

typedef struct { double x,y; } POINT2D;
typedef struct { int c,l; } PIXEL;
typedef struct { POINT2D min,max; } CLOTURE;
typedef struct { PIXEL bas,haut; } FENETRE;

typedef struct { double x,y; } VECTEUR2D;
typedef struct { POINT2D p; VECTEUR2D v;  } MOBILE;

void placer(POINT2D m);
void placerptcourant(void);
void tracer(POINT2D m1,POINT2D m2);
void allera(POINT2D m);
void tracervers(POINT2D m);
void polygone(POINT2D tab[],int taille);

void calculcoeff(CLOTURE clo,FENETRE fen);
void invcoeff();

PIXEL ptopix(POINT2D m);

int cs(POINT2D *a, POINT2D *b,CLOTURE clo);

POINT2D pixtop2(PIXEL pix);
POINT2D pixtop(int c, int l);

double norme(VECTEUR2D u);
double det(VECTEUR2D u,VECTEUR2D v);
double pscal(VECTEUR2D u,VECTEUR2D v);
double angle(VECTEUR2D u,VECTEUR2D v);
POINT2D rotation(POINT2D c, double theta, POINT2D p);


#endif


