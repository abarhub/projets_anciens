#include"2d.h"
#include"3d.h"
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

MATRICE M15,Mt;
double pscal3d(POINT u,POINT v);

void mulmat(MATRICE A, MATRICE B, MATRICE C)
{
  int i,j,k;

  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      {
	C[i][j]=0;
	for(k=0;k<4;k++)
	  C[i][j]+=A[i][k]*B[k][j];
      }
  //affiche_mat(C,"c");
}

POINT mulmavec(MATRICE M, POINT X)
{
  POINT R;

  R.x=M[0][0]*X.x+M[0][1]*X.y+M[0][2]*X.z+M[0][3]*X.t;
  R.y=M[1][0]*X.x+M[1][1]*X.y+M[1][2]*X.z+M[1][3]*X.t;
  R.z=M[2][0]*X.x+M[2][1]*X.y+M[2][2]*X.z+M[2][3]*X.t;
  R.t=M[3][0]*X.x+M[3][1]*X.y+M[3][2]*X.z+M[3][3]*X.t;

  return R;
}

void changement_repere(OBJET *ob)
{
  int i;

  for(i=0;i<ob->ns;i++)
  {
    //printf("%d %d\n",i,ob->ns);
    ob->so[i]=mulmavec(M15,ob->so[i]);
  }
}

void retour_cartesiennes(POINT *P)
{
  //printf("P->z=%lf ",P->z);
  P->x/=P->t;
  P->y/=P->t;
  P->z/=P->t;
  P->t/=P->t;
  //printf("P->z=%lf\n",P->z);
}

void objet_cartesiennes(OBJET *obj)
{
  int i,j;
  
  for(i=0;i<obj->ns;i++)
  {
    retour_cartesiennes(&((obj->tso)[i]));
  }
}

void transformation_perpective(OBJET *obj)
{
  int i;
  
  for(i=0;i<obj->ns;i++)
  {
     printf("%d\n",i);
     obj->tso[i]=mulmavec(Mt,obj->so[i]);
  }
  
}

void transformation_orthogonale(OBJET *obj)
{
  int i;
  
  for(i=0;i<obj->ns;i++)
  {
     printf("%d\n",i);
     obj->tso[i]=obj->so[i];
  }
  
}

void trace_arete(POINT A, POINT B)
{
  POINT2D a1, b1;
  
  a1.x=A.x; a1.y=A.y;
  b1.x=B.x; b1.y=B.y;

  tracer(a1,b1);
}

void visual_transpar(OBJET T)
{
  int i;

  //printf("affichage transp\n");
  for(i=0;i<T.na;i++)
    if(T.ar[i].nat)
      {
	trace_arete(T.tso[T.ar[i].nor-1],T.tso[T.ar[i].nex-1]);
	/*
	  printf("%d (%lf,%lf)(%d)   (%lf,%lf)(%d)\n",i+1,
	  T.tso[T.ar[i].nor-1].x,
	  T.tso[T.ar[i].nor-1].y,
	  //T.tso[T.ar[i].nor].z,
	  T.ar[i].nor-1,
	  T.tso[T.ar[i].nex-1].x,
	  T.tso[T.ar[i].nex-1].y,
	  //T.tso[T.ar[i].nex].z,
	  T.ar[i].nex-1);
	*/
      }
}

typedef struct FACE2 
{
  double x0,y0,x1,y1;
  double zmin,zmax;
  //FACE *face;
  POINT *sommets;
  int no_face,nb_sommets;
} FACE2;

#define PRECISION (10000.0)
// retourne la difference entre les zmin
// utilise par qsort en considerant
// la precision PRECISION
int cmp(const void *a,const void *b)
{
  return (int)(( ((FACE2*)a)->zmin *PRECISION) - 
	       ( ((FACE2*)b)->zmin *PRECISION));
}

// trie le tableau face avec taille elements
void trie(FACE2 *face,int taille)
{
  qsort(face,taille,sizeof(FACE2),cmp);
}

void trace_face(OBJET *objet,int face,FACE2 *f)
{
  FACE *faso;
  POINT p1,p2;
  int *p,i;
  
  /*faso=objet->faso;
  p=faso[face+1];
  i=0;
  do
    {
      p1=objet->tso[p[i]];
      p2=objet->tso[p[i+1]];
      trace_arete(p1,p2);
      i++;
    }
    while(p[i+1]!=p[0]);*/
  for(i=0;i<f->nb_sommets-1;i++)
    {
      trace_arete(f->sommets[i],f->sommets[i+1]);
    }
  trace_arete(f->sommets[i],f->sommets[0]);
}

