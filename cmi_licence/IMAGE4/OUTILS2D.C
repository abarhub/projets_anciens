#include"2d.h"
#include<math.h>

static double dc,dl,k;
static double dx,dy,K;

void calculcoeff(CLOTURE clo,FENETRE fen)
{
 double k1,k2,xc,yc;
 int co,lo;
 
 k1=(fen.haut.l-fen.bas.l)/(clo.max.y-clo.min.y);
 k2=(fen.haut.c-fen.bas.c)/(clo.max.x-clo.min.x);
 k=(k1>=k2)?k2:k1;
 
 xc=(clo.max.x+clo.min.x)/2;
 yc=(clo.max.y+clo.min.y)/2;
 co=(fen.haut.c+fen.bas.c)/2;
 lo=(fen.haut.l+fen.bas.l)/2;
 dc=co-k*xc;
 dl=lo-k*yc;
}

void invcoeff()
{
  K=1/k;
  dx=-K*dc;
  dy=-K*dl;
}

POINT2D pixtop(int c, int l)
{
  POINT2D p;
  p.x=K*c+dx;
  p.y=K*l+dy;
  return p;
}

PIXEL ptopix(POINT2D m)
{
 PIXEL p;
 p.c=k*m.x+dc;
 p.l=k*m.y+dl;
 
 return p;
}

void placer(POINT2D m)
{
 PIXEL p;
 p=ptopix(m);
 point(p.c,p.l);
}

void placerptcourant(void)
{
 dot();
}

void tracer(POINT2D m1,POINT2D m2)
{
 PIXEL p1,p2;
 p1=ptopix(m1);
 p2=ptopix(m2);
 line(p1.c,p1.l,p2.c,p2.l);
}

void polygone(POINT2D tab[],int taille)
{
  int i,*p;
  //POINT2D point;
  PIXEL pixel;
  int tab2[100];
  
  for(i=0;i<taille;i++)
    {
      //point.x=tab[i].x;
      //point.y=tab[i].y;
      pixel=ptopix(tab[i]);
      tab2[2*i]=pixel.c;
      tab2[2*i+1]=pixel.l;
    }
  polygon(tab2,taille*2);
}

void allera(POINT2D m)
{
 PIXEL p;
 p=ptopix(m);
 move(p.c,p.l);
}

void tracervers(POINT2D m)
{
 PIXEL p;
 p=ptopix(m);
 cont(p.c,p.l);
}

int code(POINT2D m, CLOTURE clo)
{
 int res=0;
 
 res+=(m.x<clo.min.x);
 res+=(m.x>clo.max.x)<<1;
 res+=(m.y<clo.min.y)<<2;
 res+=(m.y>clo.max.y)<<3;
 return res;
}

int mauvais(int mcode, int i)
{
 return (1<<(i-1) & mcode);
}

POINT2D intersection(POINT2D a, POINT2D b, int i, CLOTURE clo)
{
 POINT2D pt;
 int t;
 
 if(i<3)
 {
  pt.x=((i==1) ? clo.min.x : clo.max.x);
  t=(pt.x-a.x)/(b.x-a.x);
  pt.y=(1-t)*a.y+t*b.y;
 }
 else
 {
  pt.y=((i==3) ? clo.min.y : clo.max.y);
  t=(pt.y-a.y)/(b.y-a.y);
  pt.x=(1-t)*a.x+t*b.x;
 }
 return pt;  
}


int cs(POINT2D *a, POINT2D *b,CLOTURE clo)
{
 int ca,cb,res;
 double tmp;
 
 printf("a:(%.2lf,%.2lf) b:(%.2lf,%.2lf)\n",a->x,a->y,b->x,b->y);
 ca=code(*a,clo);cb=code(*b,clo);
 printf("ca=%d cb=%d\n",ca,cb);
 while((ca+cb)&&!(ca&cb))
 {
  if(ca==0)
  {
   tmp=a->x;
   a->x=b->x;
   b->x=tmp;
   tmp=a->y;
   a->y=b->y;
   b->y=tmp;
  }
  else
  {
   if(mauvais(ca,1))
   { 
    *a=intersection(*a,*b,1,clo);
   }
   if(mauvais(ca,2))
   {
    *a=intersection(*a,*b,2,clo);
   }
   if(mauvais(ca,3))
   {
    *a=intersection(*a,*b,3,clo);
   }
   if(mauvais(ca,4))
   {
    *a=intersection(*a,*b,4,clo);
   }
  }
  ca=code(*a,clo);cb=code(*b,clo);
 }
 
 if(ca+cb==0)
  return 1;
 else
  return 0;
}

double norme(VECTEUR2D u)
{
 return sqrt(u.x*u.x+u.y*u.y);
}

double det(VECTEUR2D u,VECTEUR2D v)
{
 return u.x*v.y-u.y*v.x;
}

double pscal(VECTEUR2D u,VECTEUR2D v)
{
 return u.x*v.x+u.y*v.y;
}

double angle(VECTEUR2D u,VECTEUR2D v)
{
 double x=norme(u)*norme(v)+pscal(u,v);
 if(abs(x)<1e-5) return M_PI;
 else return (2*atan(det(u,v)/x));
}

POINT2D rotation(POINT2D c, double theta, POINT2D p)
{
 POINT2D q;
 double x,y;
 
 x=p.x-c.x;y=p.y-c.y;
 q.x=x*cos(theta)-y*sin(theta);
 q.y=x*sin(theta)+y*cos(theta);
 q.x+=c.x;q.y+=c.y;
 return q;
}

POINT2D bezier(POINT2D control[],int no,double t)
{

}

