#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"3d.h"
#include"debug.h"

// une face
typedef struct FACE2 
{
  double x0,y0,x1,y1;
  double zmin,zmax;
  POINT *sommets;
  int no_face,nb_sommets;
  int couleur;
} FACE2;

#define PRECIS (1.0/1000.0)

#define PRECISION (10000.0)

double pscal3d(POINT u,POINT v);
POINT equ_plan(FACE2 *face);
POINT pvect3d(POINT u,POINT v);
extern double xo,yo,zo;
void affiche_faces(FACE2 *liste,int nb);

// retourne la difference entre les zmin
// utilise par qsort en considerant
// la precision PRECISION
int cmp(const void *a,const void *b)
{
  return (int)(( ((FACE2*)a)->zmin *PRECISION) - 
	       ( ((FACE2*)b)->zmin *PRECISION));
}

// calcul la transformation du point
void transforme_point(POINT *p)
{
  extern int perspective;
  
  if(perspective==1)
    {
      *p=mulmavec(Mt,*p);
      retour_cartesiennes(p);
    }
}

// calcul les projetes des faces
void initialiser_affichage2(OBJET *objet,FACE2 *liste_faces,int taille)
{
  int i,j;
  extern int perspective;

  if(perspective==1)
    {
      for(i=0;i<taille;i++)
	{
	  for(j=0;j<liste_faces[i].nb_sommets;j++)
	    {
	      transforme_point(&(liste_faces[i].sommets[j]));
	    }
	}
    }
}

// trie le tableau face avec taille elements
void trie(FACE2 *face,int taille)
{
  qsort(face,taille,sizeof(FACE2),cmp);
}

// retourne la norme du vecteur p sans prendre en compte t
double norme3d(POINT *p)
{
  return sqrt(p->x*p->x+p->y*p->y+p->z*p->z);
}

// affiche la face f
void trace_face(OBJET *objet,FACE2 *f)
{
  int i;
  POINT2D *p4;
  
  // selection de la couleur
  color(f->couleur);
  
  // affichage d'un poly plein
  p4=calloc(f->nb_sommets,sizeof(POINT2D));
  for(i=0;i<f->nb_sommets;i++)
    {
      p4[i].x=f->sommets[i].x;
      p4[i].y=f->sommets[i].y;
    }
  polygone(p4,f->nb_sommets);
  free(p4);
}

/* return 1 si les rectangles englobants les
   faces f1 et f2 sont disjoint
   et 0 sinon*/
int disjoint(FACE2 *f1,FACE2 *f2)
{
  if((f1->x0<f2->x0)&&(f2->x0<f1->x1))
    {// intersection
      return 0;
    }
  
  if((f1->x0<f2->x1)&&(f2->x1<f1->x1))
    {// intersection
      return 0;
    }

  if((f1->y0<f2->y0)&&(f2->y0<f1->y1))
    { // intersection
      return 0;
    }
  
  if((f1->y0<f2->y1)&&(f2->y1<f1->y1))
    {// intersection
      return 0;
    }
  
  if((f2->x0<f1->x0)&&(f1->x0<f2->x1))
    {// intersection
      return 0;
    }
  
  if((f2->x0<f1->x1)&&(f1->x1<f2->x1))
    {// intersection
      return 0;
    }

  if((f2->y0<f1->y0)&&(f1->y0<f2->y1))
    { // intersection
      return 0;
    }
  
  if((f2->y0<f1->y1)&&(f1->y1<f2->y1))
    {// intersection
      return 0;
    }

  // disjoint
  return 1;
}

#define PRECIS2 (1.0/1000.0)

/*
retourne 1 ssi x et y ont meme signe avec comme precision precis
*/
int meme_signe(double x,double y,double precis)
{
  if(precis<0) precis*=-1;
  
  if((x>=-precis)&&(x<=precis)) x=0.0;
  if((y>=-precis)&&(y<=precis)) y=0.0;
  
  if(((x+precis>=0.0)&&(y+precis>=0.0))||
     ((x-precis<=0.0)&&(y-precis<=0.0)))
    return 1;
  else
    return 0;
}

