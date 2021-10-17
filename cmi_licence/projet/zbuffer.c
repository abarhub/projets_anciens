#include<math.h>
#include<stdlib.h>
#include"3d.h"


#define M_INF -16000

static int ok=0;

static double *Z;
struct point_coul{
  int px,c;
};
struct point_coul *x_g;
struct point_coul *x_d;

POINT lumiere;

int HauteuR, LargeuR;
int miny,maxy;

int *coul;

void remplit_limites_g_d(int x1,
	     int y1, int x2,int y2, int c1, int c2)
{
  int temp,c;
  int y;
  //register int y;
  long x,pas;
  //register long x,pas;
  
  //printf("(%d,%d) (%d,%d)\n",x1,y1,x2,y2);

  if(y2!=y1)
    {
      if(y2<y1)
	{
	  temp=y1; y1=y2; y2=temp;
	  temp=x1; x1=x2; x2=temp;
	  temp=c1; c1=c2; c2=temp;
	}
      
      x  = x1<<8;
      pas= ((x2-x1)<<8)/(y2-y1);
      
      x+=pas;
      y1++;
      
      if(y1<miny) miny=y1;
      if(y2>maxy) maxy=y2;
      
      for(y=y1;y<=y2;y++)
	{
	  if((y>=0) && (y<HauteuR))
	    {
	      c=(int)(c1-((double)(c1-c2)/(double)(y2-y1))*(double)(y-y1));
	      if(x_g[y].px==M_INF)
		{
		  x_g[y].px=x>>8;
		  if(y2-y1)
		    x_g[y].c=c;
		}
	      else 
		{
		  x_d[y].px=x>>8;
		  if(y2-y1)
		    x_d[y].c=c;
		}
	    }
	  x+=pas;
	}
    }
}

