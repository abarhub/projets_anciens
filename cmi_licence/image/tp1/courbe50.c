#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include"2d.h"
//#include"lgx.c"

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

void tracer_cloture(CLOTURE clo)
{
  POINT2D p1,p2;

  p1.x=clo.max.x;p1.y=clo.min.y;
  p2.x=clo.min.x;p2.y=clo.max.y;
  tracer(clo.max,p2);
  tracer(clo.min,p2);
  tracer(clo.min,p1);
  tracer(clo.max,p1);
}

void trajectoire(MOBILE M, CLOTURE clo)
{
  MOBILE M2=M;
  
  allera(M.p);
  
  do
    {
      suivant(&M,clo);
      //printf("p(%.2f,%.2f) v(%.2f,%.2f) %d\n",M.p.x,M.p.y,M.v.x,M.v.y,i);
      tracervers(M.p);
      flushpl();
    }
  while( (M2.p.x!=M.p.x) && (M2.p.y!=M.p.y) && (M2.v.x!=M.v.x) && (M2.v.y!=M.v.y));
  flushpl();
}

void trajectoire2(int n, CLOTURE clo)
{
  MOBILE *M;
  int i;
  double t1,t2;

  M=(MOBILE*)calloc(n,sizeof(MOBILE));

  for(i=0;i<n;i++)
    {
      M[i].p.x=(rand()%(CLOMAXX-CLOMINX))+CLOMINX;
      M[i].p.y=(rand()%(CLOMAXY-CLOMINY))+CLOMINY;
      M[i].v.x=(float)(rand()%(CLOMAXX-CLOMINX))/100;
      M[i].v.y=(float)(rand()%(CLOMAXY-CLOMINY))/100;
    
      //printf("%d (%.2lf,%.2lf) (%.2lf,%.2lf)\n",i,M[i].p.x,M[i].p.y,M[i].v.x,M[i].v.y);
    }

  flushpl();
  do
    {
      tracer_cloture(clo);
      for(i=0;i<n;i++)
	{
	  suivant(&M[i],clo);
	}

      for(i=0;i<n-1;i++)
	{
	  tracer(M[i].p,M[i+1].p);
	}
      tracer(M[i].p,M[0].p);
      
      flushpl();

      t1=clock();
      while(clock()-t1<10000);
      

      erase();
      
    }
  while(1);// (M3.p.x!=M.p.x) && (M3.p.y!=M.p.y) );//&& (M3.v.x!=M.v.x) && (M3.v.y!=M.v.y));
  flushpl();
  free(M);
}

void casteljau(POINT2D M[], int dim, double t)
{
  int i,j;
  POINT2D p1,p2,p3,p4;

  if(t<0 || t>1)
    {
      printf("t doit etre compris entre 0 et 1\n");
      return ;
    }
  for(i=1;i<dim;i++)
    {
      p1=M[0];
      p4=M[dim-i];

      for(j=0;j<dim-i;j++)
	{
	  M[j].x=(1-t)*M[j].x+t*M[j+1].x;
	  M[j].y=(1-t)*M[j].y+t*M[j+1].y;
	  //printf("M[%d,%d](%d):%.0lf,%.0lf\n",j,j+i,j,M[j].x,M[j].y);
	}

      p2=M[0];
      p3=M[dim-i-1];

      tracer(p1,p2);
      //flushpl();
      //sleep(1);
      tracer(p3,p4);
      flushpl();
      //sleep(1);
    }
}