/*
retourne vers_obs ssi la face est
du cote de l'observateur par rapport au plan
c.a.d. si vers_obs==1 -> 1 ssi face est du cote de l'observateur
et si vers_obs==0 -> 1 ssi face est de l'autre cote de l'observateur
*/
int autre_cote(FACE2 *face,FACE2 *plan,int vers_obs)
{
  POINT equ;
  double espace_vers_obs,signe;
  int i;
  
  STOP(plan!=NULL,printf("err"));
  STOP(plan->sommets!=NULL,printf("err"));
  STOP(INTER(vers_obs,0,1),printf("err"));
  STOP(plan->nb_sommets>=2,printf("err%d",plan->nb_sommets));
  
  equ=equ_plan(plan);
  if(equ.x==0.0&&equ.y==0.0&&equ.z==0.0)
    {
      return 1;
    }
  espace_vers_obs=equ.x*xo+equ.y*yo+equ.z*zo+equ.t;
  
  for(i=0;i<face->nb_sommets;i++)
    {
      signe=pscal3d(equ,face->sommets[i])+equ.t;

      if(!INTER(signe,-PRECIS2,PRECIS2))
	{
	  if(vers_obs)
	{ // on veut etre du meme cote de l'observateur
	  if(!meme_signe(espace_vers_obs,signe,PRECIS2/*0.0*/))
	    {
	      return 0;
	    }
	}
      else
	{ // on veut etre de l'autre cote de l'observateur
	  if(meme_signe(espace_vers_obs,signe,PRECIS2/*0.0*/))
	    {
	      return 0;
	    }
	}
	}
    }
  
  return 1;
}

/*
retourne 1 ssi le segement p1-p2
coupe le segement s1-s2
*/
int intersection3d(POINT *p1,POINT *p2,POINT *s1,POINT *s2)
{
  double maxx1,maxy1,minx1,miny1;
  double maxx2,maxy2,minx2,miny2;
  double a,b;
  
  // calcul du rectangle englobant le 1er segment
  if(p1->x>p2->x)
    {
      maxx1=p1->x;
      minx1=p2->x;
    }
  else
    {
      maxx1=p2->x;
      minx1=p1->x;
    }
  
  if(p1->y>p2->y)
    {
      maxy1=p1->y;
      miny1=p2->y;
    }
  else
    {
      maxy1=p2->y;
      miny1=p1->y;
    }
  // calcul du rectangle englobant le 2eme segment
  if(s1->x>s2->x)
    {
      maxx2=s1->x;
      minx2=s2->x;
    }
  else
    {
      maxx2=s2->x;
      minx2=s1->x;
    }
  
  if(s1->y>s2->y)
    {
      maxy2=s1->y;
      miny2=s2->y;
    }
  else
    {
      maxy2=s2->y;
      miny2=s1->y;
    }
  
  if(INTER(maxx2,minx1,maxx1)||INTER(minx2,minx1,maxx1)||
     INTER(maxx1,minx2,maxx2)||INTER(minx1,minx2,maxx2))
    { // les rectangles englobants se croisent
      // calcul du produit croise (s1-p1)*(p2-p1)
      a=(s1->x-p1->x)*(p2->y-p1->y)-(p2->x-p1->x)*(s1->y-p1->y);
      // calcul du produit croise (s2-p1)*(p2-p1)
      b=(s2->x-p1->x)*(p2->y-p1->y)-(p2->x-p1->x)*(s2->y-p1->y);
      if(meme_signe(a,b,PRECIS2))
	return 0; // il n'y a pas intersection
      else
	return 1; // intersection
    }
  else
    return 0; // pas d'intersection
}

