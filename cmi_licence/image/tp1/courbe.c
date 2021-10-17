#include<math.h>
#include<stdlib.h>
#include"2d.h"

/* doit etre pair */
#define NB_POINT (1000)

void calcultab(float a,float b,int n,POINT2D tab[],CLOTURE *clo)
{
	double h=2*M_PI/n,t=0.0,xmin,xmax,ymin,ymax,x,y;
	int i;
	
	if(clo==NULL)
	{
		printf("Erreur: appel de calcultab avec clo a NULL");
	}
	xmin=xmax=/*tab[0].x=*/1;
	ymin=ymax=/*tab[0].y=*/0;
	
	for(i=0;i<=n;i++)
	{
		/*t=t+h;*/
		tab[i].x=x=cos(a*t);
		tab[i].y=y=sin(b*t);
		
		if(x<xmin) xmin=x;
		else if(x>xmax) xmax=x;
		
		if(y<ymin) ymin=y;
		else if(y>ymax) ymax=y;
		t=t+h;
	}
	clo->min.x=xmin;clo->max.x=xmax;
	clo->min.y=ymin;clo->max.y=ymax;
}

void affiche(POINT2D tab[], CLOTURE clo)
{
	int i;
	POINT2D pt,pt2;
	
	printf("cloture= min:(%.2lf,%.2lf) max:(%.2lf,%.2lf)\n",clo.min.x,clo.min.y,clo.max.x,clo.max.y);
	
	for(i=0;i<NB_POINT;i++)
	{
		if(i==0)
			//placer(tab[0]);
			allera(tab[0]);
		else
		{
			pt=tab[i];
			pt2=tab[i-1];
			//tracervers(tab[i]);
			if(cs(&(pt),&(pt2),clo))
				tracer(pt,pt2);
		//printf("(%lf,%lf)=(%d,%d)\n",tab[i].x,tab[i].y,ptopix(tab[i]).c,ptopix(tab[i]).l);
		}
	}
	pt=tab[0];
	pt2=tab[NB_POINT-1];
	if(cs(&(pt),&(pt2),clo))
		tracer(pt,pt2);
	
	flushpl();
}

int main()
{
	POINT2D tab[NB_POINT],*point,p1,p2;
	CLOTURE clo;
	FENETRE fen;
	int i;
	
	
	/*printf("avant\n");
	for(i=0;i<NB_POINT;i++)
		printf("(%lf,%lf)\n",tab[i].x,tab[i].y);*/
	
	calcultab(1,1,NB_POINT,tab,&clo);
	
	
	clo.min.x=clo.min.x*4/5;clo.min.y=clo.min.y*4/5;
	clo.max.x=clo.max.x*4/5;clo.max.y=clo.max.y*4/5;
	
	
	/*printf("apres\n");
	for(i=0;i<NB_POINT;i++)
		printf("(%lf,%lf)\n",tab[i].x,tab[i].y);*/
	
	openpl();
	
	fen.bas.c=/*largeur()*1.0/4*/0;fen.bas.l=/*largeur()*1.0/4*/0;
	fen.haut.c=largeur()/**3.0/4*/;fen.haut.l=hauteur()/**3.0/4*/;
	
	calculcoeff(clo,fen);
	
	p1.x=clo.max.x;p1.y=clo.min.y;
	p2.x=clo.min.x;p2.y=clo.max.y;
	tracer(clo.max,p2);
	tracer(clo.min,p2);
	tracer(clo.min,p1);
	tracer(clo.max,p1);
	
	/*erase();*/
	/*printf("fin%lf,%lf\n",clo.min.x,clo.min.y);*/
	
	affiche(tab,clo);
	
	//sleep(10);
	getchar();
	closepl();
	
	return 0;
}