void casteljau2(POINT2D M[], int dim, double t)
{
  int i,j;
  POINT2D p1,p2,p3,p4;

  if(t<0 || t>1)
    {
      printf("t doit etre compris entre 0 et 1\n");
      return ;
    }
  for(i=1;i<dim;i++)
    {
      p1=M[0];
      p4=M[dim-i];

      for(j=0;j<dim-i;j++)
	{
	  M[j].x=(1-t)*M[j].x+t*M[j+1].x;
	  M[j].y=(1-t)*M[j].y+t*M[j+1].y;
	  //printf("M[%d,%d](%d):%.0lf,%.0lf\n",j,j+i,j,M[j].x,M[j].y);
	}

      p2=M[0];
      p3=M[dim-i-1];

      tracer(p1,p2);
      //flushpl();
      //sleep(1);
      tracer(p3,p4);
      flushpl();
      //sleep(1);
    }
}
void serpent(int n, double t, CLOTURE clo)
{
  MOBILE *M, *M2;
  int i;
  double t1;
  
  M=(MOBILE*)calloc(n,sizeof(MOBILE));
  M2=(MOBILE*)calloc(n,sizeof(MOBILE));
  for(i=0;i<n;i++)
    {
      M[i].p.x=(rand()%(CLOMAXX-CLOMINX))+CLOMINX;
      M[i].p.y=(rand()%(CLOMAXY-CLOMINY))+CLOMINY;
      M[i].v.x=(float)(rand()%(CLOMAXX-CLOMINX))/100;
      M[i].v.y=(float)(rand()%(CLOMAXY-CLOMINY))/100;
    }

  flushpl();
  do
    {
      tracer_cloture(clo);
      for(i=0;i<n;i++)
	{
	  suivant(&M[i],clo);
	}
      
      for(i=0;i<n-1;i++)
	{
	  M2[i]=M[i];
	  //tracer(M[i].p,M[i+1].p);
	} 
      //tracer(M[i].p,M[0].p);
      
//      casteljau(M2,n,t);
      flushpl();

      t1=clock();
      while(clock()-t1<100000);

      erase();
    }
  while(1);// (M3.p.x!=M.p.x) && (M3.p.y!=M.p.y) );//&& (M3.v.x!=M.v.x) && (M3.v.y!=M.v.y));
  flushpl();
  free(M);
}

int main(int argc, char **argv)
{
  POINT2D /*tab[NB_POINT],*/*point;
  CLOTURE clo;
  FENETRE fen;
  MOBILE M,M2,M3;
  int i,nb,n;

  /*
    if(argc !=2)
    {
    printf("pas le bon nombre de parametres\n");
    return 0;
    }
  */
  clo.min.x=CLOMINX;  clo.min.y=CLOMINY;
  clo.max.x=CLOMAXX;  clo.max.y=CLOMAXY; 

  printf("--------------------\nnombre de mobiles ");
  scanf("%d",&n);

  openpl();
  
  fen.bas.c=0;fen.bas.l=0;
  fen.haut.c=largeur();fen.haut.l=hauteur();
  
  calculcoeff(clo,fen);
  
  color(10);
  tracer_cloture(clo);

  flushpl();
  getchar();
  
  /*
  printf("coordonnees du point:");
  scanf("%lf %lf",&(M.p.x),&(M.p.y));
  printf("            du vecteur:");
  scanf("%lf %lf",&M.v.x,&M.v.y);
  //printf("p(%.2lf,%.2lf) v(%.2lf,%.2lf)\n",M.p.x,M.p.y,M.v.x,M.v.y);
  
  printf("coordonnees du point:");
  scanf("%lf %lf",&(M2.p.x),&(M2.p.y));
  printf("            du vecteur:");
  scanf("%lf %lf",&M2.v.x,&M2.v.y);
  
  M.p.x=1;
  M.p.y=1;
  M.v.x=.2;
  M.v.y=-.3;
  
  M2.p.x=50;
  M2.p.y=50;
  M2.v.x=.2;
  M2.v.y=.1;
  */





  /*allera(M.p);
  
  for(i=0;i<10000;i++)
    {
      suivant(&M,clo);
      //printf("p(%.2f,%.2f) v(%.2f,%.2f) %d\n",M.p.x,M.p.y,M.v.x,M.v.y,i);
      tracervers(M.p);
    }*/
  //trajectoire(M,clo);

  //getchar();
  
  printf("il faut faire ^C pour arreter\n");
  
  trajectoire2(n,clo);
  
  getchar();
  
  erase();
  
  serpent(3,0.5,clo);
  
  flushpl();

  //affiche(tab,clo);

  getchar();
  closepl();
  
  return 0;
}