void remplit_poly(FACE f, OBJET *obj)
{
  int j,debut,g,d;
  int i,absc;
  //register int i,absc;
  
  int temp;
  POINT n;
  double z,deltax;

  POINT2D m1,m2,m;
  PIXEL p1,p2,p;

  miny=HauteuR;
  maxy=0;
  
  j=0;

  debut=f[j];
  
  do
    {
      m1.x=(obj->tso)[f[j]-1].x;
      m1.y=(obj->tso)[f[j]-1].y;
      m2.x=(obj->tso)[f[j+1]-1].x;
      m2.y=(obj->tso)[f[j+1]-1].y;

      //printf("(%.2lf,%.2lf) (%.2lf,%.2lf)\n",m1.x,m1.y,m2.x,m2.y);
      
      p1=ptopix(m1);
      p2=ptopix(m2);


      //printf("1(%d,%d) (%d,%d)\n",p1.c,p1.l,p2.c,p2.l);

      //p1.c=(p1.c*(LargeuR/(float)largeur()));
      p1.c*=(LargeuR/(double)largeur());
      p1.l*=(HauteuR/(double)hauteur());
      p2.c*=(LargeuR/(double)largeur());
      p2.l*=(HauteuR/(double)hauteur());


  //printf("2(%d,%d) (%d,%d)\n",p1.c,p1.l,p2.c,p2.l);

      //printf("%d %d\n",coul[f[j]-1],coul[f[j+1]-1]);
      remplit_limites_g_d(p1.c,p1.l,p2.c,p2.l,coul[f[j]-1],coul[f[j+1]-1]);
      j++;
    }
  while(f[j]!=debut);

  //printf("fin face\n");

  color(eclairement(f,obj));
  if(miny<0) miny=0;
  if(maxy>=HauteuR)
    maxy=HauteuR-1;
  
  //printf("miny=%d maxy=%d\n",miny,maxy);

  n=normale(f,obj);

  for(i=miny;i<=maxy;i++)
    {
      ok=0;
      if(i==577)
	ok=1;

      //n=normale(f,obj);
      	        
      if(x_d[i].px==M_INF)
	{
	  p.c=x_g[i].px*(largeur()/LargeuR);
	  p.l=i*(hauteur()/HauteuR);
	  m=pixtop2(p);
	  z=-(n.x*m.x + n.y*m.y + n.t)/n.z;
	  if(z>Z[x_g[i].px*HauteuR+i])
	    {
	      color(x_g[i].c);
	      //printf("on allume %d %d\n",x_g[i],i);
	      //point(x_g[i],i);
	      point(x_g[i].px*largeur()/LargeuR,i*hauteur()/HauteuR);
	      Z[x_g[i].px*HauteuR+i]=z;
	    }
	  //point(x_g[i],i);
	}
      else
	{
	  //g= (x_g[i].px<x_d[i] ? x_g[i] : x_d[i]);
	  //d= (x_g[i]<x_d[i] ? x_d[i] : x_g[i]);
	  
	  if(x_g[i].px<x_d[i].px)
	    {
	      g=x_g[i].px;
	      d=x_d[i].px;
	    }
	  else
	    {
	      g=x_d[i].px;
	      d=x_g[i].px;
	      
	      temp=x_d[i].c;
	      x_d[i].c=x_g[i].c;
	      x_g[i].c=temp;
	    }
	  
	  //printf("g=%d d=%d\n",g,d);
	  //gp=(int)(g*1024/6920);
	  //dp=d*1024/LargeuR;
	  //printf("g'=%d d'=%d\n",gp,dp);
	  
	  //t_pix=(int *)realloc(t_pix,2*(d-g)*sizeof(int)
	  
	  p.c=g*(largeur()/LargeuR);
	  p.l=i*(hauteur()/HauteuR);
	  
	  m=pixtop2(p);
	  
	  deltax=m.x;
	  z=-(n.x*m.x + n.y*m.y + n.t)/n.z;	
	  p.c=(g+1)*(largeur()/LargeuR);
	  m=pixtop2(p);
	  deltax=m.x-deltax;
	  
	  for(absc=g;absc<d;absc++)
	    {
	      if(absc>g)
		  z-=((n.x/n.z)*deltax);
	      //if(i==(int)((maxy-miny)/(double)2) && absc==(int)((d-g)/(double)2))
	      //if(i==577){
		//printf("z=%.2lf\n",z);
	      //printf("z=%.2f Z=%.2f\n",z,Z[absc*HauteuR+i]);}
	      //printf("z=%.2f Z[%d,%d]=%.2f\n",
	      //     z,(int)absc*4510/768,(int)i*6920/1024,
	      //     Z[absc*4510+i*6920/1024]);
	      //exit(0);
	      if(z>Z[absc*HauteuR+i])
		{
		  color((int)(x_g[i].c-
		       ((double)(x_g[i].c-x_d[i].c)
		         /(double)(d-g))*(double)(absc-g)));
		  //printf("on allume %d %d\n",absc,i);
		  //point(absc,i);
		 
		  point(absc*largeur()/LargeuR,i*hauteur()/HauteuR);
		  //flushpl();
		  Z[absc*HauteuR+i]=z;
		}
	    }
	  flushpl();

	     
	  //line(g,i,d,i);
	  //line(x_g[i],i,x_d[i],i);
	}
      x_g[i].px=x_d[i].px=M_INF;
    }
}

void z_buffer(OBJET *obj)
{
  int i,j;
  static int deja=0;

  HauteuR=(int)(hauteur()/(double)5);
  LargeuR=(int)(largeur()/(double)5);
  
  invcoeff();
  erase();
  flushpl();

  if(!deja)
    {
      deja=1;
      Z=(double *)malloc(((HauteuR+1)*(LargeuR+1))*sizeof(double));
      x_g=(struct point_coul *)malloc(LargeuR*sizeof(struct point_coul));
      x_d=(struct point_coul *)malloc(LargeuR*sizeof(struct point_coul));
    }
  //initialisation des tableaux des x 
  for(i=0;i<LargeuR;i++)
    x_g[i].px=x_d[i].px=M_INF;

  //initialisation de la matrice des z
  for(i=0;i<LargeuR;i++)
    for(j=0;j<HauteuR;j++)
      //if(Z[i*HauteuR+j]!=M_INF)
	Z[i*HauteuR+j]=M_INF;
  
  for(i=1;i<=(obj->nf);i++)
    remplit_poly((obj->faso)[i],obj);

}   
