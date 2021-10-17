#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include"3d.h"

#define lgn printf("\n");
#define zz printf("c\'est la?\n")

extern int *coul; //tableau qui a chaque sommet associe une couleur 
          //(grace a la normale des faces adjacentes)

typedef struct app_faces{
  int n;
  struct app_faces *suiv;
} liste_somm_faces;

liste_somm_faces **somm;

extern POINT lumiere;

//int HauteuR, LargeuR;
//int miny,maxy;

void remplit_couleurs(OBJET *obj)
{
  liste_somm_faces *lsf;
  POINT N;
  int k,i,j,debut;

  
  //allocation du tableau des sommets qui apparaissent dans les faces
  somm=(liste_somm_faces **)malloc((obj->ns)*sizeof(liste_somm_faces *));
  //allocation du tableau des normales associees aux sommets
  coul=(int *)malloc((obj->ns)*sizeof(int));
  
  for(i=0;i<obj->ns;i++)
    {
      //initialisation de somm
      somm[i]=(liste_somm_faces *)malloc(sizeof(liste_somm_faces));
      somm[i]->n='\0'; somm[i]->suiv=NULL;
    }
  
  for(i=0;i<obj->nf;i++)
    {
      debut=obj->faso[i+1][0];
      //printf("debut=%d\n",debut);
      somm[debut-1]->n=i+1;
      
      lsf=(liste_somm_faces *)malloc(sizeof(liste_somm_faces));
      
      lsf->suiv=somm[debut-1];
      somm[debut-1]=lsf;
    
      //zz;
      j=1;
      do
	{
	  k=obj->faso[i+1][j];
	  if(k!=debut)
	    {
	      somm[k-1]->n=i+1;
	      lsf=(liste_somm_faces *)malloc(sizeof(liste_somm_faces));
	      lsf->suiv=somm[k-1];
	      somm[k-1]=lsf;
	    }
	  j++;
	}
      while(k!=debut);
    }
  for(i=0;i<obj->ns;i++)
    {
      //printf("le sommet %d apparait ds les faces ",i+1);
      lsf=somm[i]->suiv;
      while(lsf!=NULL)
	{
	  //printf("%d ",lsf->n);
	  lsf=lsf->suiv;
	}
      //printf("\n");
    }

  //zz;
  //remplissage de coul

  //lumiere.x=0;
  //lumiere.y=0;
  //lumiere.z=30;
  lumiere.x=100000.0;
  lumiere.y=100000.0;
  lumiere.z=150000.0;
  lumiere.t=1.0;
  
  openpl();
  for(i=0;i<obj->ns;i++)
    {
      N.x=N.y=N.z=N.t=0;
      coul[i]=0;
      k=0;
      lsf=somm[i]->suiv;
      
      while(lsf!=NULL)
	{
	  //printf("%d\n",lsf->n);
	  /*
	    n=normale2((obj->faso)[lsf->n],obj);
	    N.x+=n.x;
	    N.y+=n.y;
	    N.z+=n.z;
	    N.t+=n.t;
	    printf("N: %.2f %.2f %.2f\n",N.x,N.y,N.z);
	  */
	  k++;
	  //printf("couleur de la face %d %d\n",lsf->n,eclairement((obj->faso)[lsf->n],obj));
	  coul[i]+=eclairement((obj->faso)[lsf->n],obj);
	  
	  lsf=lsf->suiv;
	}
      if(k)
	coul[i]/=k;
    }
  
}

