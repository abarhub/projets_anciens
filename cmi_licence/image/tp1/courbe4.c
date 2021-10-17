#include<math.h>
#include<stdlib.h>
#include<stdio.h>
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

POINT2D point_castel(POINT2D m1,POINT2D m2,double t)
{
	POINT2D tmp;
	
	tmp.x=(1-t)*m1.x+t*m2.x;
	tmp.y=(1-t)*m1.y+t*m2.y;
	
	return tmp;
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
			printf("M[%d,%d]:%lf,%lf\n",j,j+i,M[j].x,M[j].y);
			
		}
	}

}

int lire_pt_controle(POINT2D M[],char *nom_fichier)
{
	FILE *fd;
	int i=0;
	double x,y;
	char s[50];
	
	fd=fopen(nom_fichier,"r");
	if(fd==NULL)
	{
		printf("impossible d'ouvrir le fichier %s\n",nom_fichier);
		return -1;
	}
	while(fgets(s,50,fd)!=NULL)
	{
		sscanf(s,"%lf,%lf",&x,&y);
		M[i].x=x;M[i].y=y;
		i++;
	}
	
	fclose(fd);
	
	return i;
}

void polygone(POINT2D a,POINT2D p,int n)
{
	double pas=2*M_PI/n;
	POINT2D m;
	int i;
	
	allera(p);
	for(i=1;i<n;i++)
	{
		m=rotation(a,pas*i,p);
		tracervers(m);
	}
	tracervers(p);
	
}

void polygone2(POINT2D a,POINT2D p,int n,int nb,double ld)
{
	double pas=2*M_PI/n;
	POINT2D m;
	int i;
	POINT2D tab[100];
	
	
	
	for(i=0;i<nb;i++)
	{
		polygone(a,p,n);
		m=rotation(a,pas,p);
		
		p.x=(1-ld)*p.x+ld*m.x;
		p.y=(1-ld)*p.y+ld*m.y;
	}
}

int main(int argc, char **argv)
{
	POINT2D tab[NB_POINT],p1,p2;
	CLOTURE clo;
	FENETRE fen;
	int i,nb,nb_cote,nb_poly;
	double nb_ld;
	
	if(argc>=2)
	{
		nb_cote=atoi(argv[1]);
	}
	else nb_cote=6;
	
	if(argc>=3)
	{
		nb_poly=atoi(argv[2]);
	}
	else nb_poly=3;
	
	if(argc>=4)
	{
		nb_ld=atof(argv[3]);
	}
	else nb_ld=1.0/6;
	
	openpl();
	
	fen.bas.c=0;fen.bas.l=0;
	fen.haut.c=largeur();fen.haut.l=hauteur();
	
	clo.min.x=-largeur()/2;clo.min.y=-hauteur()/2;
	clo.max.x=largeur()/2;clo.max.y=hauteur()/2;
	
	calculcoeff(clo,fen);
	p1.x=3/4*largeur();p1.y=clo.max.y;
	p2.x=0;p2.y=0;

	polygone2(p2,p1,nb_cote,nb_poly,nb_ld);
	
	flushpl();
	getchar();
	
	
	closepl();
	
	return 0;
}