/*
retourne 1 ssi le point est dans le polygonne
*/
int dans_poly(POINT *point,FACE2 *poly)
{
  POINT p1,p2;
  int i;
  double sg,sg2;
  
  p1.x=point->x-poly->sommets[0].x;
  p1.y=point->y-poly->sommets[0].y;
  p1.z=point->z-poly->sommets[0].z;
  p2.x=poly->sommets[1].x-poly->sommets[0].x;
  p2.y=poly->sommets[1].y-poly->sommets[0].y;
  p2.z=poly->sommets[1].z-poly->sommets[0].z;
  sg=pscal3d(p1,p2);
  
  for(i=1;i<poly->nb_sommets-1;i++)
    {
      p1.x=point->x-poly->sommets[i].x;
      p1.y=point->y-poly->sommets[i].y;
      p1.z=point->z-poly->sommets[i].z;
      p2.x=poly->sommets[i+1].x-poly->sommets[i].x;
      p2.y=poly->sommets[i+1].y-poly->sommets[i].y;
      p2.z=poly->sommets[i+1].z-poly->sommets[i].z;
      sg2=pscal3d(p1,p2);
      if(!meme_signe(sg,sg2,0.0))
	return 1;
    }
  
  p1.x=point->x-poly->sommets[i].x;
  p1.y=point->y-poly->sommets[i].y;
  p1.z=point->z-poly->sommets[i].z;
  p2.x=poly->sommets[0].x-poly->sommets[i].x;
  p2.y=poly->sommets[0].y-poly->sommets[i].y;
  p2.z=poly->sommets[0].z-poly->sommets[i].z;
  sg2=pscal3d(p1,p2);
  if(!meme_signe(sg,sg2,0.0))
    return 1;
  
  // le point n'est pas dans le polygonne
  return 0;
}

/*
retourne 1 ssi les faces p1 et p2
ont des projections disjointes
*/
int projection_disjointe(FACE2 *p1,FACE2 *p2)
{
  int i,j;
  POINT *v4,*v5,*v6,*v7;
  
  // recheche si les 2 faces se coupent
  for(i=0;i<p1->nb_sommets;i++)
    {
      v4=&(p1->sommets[i]);
      transforme_point(v4); // calcul de la projection du point
      if(i==p1->nb_sommets-1)
	v5=&(p1->sommets[0]);
      else
	v5=&(p1->sommets[i+1]);
      transforme_point(v5); // calcul de la projection
      for(j=0;j<p2->nb_sommets;j++)
	{
	  v6=&(p2->sommets[i]);
	  transforme_point(v6); // calcul de la projection
	  if(j==p2->nb_sommets-1)
	    v7=&(p2->sommets[0]);
	  else
	    v7=&(p2->sommets[i+1]);
	  transforme_point(v7); // calcul de la projection du point
	  if((dans_poly(v6,p1))||(intersection3d(v4,v5,v6,v7)))
	    { // ils se coupent
	      return 0;
	    }
	}
    }
  // ils sont disjoints
  return 1;
}

/* retourne le produit vectoriel de u,v */
POINT pvect3d(POINT u,POINT v)
{
  POINT res;
  
  res.x=u.y*v.z-v.y*u.z;
  res.y=u.z*v.x-v.z*u.x;
  res.z=u.x*v.y-v.x*u.y;
  
  return res;
}