/* return 1 si les faces f1 et f2 sont disjoint
   et 0 sinon*/
int disjoint(FACE2 *f1,FACE2 *f2)
{
  if((f1->x0<f2->x0)&&(f2->x0<f1->x1))
    { // intersection
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

  return 1;
}

/*
retourne 1 ssi x et y ont meme signe
*/
int meme_signe(double x,double y)
{
  if(((x>0.0)&&(y>0.0))||((x<0.0)&&(y<0.0))
     ||((x==0.0)&&(y==0.0)))
    return 1;
  
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
  POINT v1,v2,v3;
  double espace_vers_obs,n;
  int i;
  extern int xo,yo,zo;
  
  v1.x=plan->sommets[1].x-plan->sommets[0].x;
  v1.y=plan->sommets[1].y-plan->sommets[0].y;
  v1.z=plan->sommets[1].z-plan->sommets[0].z;
  /*v2.x=plan->sommets[2].x-plan->sommets[1].x;
    v2.y=plan->sommets[2].y-plan->sommets[1].y;
    v2.z=plan->sommets[2].z-plan->sommets[1].z;*/
  
  v3.x=xo-plan->sommets[0].x;
  v3.y=yo-plan->sommets[0].y;
  v3.z=zo-plan->sommets[0].z;
  espace_vers_obs=pscal3d(v1,v3);
  for(i=0;i<face->nb_sommets;i++)
    {
      v2.x=face->sommets[i].x-plan->sommets[0].x;
      v2.y=face->sommets[i].y-plan->sommets[0].y;
      v2.z=face->sommets[i].z-plan->sommets[0].z;
      n=pscal3d(v1,v2);
      if(vers_obs==0)
	{ // on veut etre de l'autre cote de l'observateur
	  if(meme_signe(n,espace_vers_obs))
	    return 0;
	}
      else
	{ // on veut etre du meme cote de l'observateur
	  if(!meme_signe(n,espace_vers_obs))
	    return 0;
	}
    }
  
  return 1;
}

/*
retourne 1 ssi les faces p1 et p2
sont disjoints
*/
int projection_disjointe(FACE2 *p1,FACE2 *p2)
{
  POINT barycentre;
  int i,j;
  POINT v1,v2,v3;
  double s1,s2;
  
  barycentre.x=0.0;barycentre.y=0.0;barycentre.z=0.0;
  for(i=0;i<p1->nb_sommets;i++)
    {
      barycentre.x+=p1->sommets[i].x;
      barycentre.y+=p1->sommets[i].y;
      barycentre.z+=p1->sommets[i].z;
    }
  barycentre.x/=p1->nb_sommets;
  barycentre.y/=p1->nb_sommets;
  barycentre.z/=p1->nb_sommets;
  
  
  
  for(i=0;i<p2->nb_sommets;i++)
    {
      for(j=0;j<p1->nb_sommets-1;j++)
	{
	  v1.x=barycentre.x-p1->sommets[j].x;
	  v1.y=barycentre.y-p1->sommets[j].y;
	  v1.z=barycentre.z-p1->sommets[j].z;
	  v2.x=p2->sommets[i].x-p1->sommets[j].x;
	  v2.y=p2->sommets[i].y-p1->sommets[j].y;
	  v2.z=p2->sommets[i].z-p1->sommets[j].z;
	  v3.x=p1->sommets[j+1].x-p1->sommets[j].x;
	  v3.y=p1->sommets[j+1].y-p1->sommets[j].y;
	  v3.z=p1->sommets[j+1].z-p1->sommets[j].z;
	  s1=pscal3d(v3,v1);
	  s2=pscal3d(v3,v2);
	  if(!meme_signe(s1,s2))
	    { // le point n'est pas dans le polygonne
	      break;
	    }
	}
      if(j==p1->nb_sommets-1)
	{
	  v1.x=barycentre.x-p1->sommets[j].x;
	  v1.y=barycentre.y-p1->sommets[j].y;
	  v1.z=barycentre.z-p1->sommets[j].z;
	  v2.x=p2->sommets[i].x-p1->sommets[j].x;
	  v2.y=p2->sommets[i].y-p1->sommets[j].y;
	  v2.z=p2->sommets[i].z-p1->sommets[j].z;
	  v3.x=p1->sommets[0].x-p1->sommets[j].x;
	  v3.y=p1->sommets[0].y-p1->sommets[j].y;
	  v3.z=p1->sommets[0].z-p1->sommets[j].z;
	  s1=pscal3d(v3,v1);
	  s2=pscal3d(v3,v2);
	  if(meme_signe(s1,s2))
	    {// le point est dans le polygonne
	      return 0;
	    }
	}
    }
  // tous les points sont hors 
  return 1;
}

// visualise en utilisant l'algorithme du peintre
void visual_peintre(OBJET *objet)
{
  FACE2 *liste_faces,*face,*P,*S;
  double x0,xt,y0,yt,zt; 
  int i,j,*p,*p2,dernier;
  FACE *faso;
  
  if(objet==NULL)
    return;
  
  printf("affichage peintre:\n");
  
  liste_faces=(FACE2*)calloc(objet->nf,sizeof(FACE2));
  printf("coucou5\n");
  face=liste_faces;
  faso=objet->faso;
  for(i=0;i<objet->nf;i++,face++)
    {
      //printf("coucou6(%p,%p )\n",faso,faso[1]);
      //face=&(liste_faces[i]);
      //printf("coucou8(%p,%p,%p)\n",objet->faso,liste_faces,(objet->faso)[0]);
      p=faso[i+1];
      //printf("coucou9(%p,%d)\n",p,p[0]);
      face->x0=face->x1=objet->tso[p[0]].x;
      //printf("coucou10\n");
      face->y0=face->y1=objet->tso[p[0]].y;
      //printf("coucou11\n");
      face->zmin=objet->tso[p[0]].z;
      //printf("coucou12\n");
      face->no_face=i;
      face->sommets=(POINT*)calloc(5,sizeof(POINT));
      //p2=face->sommets;
      face->sommets[0]=objet->tso[p[0]];
      //face->face=objet->faso;
      //printf("coucou1\n");
      for(j=1;p[j]!=p[0];j++)
	{
	  printf("coucou7(%d,%d,%d,%d)\n",i,j,objet->nf,p[j]);
	  //p=objet->faso[i];
	  xt=objet->tso[p[j]].x;
	  yt=objet->tso[p[j]].y;
	  zt=objet->tso[p[j]].z;
	  if(xt<face->x0) face->x0=xt;
	  if(xt>face->x1) face->x1=xt;
	  if(yt<face->y0) face->y0=yt;
	  if(yt>face->y1) face->y1=yt;
	  if(zt<face->zmin) face->zmin=zt;
	  if(zt>face->zmax) face->zmax=zt;
	  //p2=face->sommets;
	  //p2
	  face->sommets[j]=objet->tso[p[j]];
	}
      //p2=face->sommets;
      //p2
      face->sommets[j]=objet->tso[p[j]];
      face->nb_sommets=j;
      //printf("coucou2\n");
    }
   printf("coucou3\n");
  trie(liste_faces,objet->nf);
  printf("coucou4\n");
  for(i=0;i<objet->nf;i++)
    {
      printf("face %d(%d):",liste_faces[i].no_face,i);
      printf("zmin=%g\nsommets(%d):",liste_faces[i].zmin,liste_faces[i].nb_sommets);
      for(j=0;j<liste_faces[i].nb_sommets;j++)
	{
	  printf("(%g,%g,%g)",liste_faces[i].sommets[j].x,
		 liste_faces[i].sommets[j].y,
		 liste_faces[i].sommets[j].z);
	}
      printf("!\n");
    }
  
  dernier=objet->nf-1;

  while(dernier>=0)
    {
      P=&(liste_faces[dernier]);
      S=&(liste_faces[dernier-1]);
      if(P->zmax<S->zmin)
	{
	  trace_face(objet,dernier,P);
	  dernier--;
	}
      else
	{
	  for(j=dernier-1;j>=0;j--)
	    {
	      S=&(liste_faces[j]);
	      if(S->zmin<P->zmax)
		{
		  if(disjoint(S,P))
		    {
		      //trace_face(objet,dernier,P);
		      if(autre_cote(P,S,0))
			{
			  if(autre_cote(P,S,1))
			    {
			      if(projection_disjointe(P,S))
				{
				  
				}
			    }
			}
		    }
		}
	    }
	  if(j<0)
	    dernier--;
	}
      
      }
  
  free(liste_faces->sommets);
  free(liste_faces);
  printf("fin affichage peintre\n");
}

OBJET *lecture_objet(char *nom_fichier)
{
  FILE *fd;
  OBJET *obj;
  POINT *p;
  ARETE *a;
  FACE *fso,*far;
  int i,j,no,a1,a2,a3,a4,no2;
  double p1,p2,p3,p4;

  fd=fopen(nom_fichier,"r");
  obj=(OBJET*)malloc(sizeof(OBJET));
  /* lecture des sommets */
  fscanf(fd,"%d",&no);
  obj->ns=no;
  obj->so=(POINT*)calloc(no,sizeof(POINT));
  obj->tso=(POINT*)calloc(no,sizeof(POINT));
  for(i=0;i<no;i++)
    {
      fscanf(fd,"%lf %lf %lf %lf",&p1,&p2,&p3,&p4);
      (obj->so)[i].x=p1;
      (obj->so)[i].y=p2;
      (obj->so)[i].z=p3;
      (obj->so)[i].t=p4;
    }
  /* lecture des aretes */
  fscanf(fd,"%d",&no);
  obj->na=no;
  obj->ar=(ARETE*)calloc(no,sizeof(ARETE));
  for(i=0;i<no;i++)
    {
      fscanf(fd,"%d %d %d",&a1,&a2,&a3);
      (obj->ar)[i].nor=a1;
      (obj->ar)[i].nex=a2;
      (obj->ar)[i].nat=a3;
    }

  /* lecture des faces faar*/
  fscanf(fd,"%d %d",&no,&no2);
  obj->nf=no;
  obj->faar=(FACE*)calloc(no,sizeof(FACE));
  for(i=0;i<no;i++)
    {
      (obj->faar)[i]=(int*)calloc(no2,sizeof(int));
      for(j=0;j<no2;j++)
 {
   fscanf(fd,"%d", &a1);
   (obj->faar)[i][j]=a1;
 }
    }

  /* lecture des faces faso*/
  fscanf(fd,"%d %d",&no,&no2);
  obj->nf=no;
  obj->faso=(FACE*)calloc(no,sizeof(FACE));
  for(i=0;i<no;i++)
    {
      (obj->faso)[i]=(int*)calloc(no2,sizeof(int));
      for(j=0;j<no2;j++)
 {
   fscanf(fd,"%d", &a1);
   (obj->faso)[i][j]=a1;
 }
    }
  fclose(fd);
  return obj;
}

OBJET *lecture_objet2(char *nom_fichier)
{
  FILE *f;
  char s[50];
  OBJET *o1,*o2;
  int i,j,*p1,tmp,debut,k,no,*tmp2,l;

  //printf("124\n");
  f=fopen(nom_fichier,"r");
  //printf("1245\n");
  if(f==NULL)
    {
      printf("Erreur: Impossible d'ouvrir le fichier %s\n",nom_fichier);
      return NULL;
    }

  //fscanf("#%s",s);
  fgets(s,sizeof(s),f);
  //printf("12456\n");
  if(s[0]!='#')
    {
      printf("Erruer0: Il faut que le fichier commence par un #\n");
      return NULL;
    }
  
  //printf("124567\n");
  o1=(OBJET*)malloc(sizeof(OBJET));
  memset(o1,0,sizeof(OBJET));
  //printf("1245678\n");
  fscanf(f,"%d %d %d\n",&(o1->ns),&(o1->na),&(o1->nf));

  //printf("ns=%d na=%d nf=%d\n",o1->ns,o1->na,o1->nf);
  
  o1->so=(POINT*)calloc(o1->ns,sizeof(POINT));
  o1->tso=(POINT*)calloc(o1->ns,sizeof(POINT));
  o1->ar=(ARETE*)calloc(o1->na,sizeof(ARETE));
  o1->faar=(FACE*)calloc(o1->nf+1,sizeof(FACE));
  o1->faso=(FACE*)calloc(o1->nf+1,sizeof(FACE));
  
  //printf("1245678\n");
  fgets(s,sizeof(s),f);
  if(s[0]!='#')
    {
      printf("Erreur1: Il faut que la ligne commence par un #\n(%s)\n",s);
      return NULL;
    }
  
  for(i=0;i<o1->ns;i++)
    {
      fscanf(f,"%d %lf %lf %lf %lf\n",&tmp,&((o1->so[i]).x),
      &((o1->so[i]).y),&((o1->so[i]).z),&((o1->so[i]).t));
    }

  fgets(s,sizeof(s),f);
  if(s[0]!='#')
    {
      printf("Erruer2: Il faut que la ligne commence par un #\n");
      return NULL;
    }
  
  for(i=0;i<o1->na;i++)
    {
      fscanf(f,"%d %d %d %d\n",&tmp,&(o1->ar[i].nor),
      &(o1->ar[i].nex),&(o1->ar[i].nat));
    }

  fgets(s,sizeof(s),f);
  if(s[0]!='#')
    {
      printf("Erruer3: Il faut que la ligne commence par un #1\n");
      return NULL;
    }

  //FACES
  //printf("coucou\n");
  //lecture faso
  
  for(i=0;i<o1->nf;i++)
    {
      j=1;
      fscanf(f,"%d %d",&tmp,&debut);
      p1=(int*)malloc(1*sizeof(int));
      p1[0]=debut;
      do
	{
	  fscanf(f,"%d", &k);
	  j++;
	  p1=(int *)realloc(p1,(j+0)*sizeof(int));
	  p1[j-1]=k;
	  /*
	    for(l=0;l<j;l++)
	    printf("p1[%d]=%d\n",l,p1[l]);
	    printf("\n");
	  */
	}
      while(k!=debut);
      
      fscanf(f," \n");
      (o1->faso)[i+1]=p1;
      //printf("coucou2\n");
      //affichage des faces
      /*
 //tmp2=(o1->faso)[i];
 tmp2=p1;
 j=0;
 debut=tmp2[j];
 printf("%d ",i+1);
 printf("%d ",tmp2[j]);
 j++;
 do
 { 
 j++;
 printf("%d ",tmp2[j]);
 }
 while(tmp2[j] != debut);
 printf("\n");
      */
    }
  //printf("coucou3\n");
  // lecture de faar
  if(!feof(f))
    {
      fgets(s,sizeof(s),f);
      if(s[0]!='#')
	{
	  printf("Erruer: Il faut que la ligne commence par un #2\n");
	  return NULL;
	}
      
      fscanf(f,"%d\n",&no);
      for(i=0;i<o1->nf;i++)
	{
	  j=1;//printf("coucou6\n");
	  fscanf(f,"%d",&tmp);
	  p1=(int*)malloc(no*sizeof(int));
	  for(j=0;j<no;j++)
	    {
	      //printf("coucou7(%d)\n",j);
	      fscanf(f,"%d", &k);
	      //printf("k=%d\n",k);
	      p1[j]=k;
	    }
	  //printf("coucou5\n");
	  fscanf(f,"\n");
	  (o1->faar)[i]=p1;
	}
      //printf("coucou4\n");
    }
  
  fclose(f);
  
  return o1;
}

// lecture d'un fichier de points de control
PT_CTL *lecture_pt_control(char *nom_fichier)
{
  PT_CTL *res=NULL;
  FILE *f;
  int i,j;
  double d1,d2,d3;

  f=fopen(nom_fichier,"r");
  if(f==NULL)
    {
      printf("Erreur: Impossible d'ouvrir le fichier %s\n",nom_fichier);
      return NULL;
    }
//printf("suite0\n");
  res=(PT_CTL*)malloc(sizeof(PT_CTL));
  fscanf(f,"%d\n%d\n",&(res->nb_ligne),&(res->nb_col));
  res->tab=(POINT*)calloc(res->nb_ligne*res->nb_col,sizeof(POINT));

  //printf("suite(%d %d)\n",res->nb_ligne,res->nb_col);

  for(i=0;i<res->nb_ligne;i++)
    {
      for(j=0;j<res->nb_col;j++)
 {
   //printf("C0\n");
   fscanf(f,"%lf %lf %lf",&d1,&d2,&d3);
   printf("%p\n",res->tab);
   printf(" %p\n",res->tab[i*res->nb_col+j]
    /*res->tab[i*res->nb_col+j]*/);
   (res->tab)[i*res->nb_col+j].x=d1;
   (res->tab)[i*res->nb_col+j].y=d2;
   (res->tab)[i*res->nb_col+j].z=d3;
   (res->tab)[i*res->nb_col+j].t=1.0;
   printf("C\n");
 }
      fscanf(f,"\n");
      printf("V\n");
    }
  printf("suite2\n");
  return res;
}

void remplir_M15(double xo, double yo, double zo, double theta, double phi,
double psi)
{
  MATRICE 

  M12={
    {1.0, 0.0, 0.0, -xo},
    {0.0, 1.0, 0.0, -yo},
    {0.0, 0.0, 1.0, -zo},
    {1.0, 0.0, 0.0, 1.0}
  },

  M23=
  {
    {cos(theta), 0.0, -sin(theta),0.0},
    {0.0,        1.0, 0.0,0.0},
    {sin(theta), 0.0, cos(theta),0.0},
    {0.0,        0.0, 0.0,1.0}
  },

  M34=
  {
    {1.0,0.0,0.0,0.0},
    {0.0,cos(phi),-sin(phi),0.0},
    {0.0,sin(phi),cos(phi),0.0},
    {0.0,0.0,0.0,1.0}
  },
  
  M45=
  {
    {cos(psi),-sin(psi),0.0,0.0},
    {sin(psi),cos(psi),0.0,0.0},
    {0.0,0.0,1.0,0.0},
    {0.0,0.0,0.0,1.0}
  },Mtemp,Mtemp2,Mtemp3;

  //printf("y0=%lf\n",yo);
  //affiche_mat(M12,"M12");affiche_mat(M23,"M23");
  //affiche_mat(M34,"M34");affiche_mat(M45,"M45");
  mulmat(M12,M23,Mtemp);
  //affiche_mat(Mtemp,"M12*M23");
  mulmat(Mtemp,M34,Mtemp2);
  //affiche_mat(Mtemp2,"Mtemp*M34");
  mulmat(Mtemp2,M45,M15);
  //affiche_mat(M15,"Mtemp*MM45");
  //printf();
  //exit(0);
}

void remplir_Mt(double d, double r)
{
  MATRICE Mtemp=
  {
    {d,0.0,0.0,0.0},
    {0.0,d,0.0,0.0},
    {0.0,0.0,d,0.0},
    {0.0,0.0,-1.0,r}
  };

  memcpy(Mt,Mtemp,sizeof(Mtemp));
}

void affiche_objet(OBJET *obj)
{
 int i,j, *tmp, tmp2;
 POINT *so;
 ARETE *ar;
 FACE *faso, *faar;
 
 printf("Affichage_objet so:\n");
 
 if(obj==NULL)
   {
     printf("Erreur: objet null\n");
     return ;
   }
 /* les sommets */
 so=obj->so;
 printf("#il y a %d sommets:\n",obj->ns);
 for(i=0;i<obj->ns;i++)
 {
  printf("%d %g %g %g %g\n",i+1,so->x,so->y,so->z,so->t);
  so++;
 }
 /* les aretes */
 ar=obj->ar;
 printf("#il y a %d aretes:\n",obj->na);
 for(i=0;i<obj->na;i++)
 {
  printf("%d %d %d %d\n",i+1,ar->nor,ar->nex,ar->nat);
  ar++;
 }
 /* les faces */
 faso=obj->faso;
 printf("#il y a %d faces: (faso)\n",obj->nf);

 for(i=1;i<=obj->nf;i++)
 {
   printf("%d ",i);
   j=0;
   tmp=faso[i];
   tmp2=tmp[j];
   //j++;
   printf("%d ",tmp[j]);
   do
     {
       j++;
       printf("%d ",tmp[j]);
     }
     while(tmp[j] != tmp[0]);
   printf("\n");
 }
}

void affiche_objet_ds_fichier(OBJET *obj)
{
 int i,j, *tmp, tmp2;
 POINT *so;
 ARETE *ar;
 FACE *faso, *faar;
 FILE *f; 
 //printf("Affichage_objet so:\n");
 
 f=fopen("essai","w");

 
 if(obj==NULL)
   {
     printf("Erreur: objet null\n");
   }

 fprintf(f,"# equation parametrique\n");
 fprintf(f,"%d %d %d\n",obj->ns,obj->na,obj->nf);
 /* les sommets */
 so=obj->so;
 fprintf(f,"#il y a %d sommets:\n",obj->ns);
 for(i=0;i<obj->ns;i++)
 {
  fprintf(f,"%d %g %g %g %g\n",i+1,so->x,so->y,so->z,so->t);
  so++;
 }
 /* les aretes */
 ar=obj->ar;
 fprintf(f,"#il y a %d aretes:\n",obj->na);
 for(i=0;i<obj->na;i++)
 {
  fprintf(f,"%d %d %d %d\n",i+1,ar->nor,ar->nex,ar->nat);
  ar++;
 }
 /* les faces */
 faso=obj->faso;
 fprintf(f,"#il y a %d faces: (faso)\n",obj->nf);

 for(i=1;i<=obj->nf;i++)
 {
   fprintf(f,"%d ",i);
   j=0;
   tmp=faso[i];
   tmp2=tmp[j];
   //j++;
   fprintf(f,"%d ",tmp[j]);
   do
     {
       j++;
       fprintf(f,"%d ",tmp[j]);
     }
     while(tmp[j] != tmp[0]);
   fprintf(f,"\n");
 }
 fclose(f);
 
}

void affiche_objet2(OBJET *obj)
{
  int i,j;
  POINT *so;
  ARETE *ar;
  
  printf("Affichage_objet tso:\n");
 
 /* les sommets */
 so=obj->tso;
 printf("il y a %d sommets:\n",obj->ns);
 for(i=0;i<obj->ns;i++)
 {
  printf("%d) = %g %g %g %g\n",i+1,so->x,so->y,so->z,so->t);
  so++;
 }
 
 /* les aretes */

 ar=obj->ar;
 printf("il y a %d aretes:\n",obj->na);
 for(i=0;i<obj->na;i++)
 {
  printf("%d) = %d %d %d\n",i+1,ar->nor,ar->nex,ar->nat);
  ar++;
 }
 
 
}

/* retourne le produit scalaire de u et v */
double pscal3d(POINT u,POINT v)
{
  return u.x*v.x+u.y*v.y+v.z*v.z;
}

/* retourne 1 ssi les point p1 p2 et p3 sont alignes */
int colineaire(POINT p1,POINT p2,POINT p3)
{
  POINT u,v;
  
  // u contient le segment [p1,p2]
  u.x=p2.x-p1.x;
  u.y=p2.y-p1.y;
  u.z=p2.z-p1.z;
// v contient le segment [p1,p3]
  v.x=p3.x-p1.x;
  v.y=p3.y-p1.y;
  v.z=p3.z-p1.z;

  return pscal3d(u,v)==0.0;
}

/* retourne un objet triangule */
OBJET *triangulation(double(*f)(double a,double b), 
       double(*g)(double a,double b), 
       double(*h)(double a,double b),
       double umin,double umax,double vmin,
       double vmax,int nu,int nv)
{
  int i,j,na,ns,nf,s1,s2,s3,s4,a1,a2,a3,a4,a5;
  int *tmp, tmp2;
  double pu,pv,x,y,z,u,v;
  OBJET *ret;
  POINT *so;
  ARETE *ar;
  FACE *faso, *faar;
  
  if(nv<=1||nu<=1)
    {
      printf("Erreur: nu ou nv doivent etre >1\n");
      exit(1);
    }

  pu=(umax-umin)/(nv-1);
  pv=(vmax-vmin)/(nu-1);
  
  ret=(OBJET *)malloc(sizeof(OBJET));
  
printf("salut\n");
  //Sommets
  ns=0;
  so=(POINT*)calloc(nu*nv+1,sizeof(POINT));
  for(j=0;j<nu;j++)
    for(i=0;i<nv;i++)
      {
	ns++;
	u=umin+i*pu;
	v=vmin*j*pv;
	x=f(u,v);y=g(u,v);z=h(u,v);
	so[ns].x=x; // /R 
	so[ns].y=y;
	so[ns].z=z;
	so[ns].t=1;
	/*so[ns].x=y; // /R1
	  so[ns].y=z;
	  so[ns].z=x;
	  so[ns].t=1;*/
      }
printf("salut4\n");
  //aretes
  na=0;
  printf("salut6(%d)\n",3*nu*nv-2*nu-2*nv+1);
  ar=(ARETE *)calloc(3*nu*nv-2*nu-2*nv+1+1,sizeof(ARETE));
  printf("salut7\n");
  //horizontales
  for(j=0;j<nu;j++)
    for(i=0;i<nv-1;i++)
      {
	na++;
	printf("na=%d\n",na);
	ar[na].nor=j*nv+i+1; //s1
	ar[na].nex=j*nv+(i+1)+1; //s2
	ar[na].nat=1;
      }
 printf("salut5(%d)\n",na);
  //verticales
  for(j=0;j<nu-1;j++)
    for(i=0;i<nv;i++)
      {
	na++;
	ar[na].nor=j*nv+i+1; //s1
	ar[na].nex=(j+1)*nv+i+1; //s4
	ar[na].nat=1;
      }
 printf("salut3(%d)\n",na);
  //obliques
  for(j=0;j<nu-1;j++)
    for(i=0;i<nv-1;i++)
      {
	na++;
	printf("na2=%d\n",na);
	ar[na].nor=j*nv+(i+1)+1; //s2
	ar[na].nex=(j+1)*nv+i+1; //s4
	ar[na].nat=1;
      }
  
  //Faces
  nf=0;
  faar=(FACE*)calloc(2*nu*nv-2*nu-2*nv+2+1,sizeof(FACE));
  faso=(FACE*)calloc(2*nu*nv-2*nu-2*nv+2+1,sizeof(FACE));
  printf("salut2\n");
  for(j=0;j<nu-1;j++)
    for(i=0;i<nv-1;i++)
      {
	s1=j*nv+i+1;
	s2=j*nv+(i+1)+1;
	s3=(j+1)*nv+(i+1)+1;
	s4=(j+1)*nv+i+1;
	
	a1=j*(nv-1)+i+1;
	a4=nu*(nv-1)+j*nv+i+1;
	a2=a4+nu-1;
	a3=a1+nv-1;
	a5=nu*(nv-1)+nv*(nu-1)+j*(nv-1)+i+1;
	
	if(!colineaire(so[s1],so[s2],so[s4]))
	  { // so[s1],so[s2],so[s4] pas colineaires
	    nf++;
	    faso[nf]=(int *)calloc(4,sizeof(int));
	    (faso[nf])[0]=s1;
	    (faso[nf])[1]=s2;
	    (faso[nf])[2]=s4;
	    (faso[nf])[3]=s1;
	    faar[nf]=(int *)calloc(3,sizeof(int));
	    (faar[nf])[0]=a1;
	    (faar[nf])[1]=a5;
	    (faar[nf])[2]=-a4;
	  }
	if(!colineaire(so[s2],so[s3],so[s4]))
	  { // so[s2],so[s3],so[s4] pas colineaires
	    nf++;
	    faso[nf]=(int *)calloc(4,sizeof(int));
	    (faso[nf])[0]=s2;
	    (faso[nf])[1]=s3;
	    (faso[nf])[2]=s4;
	    (faso[nf])[3]=s2;
	    faar[nf]=(int *)calloc(3,sizeof(int));
	    (faar[nf])[0]=a2;
	    (faar[nf])[1]=-a3;
	    (faar[nf])[2]=-a5;
	  }
      }
  
 /* retour de l'objet */
  ret->ns=ns;
  ret->na=na;
  ret->nf=nf;

  ret->so=so;
  ret->tso=(POINT*)calloc(nu*nv+1,sizeof(POINT));
  ret->ar=ar;
  ret->faar=faar;
  ret->faso=faso;

  return ret;
}

void efface_objet(OBJET *objet)
{
  int i;
  
  if(objet==NULL)
    return;

  if(objet->so!=NULL)
    free(objet->so);
  if(objet->tso!=NULL)
    free(objet->tso);
  if(objet->ar!=NULL)
    free(objet->ar);
  if(objet->faso!=NULL)
    {
      for(i=0;i<objet->nf+1;i++)
	{
	  if(objet->faso[i]!=NULL)
	    {
	      free(objet->faso[i]);
	    }
	}
      free(objet->faso);
    }
  if(objet->faar!=NULL)
    {
      for(i=0;i<objet->nf;i++)
	{
	  if(objet->faso[i]!=NULL)
	    {
	      free(objet->faar[i]);
	    }
	}
      free(objet->faar);
    }
  free(objet);
}