POINT normale2(FACE f, OBJET *obj)
{
  int j,degeneree;
  POINT A,B,C,AB,BC,N;
  double norm;
  
  j=0;
  degeneree=0;

  //printf("\npour la face %d\n",f);
  /*
    A.x=(obj->so)[(obj->faso)[f][j]-1].x;
    A.y=(obj->so)[(obj->faso)[f][j]-1].y;
    A.z=(obj->so)[(obj->faso)[f][j]-1].z;
  */
  A.x=(obj->so)[f[j]-1].x;
  A.y=(obj->so)[f[j]-1].y;
  A.z=(obj->so)[f[j]-1].z;
  //printf("A: %.2f %.2f %.2f\n",A.x,A.y,A.z);
 
  B=A;
  do
    {
      if(f[j]=='\0')
	{
	  printf("zobi\n");
	  degeneree=1;
	  break;
	}
      else
	{
	  j++;
	  B.x=(obj->so)[f[j]-1].x;
	  B.y=(obj->so)[f[j]-1].y;
	  B.z=(obj->so)[f[j]-1].z;
	  //printf("B: %.2f %.2f %.2f\n",B.x,B.y,B.z);
	  AB.x=B.x-A.x;
	  AB.y=B.y-A.y;
	  AB.z=B.z-A.z;
	  
	  //printf("AB: %.2f %.2f %.2f\n",AB.x,AB.y,AB.z);
	}
    }
  while((B.x==A.x) && (B.y==A.y) && (B.y==A.y));
  
  if(degeneree==0)
    {
      C=B;
      do
	{
	  if(f[j]=='\0')
	    {
	      degeneree=1;
	      break;
	    }
	  j++;
	  C.x=(obj->so)[f[j]-1].x;
	  C.y=(obj->so)[f[j]-1].y;
	  C.z=(obj->so)[f[j]-1].z;
	  
	  //printf("C: %.2f %.2f %.2f\n",C.x,C.y,C.z);
	  
	  BC.x=C.x-B.x;
	  BC.y=C.y-B.y;
	  BC.z=C.z-B.z;

	  //printf("BC: %.2f %.2f %.2f\n",BC.x,BC.y,BC.z);
	  
	  N.x=AB.y*BC.z-AB.z*BC.y;
	  N.y=AB.z*BC.x-AB.x*BC.z;
	  N.z=AB.x*BC.y-AB.y*BC.x;


	  //printf("N: %.2f %.2f %.2f\n",N.x,N.y,N.z);

	  //u=AB.y-BC.z-AB.z-BC.y;
	  //v=AB.z-BC.x-AB.x-BC.z;
	  //w=AB.x-BC.y-AB.y-BC.x;
	  
	  norm=sqrt(N.x*N.x+N.y*N.y+N.z*N.z);
	  //norm=sqrt(u*u+v*v+w+w);
	}
      while(norm*norm==0);
      
      //eventuellement :   u=u/norm ; v=v/norm ; w=w/norm ;
      
      //h= -u*A.x -v*A.y -w*A.z;  
      N.t= -N.x*A.x -N.y*A.y -N.z*A.z;     
    }

  //printf("normale de %d : %.2f %.2f %.2f\n",f,N.x,N.y,N.z);
  //if(ok==1)
    //printf("normale : %.2lf %.2lf %.2lf %.2lf\n",N.x,N.y,N.z,N.t);
  
  return N;
}

