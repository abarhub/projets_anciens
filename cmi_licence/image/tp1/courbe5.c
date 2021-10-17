#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include"2d.h"
//#include"lgx.c"

/* doit etre pair */
#define NB_POINT (1000)
#define CLOMINX (0)
#define CLOMINY (0)
#define CLOMAXX (100)
#define CLOMAXY (100)

void Sx(VECTEUR2D *v)
{
  v->x=-v->x;
}

void Sy(VECTEUR2D *v)
{
  v->y=-v->y;
}

void suivant(MOBILE *M, CLOTURE clo)
{
  if(M->p.x+M->v.x>clo.max.x || M->p.x+M->v.x<clo.min.x)
    {
      Sx(&M->v);
    }
 
  if(M->p.y+M->v.y>clo.max.y || M->p.y+M->v.y<clo.min.y)
    {
      Sy(&M->v);
    }
  
  M->p.x+=M->v.x;
  M->p.y+=M->v.y;
}

void trajectoire(MOBILE M, CLOTURE clo)
{
  MOBILE M2=M;
  int i=0;
  
  allera(M.p);
  
  do
    {
      suivant(&M2,clo);
      printf("p(%.2f,%.2f) v(%.2f,%.2f) %d\n",M2.p.x,M2.p.y,M2.v.x,M2.v.y,i);
      tracervers(M2.p);
      flushpl();
      i++;
    }
  //while( (M2.p.x!=M.p.x) && (M2.p.y!=M.p.y) && (M2.v.x!=M.v.x) && (M2.v.y!=M.v.y));
  while(i<100);
  flushpl();
}

void trajectoire2(MOBILE M,MOBILE M2, CLOTURE clo)
{
  MOBILE M3=M,M4=M2;
  
  /*allera(M.p);*/
  erase();
  tracer(M.p,M2.p);
  //sleep(1);
  flushpl();
  do
    {
      suivant(&M,clo);
      suivant(&M2,clo);
      //printf("p(%.2f,%.2f) v(%.2f,%.2f) %d\n",M.p.x,M.p.y,M.v.x,M.v.y,i);
      //tracervers(M.p);
      erase();
      tracer(M.p,M2.p);
      //sleep(1);
      flushpl();
    }
  while( (M3.p.x!=M.p.x) && (M3.p.y!=M.p.y) && (M3.v.x!=M.v.x) && (M3.v.y!=M.v.y));
  flushpl();
}

void casteljau(POINT2D M[], int dim, double t)
{
	int i,j;
	
	if(t<0 || t>1)
	{
		printf("t doit etre compris entre 0 et 1\n");
		return ;
	}
	for(i=1;i<dim;i++)
	{
		for(j=0;j<dim-i;j++)
		{
			//if()
			//M[i+j][i+j+i]
			//else
			//printf("M[%d,%d]: M[%d,%d] M[%d,%d]\n",j,j+i,j,j+i-1,j+1,j+i);
			
			//printf("M[%d]: M[%d] M[%d]\n",j,j,j+1);
			
			M[j].x=(1-t)*M[j].x+t*M[j+1].x;
			M[j].y=(1-t)*M[j].y+t*M[j+1].y;
			//printf("M[%d,%d]:%lf,%lf\n",j,j+i,M[j].x,M[j].y);
			
		}
	}

}

void serpent(int no)
{
	MOBILE *M;
	int i;
	
	M=(MOBILE*)calloc(no,sizeof(MOBILE));
	for(i=0;i<no;i++)
	{
		M[i].p.x=(rand()%(CLOMAXX-CLOMINX))+CLOMINX;
		M[i].p.y=(rand()%(CLOMAXY-CLOMINY))+CLOMINY;
		M[i].v.x=rand()%((CLOMAXX-CLOMINX)/100);
		M[i].v.y=rand()%((CLOMAXY-CLOMINY)/100);
	}
	
	free(M);
}

int main(int argc, char **argv)
{
  POINT2D tab[NB_POINT],*point,p1,p2;
  CLOTURE clo;
  FENETRE fen;
  MOBILE M,M2;
  int i,nb;

  /*
    if(argc !=2)
    {
    printf("pas le bon nombre de parametres\n");
    return 0;
    }
  */
  clo.min.x=CLOMINX;  clo.min.y=CLOMINY;
  clo.max.x=CLOMAXX;  clo.max.y=CLOMAXY; 

  openpl();
  
  fen.bas.c=0;fen.bas.l=0;
  fen.haut.c=largeur();fen.haut.l=hauteur();
  
  calculcoeff(clo,fen);
  
  p1.x=clo.max.x;p1.y=clo.min.y;
  p2.x=clo.min.x;p2.y=clo.max.y;
  tracer(clo.max,p2);
  tracer(clo.min,p2);
  tracer(clo.min,p1);
  tracer(clo.max,p1);

  flushpl();

  printf("fenetre=(%lf,%lf)-(%lf,%lf)\n",clo.min.x,clo.min.y,clo.max.x,clo.max.y);

  printf("coordonnees du point:");
  scanf("%lf %lf",&(M.p.x),&(M.p.y));
  printf("            du vecteur:");
  scanf("%lf %lf",&M.v.x,&M.v.y);
  //printf("p(%.2lf,%.2lf) v(%.2lf,%.2lf)\n",M.p.x,M.p.y,M.v.x,M.v.y);
  
  printf("coordonnees du point:");
  scanf("%lf %lf",&(M2.p.x),&(M2.p.y));
  printf("            du vecteur:");
  scanf("%lf %lf",&M2.v.x,&M2.v.y);
  
  /*allera(M.p);
  
  for(i=0;i<10000;i++)
    {
      suivant(&M,clo);
      //printf("p(%.2f,%.2f) v(%.2f,%.2f) %d\n",M.p.x,M.p.y,M.v.x,M.v.y,i);
      tracervers(M.p);
    }*/
  //trajectoire(M,clo);

  //getchar();
  
  trajectoire2(M,M2,clo);

  getchar();
  
  erase();
  
  serpent(10);
  
  flushpl();

  //affiche(tab,clo);

  getchar();
  closepl();
  
  return 0;
}