/*
retourne l'equation du plan a*x+b*y+c*z+d=0
qui contient la face "face"
x=a, y=b, z=c, t=d
si le plan est indefini, retourne (0,0,0,0)
*/
POINT equ_plan(FACE2 *face)
{
  POINT p1,p2,p3,res;
  int i;
  
  p1.x=face->sommets[1].x-face->sommets[0].x;
  p1.y=face->sommets[1].y-face->sommets[0].y;
  p1.z=face->sommets[1].z-face->sommets[0].z;
  // recherche d'un vecteur non colineaire
  for(i=2;i<face->nb_sommets;i++)
    {
      i++;
      p2.x=face->sommets[i].x-face->sommets[0].x;
      p2.y=face->sommets[i].y-face->sommets[0].y;
      p2.z=face->sommets[i].z-face->sommets[0].z;
      p3=pvect3d(p1,p2);
      if((p3.x!=0.0)||(p3.y!=0.0)||(p3.z!=0.0))
	{ // p1 et p2 ne sont pas colineaires
	  break;
	}
    }

  if(i>=face->nb_sommets)
    {// le plan est indefini
      res.x=0.0;res.y=0.0;res.z=0.0;res.t=0.0;
      return res;
    }
  res.x=p3.x;
  res.y=p3.y;
  res.z=p3.z;
  res.t=-pscal3d(p3,face->sommets[0]);
  
  STOP(INTER(res.t+p3.x*face->sommets[0].x+
	     p3.y*face->sommets[0].y+
	     p3.z*face->sommets[0].z,-PRECIS2,PRECIS2),
       printf("erreur dans le calcul de d\n");
       printf("d=%g\n",-(p3.x*face->sommets[0].x+
	     p3.y*face->sommets[0].y+p3.z*face->sommets[0].z)););

  return res;
}