POINT normale(FACE f, OBJET *obj)
{
  int j,degeneree;
  POINT A,B,C,AB,BC,N;
  double norm;
  
  j=0;
  degeneree=0;

  //printf("\npour la face %d\n",f);
  /*
    A.x=(obj->so)[(obj->faso)[f][j]-1].x;
    A.y=(obj->so)[(obj->faso)[f][j]-1].y;
    A.z=(obj->so)[(obj->faso)[f][j]-1].z;
  */
  A.x=(obj->tso)[f[j]-1].x;
  A.y=(obj->tso)[f[j]-1].y;
  A.z=(obj->tso)[f[j]-1].z;
  //printf("A: %.2f %.2f %.2f\n",A.x,A.y,A.z);
 
  B=A;
  do
    {
      if(f[j]=='\0')
	{
	  printf("zobi\n");
	  degeneree=1;
	  break;
	}
      else
	{
	  j++;
	  B.x=(obj->tso)[f[j]-1].x;
	  B.y=(obj->tso)[f[j]-1].y;
	  B.z=(obj->tso)[f[j]-1].z;
	  //printf("B: %.2f %.2f %.2f\n",B.x,B.y,B.z);
	  AB.x=B.x-A.x;
	  AB.y=B.y-A.y;
	  AB.z=B.z-A.z;
	  
	  //printf("AB: %.2f %.2f %.2f\n",AB.x,AB.y,AB.z);
	}
    }
  while((B.x==A.x) && (B.y==A.y) && (B.y==A.y));
  
  if(degeneree==0)
    {
      C=B;
      
      do
	{
	  if(f[j]=='\0')
	    {
	      degeneree=1;
	      break;
	    }
	  j++;
	  C.x=(obj->tso)[f[j]-1].x;
	  C.y=(obj->tso)[f[j]-1].y;
	  C.z=(obj->tso)[f[j]-1].z;
	  
	  //printf("C: %.2f %.2f %.2f\n",C.x,C.y,C.z);
	  
	  BC.x=C.x-B.x;
	  BC.y=C.y-B.y;
	  BC.z=C.z-B.z;

	  //printf("BC: %.2f %.2f %.2f\n",BC.x,BC.y,BC.z);
	  
	  N.x=AB.y*BC.z-AB.z*BC.y;
	  N.y=AB.z*BC.x-AB.x*BC.z;
	  N.z=AB.x*BC.y-AB.y*BC.x;


	  //printf("N: %.2f %.2f %.2f\n",N.x,N.y,N.z);

	  //u=AB.y-BC.z-AB.z-BC.y;
	  //v=AB.z-BC.x-AB.x-BC.z;
	  //w=AB.x-BC.y-AB.y-BC.x;
	  
	  norm=sqrt(N.x*N.x+N.y*N.y+N.z*N.z);
	  //norm=sqrt(u*u+v*v+w+w);
	}
      while(norm*norm==0);
      
      //eventuellement :   u=u/norm ; v=v/norm ; w=w/norm ;
      
      //h= -u*A.x -v*A.y -w*A.z;  
      N.t= -N.x*A.x -N.y*A.y -N.z*A.z;     
    }

  //printf("normale de %d : %.2f %.2f %.2f\n",f,N.x,N.y,N.z);
  //if(ok==1)
    //printf("normale : %.2lf %.2lf %.2lf %.2lf\n",N.x,N.y,N.z,N.t);
  
  return N;
}

double pscal3d2(POINT u,POINT v)
{
  return u.x*v.x+u.y*v.y+u.z*v.z;
}

double norme3d2(POINT u)
{
  return sqrt(u.x*u.x+u.y*u.y+u.z*u.z);
}

int eclairement(FACE f,OBJET *obj)
{
  double a; //cosinus de l'angle entre le rayon et la normale de la face
  POINT N,R;

  
  N=normale2(f,obj);


  //printf("N: %.2f %.2f %.2f\n",N.x,N.y,N.z);

  //printf("lumiere: %.2f %.2f %.2f\n",lumiere.x,lumiere.y,lumiere.z);

  //printf("A: %.2f %.2f %.2f\n",(obj->so)[(obj->faso)[f][0]].x,(obj->so)[(obj->faso)[f][0]].y,(obj->so)[(obj->faso)[f][0]].z);
  
  /*
    R.x=lumiere.x-(obj->so)[(obj->faso)[f][0]].x;
    R.y=lumiere.y-(obj->so)[(obj->faso)[f][0]].y;
    R.z=lumiere.z-(obj->so)[(obj->faso)[f][0]].z;
  */
  R.x=lumiere.x-(obj->tso)[f[0]].x;
  R.y=lumiere.y-(obj->tso)[f[0]].y;
  R.z=lumiere.z-(obj->tso)[f[0]].z;
  
  //printf("R: %.2f %.2f %.2f\n",R.x,R.y,R.z);
  
  //printf("ps=%.2f n=%.2f\n",pscal3d(N,R),norme3d(N)*norme3d(R));
  
  a=(pscal3d2(N,R))/((norme3d2(N)*norme3d2(R)));
  
  if(a<0)
    //return nb_gris()-1;
    a=-a;
  //printf("a=%.2f\n",acos(a)*180/M_PI);
  //printf("a=%.2f\n",a);

  a=1-a;  
  //printf("eclairement: %d sur %d\n",(int)(a*nb_gris()),nb_gris());
  //return nb_gris()-(int)(a*nb_gris()+1);
  return (int)( ((double)nb_gris()-1.0)*((double)a/(double)M_PI_2) );
}