// retourne 1 ssi P est derriere S
int ordre_correcte(FACE2 *P,FACE2 *S)
{
  STOP(P!=NULL,printf("err"));
  STOP(P->sommets!=NULL,printf("err"));
  STOP(S!=NULL,printf("err"));
  STOP(S->sommets!=NULL,printf("err"));
  
  
  if(disjoint(S,P))
    {
      return 1;
    }
  else if(autre_cote(P,S,0))
    {
      return 1;
    }
  else if(autre_cote(S,P,1))
    {
      return 1;
    }
  else if(projection_disjointe(P,S))
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

/* echange P et S */
void echange(FACE2 *P,FACE2 *S)
{
  FACE2 tmp;
  
  tmp=*P;
  *P=*S;
  *S=tmp;
}

/*
 ajoute le point p a la liste de point liste
 a la fin et retourne la liste
 taille contient le nombre d'element avant l'ajout
 (il est mis a jour)
*/
POINT *ajoute_point(POINT *liste,int *taille,POINT *p)
{
  POINT *tmp;
  
  if((liste==NULL)&&(*taille!=0))
    {
      printf("Erreur:liste NULL et taille non nulle\n");
      return NULL;
    }
  (*taille)++;
  tmp=malloc((*taille)*sizeof(POINT));
  if(liste!=NULL)
    {
      memcpy(tmp,liste,((*taille)-1)*sizeof(POINT));
      free(liste);
    }
  liste=tmp;
  liste[(*taille)-1]=*p;
  
  return liste;
}

/*
calcul le point intersection entre le plan
et le vecteur definit pas les 2 points p1-p2
ce point doit exister
*/
POINT intersection_plan_droite(POINT *plan, POINT *p1, POINT *p2)
{
  POINT p,res;
  double q,s,x;
  
  // calcul dans q du vecteur directeur p1-p2
  p.x=p2->x-p1->x;
  p.y=p2->y-p1->y;
  p.z=p2->z-p1->z;
  
  // simplification de l'equation a resoudre
  q=pscal3d(*plan,p);
  if(q==0.0)
    {
      printf("Erreur: colineaire\n");
      //getchar();
      exit(1);
      //return;
    }
  s=plan->t+p1->x+p1->y+p1->z;
  
  // maintenant, il faut resoudre q*x+s=0 avec x comme inconnue
  x=-s/q;
  
  // calcul du point d'intersection
  res.x=x*p.x+p1->x;
  res.y=x*p.y+p1->y;
  res.z=x*p.z+p1->z;
  
  return res;
}

/* 
insert dans la liste de faces liste_faces
les 2 faces de P apres le decoupage par le
plan contenant la face S
taille est le nombre d'element de liste_faces
*/
FACE2 *decoupe(FACE2 *liste_faces,int no,FACE2 *P,FACE2 *S,int taille)
{
  POINT equ,*l1=NULL,*l2=NULL,*p,inter;
  int i,nb_l1,nb_l2,sg_l1,sg_l2,couleur,j,inter_oui=0;
  double signe,dernier,tmp;
  FACE2 *liste_faces2;
  double xmin1,xmax1,ymin1,ymax1,zmin1,zmax1;
  double xmin2,xmax2,ymin2,ymax2,zmin2,zmax2;
  
  STOP(liste_faces!=NULL,printf("err"));
  STOP(P!=NULL,printf("err"));
  STOP(S!=NULL,printf("err"));
  STOP(taille>0,printf("err"));
  STOP(INTER(no,0,taille),printf("err"));
  STOP(&(liste_faces[no])==P,printf("err"));
  
  STOP(P->sommets!=NULL,printf("err"));
  equ=equ_plan(S);
  STOP(equ.x!=0.0&&equ.y!=0.0&&equ.z!=0.0,printf("plan indefini\n"););
  p=&(P->sommets[0]);
  nb_l1=0;nb_l2=0;
  couleur=P->couleur;
  l1=ajoute_point(l1,&nb_l1,p);
  STOP(l1!=NULL,printf("err"));
  STOP(l1[0].x==p->x,printf("err"));
  STOP(nb_l1==1,printf("err"));
  STOP(l2==NULL&&nb_l2==0,printf("err"));
  STOP(!autre_cote(P,S,0),printf("err0"));
  STOP(!autre_cote(S,P,1),printf("err1"));
  signe=equ.x*l1[0].x+equ.y*l1[0].y+equ.z*l1[0].z+equ.t;
  sg_l1=(int)signe;
  sg_l2=-sg_l1;
  xmin1=xmax1=p[0].x;
  ymin1=ymax1=p[0].y;
  zmin1=zmax1=p[0].z;
  for(i=1;i<liste_faces[no].nb_sommets;i++)
    {
      dernier=signe;
      STOP(INTER(i-1,0,nb_l1-1),printf("err"));
      STOP(INTER(i,0,liste_faces[no].nb_sommets-1),printf("err"));
      signe=equ.x*l1[i].x+equ.y*l1[i].y+equ.z*l1[i].z+equ.t;

      if(!meme_signe((double)dernier,signe,0.0))
	{ //il y a intersection entre le plan et le segemnt
	  // defini par le point precedent et le point actuel
	  inter_oui=1;
	  // calcul du point d'intersection
	  inter=intersection_plan_droite(&equ,&(P->sommets[i-1]),&(p[i]));
	  // ajout du point d'intersection au 2 faces
	  l1=ajoute_point(l1,&nb_l1,&(inter));
	  l2=ajoute_point(l2,&nb_l2,&(inter));
	}
      if(meme_signe((double)sg_l1,signe,0.0))
	{
	  // ajout du point dans l1
	  l1=ajoute_point(l1,&nb_l1,&(p[i]));
	  // calcul du rectangle englobant de la liste de point l1
	  if(p[i].x>xmax1) xmax1=p[i].x;
	  if(p[i].x<xmin1) xmin1=p[i].x;
	  if(p[i].y>ymax1) ymax1=p[i].y;
	  if(p[i].y<ymin1) ymin1=p[i].y;
	  if(p[i].z>zmax1) zmax1=p[i].z;
	  if(p[i].z<zmin1) zmin1=p[i].z;
	}
      else
	{
	  // calcul du rectangle englobant de la liste de point l2
	  if(l2==NULL)
	    {
	      xmin2=xmax2=p[i].x;
	      ymin2=ymax2=p[i].y;
	      zmin2=zmax2=p[i].z;
	    }
	  else
	    {
	      if(p[i].x>xmax2) xmax2=p[i].x;
	      if(p[i].x<xmin2) xmin2=p[i].x;
	      if(p[i].y>ymax2) ymax2=p[i].y;
	      if(p[i].y<ymin2) ymin2=p[i].y;
	      if(p[i].z>zmax2) zmax2=p[i].z;
	      if(p[i].z<zmin2) zmin2=p[i].z;
	    }
	  // ajout du point dans l2
	  l2=ajoute_point(l2,&nb_l2,&(p[i]));
	  STOP(l2!=NULL,printf("err"));
	}
    }

  STOP(inter_oui==1,
       printf("Il n'y a pas eut d'intersection\n");
       for(i=0;i<taille-1;i++)
       {
	 if(ordre_correcte(&(liste_faces[i]),&(liste_faces[i+1])))
	   printf("faces %d et %d correctes################\n",i,i+1);
	 else
	   printf("faces %d et %d incorrectes@@@@@@@@@@@@@@@@@@@@@@@@\n",i,i+1);
       });
  
  STOP(l2!=NULL,printf("err"));
  STOP(l1!=NULL,printf("err"));
  
  // effacement de la case a remplacer
  free(liste_faces[no].sommets);
  liste_faces[no].sommets=NULL;
  STOP(liste_faces!=NULL,printf("err"));

  // ajout des 2 faces a la place de P
  liste_faces2=(FACE2*)malloc((taille+1)*sizeof(FACE2));

  memcpy(liste_faces2,liste_faces,(taille+1)*sizeof(FACE2));
  free(liste_faces);
  liste_faces=liste_faces2;

  // on decale les cases apres no d'une case 
  memmove(&(liste_faces[no+2]),&(liste_faces[no+1]),
	  (taille-no)*sizeof(POINT));

  // on met l1 le plus petit zmin et l2 le plus grand
  if(zmin1>zmin2) 
    {
      p=l2;
      l2=l1;
      l1=p;
      tmp=xmin2;
      xmin2=xmin1;
      xmin1=tmp;
      tmp=xmax2;
      xmax2=xmax1;
      ymax1=tmp;
      tmp=ymin2;
      ymin2=ymin1;
      ymin1=tmp;
      tmp=ymax2;
      ymax2=ymax1;
      ymax1=tmp;
      tmp=zmin2;
      zmin2=zmin1;
      zmin1=tmp;
      tmp=zmax2;
      zmax2=zmax1;
      zmax1=tmp;
      i=nb_l1;
      nb_l1=nb_l2;
      nb_l2=i;
    }
   
  // insertion de l1
  liste_faces[no].sommets=l1;
  liste_faces[no].nb_sommets=nb_l1;
  liste_faces[no].x0=xmin1;
  liste_faces[no].y0=ymin1;
  liste_faces[no].x1=xmax1;
  liste_faces[no].y1=ymax1;
  liste_faces[no].zmin=zmin1;
  liste_faces[no].zmax=zmax1;
  liste_faces[no].couleur=couleur;
  
  // insertion de l2
  for(i=1;i+no<taille-1;i++)
    {
      if(liste_faces[no+i+1].zmin>zmin2)
	{
	  break;
	} 
    } 
  
  if((i+no==taille-1)&&(liste_faces[no+i].zmin>zmin2))
    j=taille-2-no; // l2 doit etre mis a la fin du tableau
  else
    j=i-1;
  
  // on decale les elements du tableau
  if(i>1)
    memmove(&(liste_faces[no+1]),&(liste_faces[no+2]),
	    (j)*sizeof(POINT));
 
  liste_faces[no+i].sommets=l2;
  liste_faces[no+i].nb_sommets=nb_l2;
  liste_faces[no+i].x0=xmin2;
  liste_faces[no+i].y0=ymin2;
  liste_faces[no+i].x1=xmax2;
  liste_faces[no+i].y1=ymax2;
  liste_faces[no+i].zmin=zmin2;
  liste_faces[no+i].zmax=zmax2;
  liste_faces[no+i].couleur=couleur;
  
  STOP(liste_faces[no].sommets!=NULL,printf("err"));
  STOP(liste_faces[no+1].sommets!=NULL,printf("err"));
  
  return liste_faces;
}

// affichage de la liste des faces
void affiche_faces(FACE2 *liste,int nb)
{
  FACE2 *face;
  int i,j;
  POINT *p;
  
  printf("Affichage des faces (peintre) :\n");
  for(i=0;i<nb;i++)
    {
      face=&(liste[i]);
      printf("face %d (%d sommets) ",face->no_face,face->nb_sommets);
      printf("xo=%g yo=%g x1=%g y1=%g zmin=%g zmax=%g\n",face->x0,face->y0,
	     face->x1,face->y1,face->zmin,face->zmax);
      p=face->sommets;
      for(j=0;j<face->nb_sommets;j++,p++)
	{
	  printf("%d) x=%g y=%g z=%g t=%g \n",j,p->x,p->y,p->z,p->t);
	}
      printf("\n");
    }
  printf("fin affichage faces\n");
}

// visualise en utilisant l'algorithme du peintre
void visual_peintre(OBJET *objet)
{
  FACE2 *liste_faces,*face,*P,*S;
  double xt,yt,zt,c; 
  int i,j,*p,dernier,p_mal_place,taille,k,fin;
  FACE *faso;
  POINT p1,p4;
  
  if(objet==NULL)
    return;
  
  printf("affichage peintre...\n");
  // creation dans liste_faces de la liste des faces
  liste_faces=(FACE2*)calloc(objet->nf,sizeof(FACE2));
  face=liste_faces;
  faso=objet->faso;
  for(i=0;i<objet->nf;i++,face++)
    {
      p=faso[i+1];
      face->x0=face->x1=objet->so[p[0]-1].x;
      face->y0=face->y1=objet->so[p[0]-1].y;
      face->zmin=face->zmax=objet->so[p[0]-1].z;
      face->no_face=i;
      
      face->sommets=(POINT*)calloc(5,sizeof(POINT));
      STOP(face->sommets!=NULL,printf("err"));
      face->sommets[0]=objet->so[p[0]-1];
      for(j=1;p[j]!=p[0];j++)
	{
	  STOP(INTER(p[j]-1,0,objet->ns),printf("err"));
	  STOP(j>=0,printf("err"));
	  xt=objet->so[p[j]-1].x;
	  yt=objet->so[p[j]-1].y;
	  zt=objet->so[p[j]-1].z;
	  if(xt<face->x0) face->x0=xt;
	  if(xt>face->x1) face->x1=xt;
	  if(yt<face->y0) face->y0=yt;
	  if(yt>face->y1) face->y1=yt;
	  if(zt<face->zmin) face->zmin=zt;
	  if(zt>face->zmax) face->zmax=zt;
	  
	  face->sommets[j]=objet->so[p[j]-1];
	}
      
      face->sommets[j]=objet->so[p[j]-1];
      face->nb_sommets=j;

      // calcul du vecteur normal a la surface
      STOP(INTER(p[1]-1,0,objet->ns-1),printf("%d,%d<0",p[1]-1,objet->ns-1););
      STOP(INTER(p[0]-1,0,objet->ns-1),printf("%d<0",p[0]-1););
      STOP(INTER(j-1,0,3),printf("%d<0",j-1););
      STOP(INTER(p[j-1]-1,0,objet->ns-1),printf("%d<0",p[j-1]-1););
          
      p1.x=objet->so[p[1]-1].x-objet->so[p[0]-1].x;
      p1.y=objet->so[p[1]-1].y-objet->so[p[0]-1].y;
      p1.z=objet->so[p[1]-1].z-objet->so[p[0]-1].z;
      
      p4.x=objet->so[p[0]-1].x-objet->so[p[j-1]-1].x;
      p4.y=objet->so[p[0]-1].y-objet->so[p[j-1]-1].y;
      p4.z=objet->so[p[0]-1].z-objet->so[p[j-1]-1].z;
      
      
      p1=pvect3d(p1,p4);
      
      STOP(norme3d(&p1)>0.0,printf("err"));
      
      // calcul du vecteur de la source ponctuel
      p4.x=0;
      p4.y=0;
      p4.z=30;
      
      // calcul du cosinus entre ces 2 vecteurs
      c=((double)(p1.x*p4.x+p1.y*p4.y+p1.z*p4.z))/
	(norme3d(&p1)*norme3d(&p4));
      
      if(c<0) c*=-1;
      
      c=1.0-c;
      STOP(INTER(c,0,M_PI_2),printf("err"));
      face->couleur=(int)( ((double)nb_gris()-1.0)*((double)c/(double)M_PI_2) );
      STOP(INTER(face->couleur,BLANC,NOIR),printf("err"));
    }
 
  // trie des faces suivant les z decroissants
  trie(liste_faces,objet->nf);

  dernier=0;
  taille=objet->nf; // contient le nombre de faces dans liste_faces
  while((dernier<taille))
    {
      P=&(liste_faces[dernier]);
      S=&(liste_faces[dernier+1]);
      if(P->zmax<=S->zmin)
	{ // P est correctement placer
	  dernier++;
	}
      else
	{ // peut-etre que P est mal place
	  p_mal_place=0;fin=0;
	  //do {
	  p_mal_place=0;
	  for(j=dernier+1;j<taille;j++)
	    {
	      S=&(liste_faces[j]);
	      if(S->zmin<P->zmax) 
		{
		  STOP(S!=NULL,printf("err(j=%d)",j));
		  STOP(P!=NULL,printf("err(j=%d)",j));
		  STOP(S->sommets!=NULL,printf("err(j=%d %d )",j,S->nb_sommets));
		  STOP(P->sommets!=NULL,printf("err(j=%d)",j));
		  if((!ordre_correcte(P,S)))
		    {
		      // echange du contenu dans le tableau de P et de S
		      echange(P,S);
		      
		      //recheche si c'est bien place
		      p_mal_place=0;
		      for(k=dernier+1;k<taille;k++)
			{
			  S=&(liste_faces[k]);
			  if(S->zmin<P->zmax)
			    {
			      STOP(S!=NULL,printf("err(k=%d)",k));
			      STOP(P!=NULL,printf("err(k=%d)",k));
			      STOP(S->sommets!=NULL,printf("err(k=%d)",k));
			      STOP(P->sommets!=NULL,printf("err(k=%d)",k));
			      if(!ordre_correcte(P,S))
				{ // Ils sont toujours mals places
				  p_mal_place=1;
				  break;
				}
			    }
			  /*else
			    {
			    break;
			    }*/
			}
		      
		      if(p_mal_place==1)
			{// il faut decouper P
			  liste_faces=decoupe(liste_faces,dernier,P,S,taille);
			  taille++;
			  break;
			}
		    }
		}
	      else
		{
		  break;
		}
	    }
	  fin++;
	  //break;
	  //} while((p_mal_place==1)&&(fin<iter));
	  if(p_mal_place==0)
	    {// P et bien place
	      dernier++;
	    }
	}
      }
  
  // on calcul les projections des faces
  initialiser_affichage2(objet,liste_faces,taille);
  
  // affichage des faces suivant l'ordre
  for(i=0;i<taille;i++)
    {
      P=&(liste_faces[i]);
      trace_face(objet,P);
    }
  
  // effacement de la liste des faces
  for(i=0;i<taille;i++)
    {
      free(liste_faces[i].sommets);
    }
  
  free(liste_faces);
}
