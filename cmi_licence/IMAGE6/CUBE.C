#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include"3d.h"

#include"2d.h"
#include<math.h>
#include<string.h>
#include<time.h>
#include"debug.h"

enum Sous_type {Ecube=0,Eavion,Ecanard,Ecrane,Evoiture,Esphere,
      Eparabole,Esurface,Etest1,Eautre,Ebezier1} fichier_actuel;

typedef struct Tconfig
{
  double xo,yo,zo,phi,theta,psi;
  double r,d;
  double min_v,max_v,min_u,max_u;
  int nb_u,nb_v;
  struct Tconfig *suivant;
} Tconfig;

typedef struct Tobjet
{
  int nb_config;
  Tconfig *liste_config;
  char *nom;
  enum Type_objet {Efichier,Ecourbe,Ebezier} type_objet;
  enum Sous_type sous_type;
  struct Tobjet *suivant;
} Tobjet;

char nom_fichier[100];
OBJET *objet=NULL;
CLOTURE clo;
FENETRE fen2;
POINT p1={-10.0,-10.0,0.0,1.0},p2={10.0,10.0,0.0,1.0};
PT_CTL *ctl;
double xo,yo,zo,phi,theta,psi;
double r,d;
static int fin_tourner=0;
char c2[2]="0";
enum Type_objet etat;
int nb_v,nb_u,relatif=0;
double min_v,max_v,min_u,max_u;
double (*fonc1)(double,double);
double (*fonc2)(double,double);
double (*fonc3)(double,double);
double sphere1(double u,double v);
double parabole1(double u,double v);
enum {Etransparent,Ecache} etat_affiche=Etransparent;
Tobjet *liste_config=NULL;


int perspective=1;

void initialiser_affichage();
void relecture_objet(void);

Tobjet *cree_objet(char *nom, enum Type_objet type_objet,
		     enum Sous_type sous_type)
{
  Tobjet *objet;
  
  objet=(Tobjet*)malloc(sizeof(Tobjet));
  if(nom!=NULL)
    objet->nom=strdup(nom);
  else
    objet->nom=NULL;
  objet->liste_config=0;
  objet->type_objet=type_objet;
  objet->sous_type=sous_type;
  objet->nb_config=0;
  objet->suivant=NULL;
  
  return objet;
}

Tobjet *ajoute_objet(Tobjet *liste_objet,Tobjet *objet)
{
  Tobjet *tmp;
  /*Tobjet *objet;
  
  STOP(objet!=NULL,printf("Ajout d'un objet vide\n"));
  
  objet=malloc(sizeof(Tobjet));
    objet->nom=strdup(nom);
  objet->liste_config=0;
  objet->type_objet=type_objet;
  objet->sous_type=sous_type;
  objet->nb_config=0;*/
  objet->suivant=NULL;
  if(liste_objet==NULL)
    {
      return objet;
    }
  else
    {
      tmp=liste_objet;
      while(tmp->suivant!=NULL) tmp=tmp->suivant;
      tmp->suivant=objet;
      return liste_objet;
    }
}

void ajoute_config(Tobjet *objet,double xo,double yo,double zo,
		   double phi,double theta,double psi,double r,double d,
		   double min_v,double max_v,double min_u,double max_u,
		   int nb_u,int nb_v)
{
  Tconfig *conf;
  Tconfig *config;
  
  STOP(objet!=NULL,printf("ajoute d'une config a un objet vide"));
  
  config=(Tconfig*)malloc(sizeof(Tconfig));
  config->xo=xo;
  config->yo=yo;
  config->zo=zo;
  config->phi=phi;
  config->theta=theta;
  config->psi=psi;
  config->r=r;
  config->d=d;
  config->min_v=min_v;
  config->max_v=max_v;
  config->min_u=min_u;
  config->max_u=max_u;
  config->nb_u=nb_u;
  config->nb_v=nb_v;
  config->suivant=NULL;
  if(objet->liste_config==NULL)
    {
      objet->liste_config=config;
    }
  else
    {
      conf=objet->liste_config;
      while(conf->suivant!=NULL) conf=conf->suivant;
      conf->suivant=config;
    }
  objet->nb_config++;
}

void efface_configs(Tobjet *liste_objet)
{
  Tobjet *tmp,*tmp2;
  Tconfig *conf1,*conf2;
  
  tmp=liste_objet;
  while(tmp!=NULL)
    {
      tmp2=tmp->suivant;
      conf1=tmp->liste_config;
      while(conf1!=NULL)
	{
	  conf2=conf1->suivant;
	  free(conf1);
	  conf1=conf2;
	}
      if(tmp->nom!=NULL)
	free(tmp->nom);
      free(tmp);
      tmp=tmp2;
    }
}

Tobjet *recherche_objet(Tobjet *objet,enum Sous_type type)
{
  Tobjet *tmp;
  STOP(objet!=NULL,printf("Err"));
  
  tmp=objet;
  while((tmp!=NULL)&&(tmp->sous_type!=type))
    {
      tmp=tmp->suivant;
    }
  
  return tmp;
}

Tconfig *recherche(Tobjet *objet,enum Sous_type type,int no)
{
  Tobjet *tmp;
  Tconfig *conf;
  int i;
  
  STOP(no>=0,printf("Numero de configuration invalide"));
  STOP(objet!=NULL,printf("err"));
  
  tmp=recherche_objet(objet,type);
    
  if(tmp!=NULL)
    {
      conf=tmp->liste_config;
      for(i=0;i<no;i++)
	{
	  if(conf==NULL)
	    {
	      printf("Numero invalide\n");
	      return NULL;
	    }
	}
      return conf;
    }
  else
    return NULL;
}

/*
static void catch_sigint(int signo)
{
  char c[2]="\0";
  //printf("Signal:psi=%g theta=%g\n",psi,theta);
  //c=getchar();
  //if(c=='q') fin_tourner=1;
  write(1,"Signal\n",8);
  read(0,c,2);
  if(c[0]=='q')
    fin_tourner=1;
  write(1,c,2);
  c2[0]=c[0];
  c2[1]=c[1];
}
*/

void affiche_mat(MATRICE m,char *s)
{
  int i,j;
  
  printf("%s:\n",s);
  for(i=0;i<4;i++)
    {
      for(j=0;j<4;j++)
 printf("%05.3lf ",m[i][j]);
      printf("\n");
    }
  
}


void afficher()
{
  erase();
  if(etat_affiche==Etransparent)
    {// transparence
      visual_transpar(*objet); //utilise na, ar, tso
    }
  else
    {//  faces caches
      visual_peintre(objet);
    }
  flushpl();
  
  //affiche_objet(objet);
}

// choisi les parametres predefinis numero 1, etc...
void choix_coordonnees(int n)
{
  Tconfig *conf;
  //relecture_objet();
  
  conf=recherche(liste_config,fichier_actuel,n);
  
  STOP(conf!=NULL,printf("Err"));
  
  //strcpy(nom_fichier,conf);
  
  phi=conf->phi;
  theta=conf->theta;
  psi=conf->psi;
  xo=conf->xo;
  yo=conf->yo;
  zo=conf->zo;
  d=conf->d;
  r=conf->r;
  min_v=conf->min_v;
  max_v=conf->max_v;
  min_u=conf->min_u;
  max_u=conf->max_u;
  nb_u=conf->nb_u;
  nb_v=conf->nb_v;
  
  return;
  /*
  if(fichier_actuel==Ecube) // cube
    {
      phi=0;psi=1.0472;theta=0.628319;
      xo=0;yo=0;zo=7;
      d=10;r=1;
    }
  else if(fichier_actuel==Eavion) //avion
    {
      switch(n)
	{
	case 0:
	  phi=0;psi=1.0472;theta=0.628319;
	  xo=0;yo=0;zo=7;
	  d=10;r=1;
	  break;
	case 1:
	  phi=0;psi=1.0472;theta=1.0;
	  xo=0;yo=0;zo=7;
	  d=10;r=1;
	  break;
	case 2:
	  phi=0;psi=1.0472;theta=1.5;
	  xo=0;yo=0;zo=7;
	  d=10;r=1;
	  break;
	case 3:
	  phi=1;psi=1.4;theta=.5;
	  xo=0;yo=0;zo=7;
	  d=10;r=1;
	  break;
	case 4:
	  phi=0;psi=1.0;theta=1.2;
	  xo=0;yo=0;zo=7;
	  d=10;r=1;
	  break;
	case 5:
	  phi=1;psi=1.4;theta=-.1;
	  xo=0;yo=0;zo=7;
	  d=10;r=1;
	  break;
	case 6:
	  phi=1.4;psi=1.5;theta=-.3;
	  xo=0;yo=0;zo=7;
	  d=10;r=1;
	  break;
	default:
	  break;
	}
    }
  else if(fichier_actuel==Ecanard) //canard
    {
      switch(n)
	{
	case 0:
	  phi=0;psi=1.0472;theta=0.628319;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	case 1:
	  phi=2;psi=0.0;theta=1.0;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	case 2:
	  phi=-2.2;psi=0.0;theta=-2.0;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	case 3:
	  phi=-1.0;psi=0.0;theta=-2.0;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	case 4:
	  phi=-1.0;psi=0.0;theta=-1.57;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	case 5:
	  phi=-1.0;psi=0.0;theta=1.57;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	default:
	  break;
	}
    }  
  else if(fichier_actuel==Ecrane) // crane
    {
      printf("crane!!!(%d)\n",n);
      switch(n)
	{
	case 0:
	  phi=0;psi=1.0472;theta=0.628319;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	case 1:
	  phi=1.05;psi=0.0;theta=0.628319;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	case 2:
	  phi=1.5;psi=0.0;theta=0.0;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	case 3:
	  phi=1.57;psi=-1.57;theta=0.0;
	  xo=0;yo=0;zo=3000;
	  d=10;r=1;
	  break;
	default:
	  break;
	}
    }
  else if(fichier_actuel==Ecrane)// voiture
    {
      phi=0;psi=1.0472;theta=0.628319;
      xo=0;yo=0;zo=3000;
      d=10;r=1;
    }
  else if(fichier_actuel==Esphere)
    {
      phi=0.8;psi=0.82;theta=0.82;
      xo=0;yo=0;zo=200;
      d=10;r=1;
    }
  else if(fichier_actuel==Eparabole)
    {
      phi=0.8;psi=0.82;theta=0.82;
      xo=0;yo=0;zo=200;
      d=10;r=1;
    }
  else if(fichier_actuel==Esurface)
    {
      phi=0.8;psi=0.82;theta=0.82;
      xo=0;yo=0;zo=200;
      d=10;r=1;
    }
  else if(fichier_actuel==Ebezier1)
    {
      phi=0.8;psi=0.82;theta=0.82;
      xo=0;yo=0;zo=200;
      d=10;r=1;
    }
  else if(fichier_actuel==Etest1) // test1
    {
      switch(n)
	{
	case 0:
	  //phi=0.72;psi=1.7872;theta=1.36832;
	  //phi=0.72;psi=1.8072;theta=1.38832;
	  phi=0.72;theta=1.40832;psi=1.8272;
	  xo=0;yo=0;zo=30;
	  d=10;r=1;
	  break;
	case 1:
	  phi=0;psi=1.0472;theta=0.628319;
	  xo=0;yo=0;zo=30;
	  d=10;r=1;
	  break;
	default:
	  break;
	}
    }
  else // fichier_actuel==Eautre
    {
      phi=0;psi=1.0472;theta=0.628319;
      xo=0;yo=0;zo=7;
      d=10;r=1;
    }
  */  
  //initialiser_affichage();
}

// initialise l'affichage et calcul la cloture
void initialiser_affichage()
{
  int i;
  double minx,maxx,miny,maxy;

  minx=maxx=miny=maxy=0;
  /*
    printf("on va initialiser M15 avec\n");
    printf("xo=%.1lf yo=%.1lf zo=%.1lf\n",xo,yo,zo);
    printf("phi=%.2lf theta=%.2lf psi=%.2lf\n",phi,theta,psi);
    printf("et Mt avec\n");
    printf("d=%.1lf r=%.1lf\n",d,r);
  */
  //printf("tout2\n");
  
      remplir_M15(xo,yo,zo,phi,theta,psi);//affiche_mat(M15,"M15");
      remplir_Mt(d,r);//affiche_mat(Mt,"Mt");
      printf("tout3\n");
      changement_repere(objet); //utilise ns, so, M15
      printf("tout6\n");
      if(perspective==1)
	{
	  transformation_perpective(objet); //utilise ns, so, tso, Mt
	  printf("tout7\n");
	  objet_cartesiennes(objet); //utilise ns, tso
	}
      else
	{
	  transformation_orthogonale(objet);
	}
      //objet_cartesiennes(objet); //utilise ns, tso
  printf("tout1\n");
  for(i=0;i<objet->ns;i++)
    {
      if(objet->tso[i].x<minx)
	minx=objet->tso[i].x;
      if(objet->tso[i].x>maxx)
	maxx=objet->tso[i].x;
      if(objet->tso[i].y<miny)
	miny=objet->tso[i].y;
      if(objet->tso[i].y>maxy)
	maxy=objet->tso[i].y;
    }
  printf("tout4\n");
  clo.min.x=minx;
  //clo.min.y=0;
  clo.min.y=miny;
  //clo.max.x=largeur();
  clo.max.x=maxx;
  //clo.max.y=hauteur();
  clo.max.y=maxy;
printf("tout5\n");
  calculcoeff(clo,fen2);
}

// initialise pour la 1ere fois les variables globales
// et lecture d'un cube
void initialisation()
{
  Tobjet *obj;
  
  efface_objet(objet);
  printf("M1\n");
  /****************************/
  obj=cree_objet("cube2.txt",Efichier,Ecube);
  ajoute_config(obj,0/*xo*/,0/*yo*/,7/*zo*/,0/*phi*/,0.628319/*theta*/,1.0472/*psi*/,
		1/*r*/,10/*d*/,0/*min_v*/,0/*max_v*/,0/*min_u*/,0/*max_u*/,0/*nb_u*/,0/*nb_v*/);
  //ajoute_config(obj,0,0,7,0,0.628319,1.0472,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
   printf("M1\n");
  obj=cree_objet("avion",Efichier,Eavion);
  ajoute_config(obj,0,0,7,0,1.0472,0.628319,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,0,1.0,1.0472,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,0,1.5,1.0472,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,1,.5,1.4,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,0,1.2,1.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,1,-.1,1.4,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,1.4,-.3,1.5,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
   printf("M1\n");
  obj=cree_objet("canard",Efichier,Ecanard);
  ajoute_config(obj,0,0,3000,0,.628319,1.0472,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,2,1,0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,-2.2,-2.0,0.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,-1,-2,0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,-1,-1.57,0.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,-1,1.57,0.0,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
 printf("M1\n");
  obj=cree_objet("crane",Efichier,Ecrane);
  ajoute_config(obj,0,0,3000,0,0.628319,1.0472,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,1.5,0.628319,0.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,1.5,0.0,0.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,1.57,0.0,-1.57,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
 printf("M1\n");
  obj=cree_objet("voiture",Efichier,Evoiture);
  ajoute_config(obj,0,0,3000,0.0,0.628319,1.0472,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
 printf("M2\n");
  obj=cree_objet(NULL,Ecourbe,Esphere);printf("M3\n");
  ajoute_config(obj,0,0,200,0.8,0.82,.82,1,10,0,0,0,0,0,0);printf("M4\n");
  liste_config=ajoute_objet(liste_config,obj);printf("M5\n");
   printf("M1\n");
  obj=cree_objet(NULL,Ecourbe,Eparabole);
  ajoute_config(obj,0,0,200,0.8,0.82,.82,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
 printf("M1\n");
  obj=cree_objet(NULL,Ecourbe,Esurface);
  ajoute_config(obj,0,0,200,0.8,0.82,.82,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
 printf("M1\n");
 obj=cree_objet(NULL,Ecourbe,Ebezier);
  ajoute_config(obj,0,0,200,0.8,0.82,.82,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
  obj=cree_objet("test1.txt",Efichier,Etest1);
  ajoute_config(obj,0,0,30,0.72,1.40832,1.8272,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,30,0,0.628319,1.0472,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
  /***********************************/
  //  strcpy(nom_fichier,liste_fichier[0]/*"cube2.txt"*/);
  //fichier_actuel=Ecube;
  
  //strcpy(nom_fichier,liste_fichier[5]/*"cube2.txt"*/);
  fichier_actuel=Etest1;
  obj=recherche_objet(liste_config,fichier_actuel);
  STOP(obj!=NULL,printf("err"));
  
  strcpy(nom_fichier,obj->nom);

  objet=lecture_objet2(nom_fichier);

  if(objet==NULL)
    {
      printf("Erreur\n");
      return;
    }
  else
    {/*
      affiche_objet(objet);
      printf("Ok\n");
      //getchar();
      //objet=lecture_objet2("cube2.txt");
      affiche_objet(objet);
      ctl=lecture_pt_control("pt_control.txt");
     */
      etat=Efichier;
      //affiche_objet(objet);
      //getchar();
    }

  //return 0;
  //printf("salut\n");
  //openpl();
  
  /*  
  //clo.min.x=0;
  clo.min.x=-3;
  //clo.min.y=0;
  clo.min.y=-3;
  //clo.max.x=largeur();
  clo.max.x=5;
  //clo.max.y=hauteur();
  clo.max.y=5;
  */

  fen2.bas.c=0;fen2.bas.l=0;
  fen2.haut.c=largeur();fen2.haut.l=hauteur();

  //calculcoeff(clo,fen2);
  // parametres par defaut

  /*xo=0.0; yo=0.0; zo=7.0;
  phi=0.0; theta=M_PI/5; psi=M_PI/3;
  perspective=1;
  d=10.0;r=1.0;
  */
  choix_coordonnees(0);
  relatif=0;

  initialiser_affichage();

  //afficher();
}

double sphere1(double u,double v)
{
  //return u;
  return u*cos(v);
}

double sphere2(double u,double v)
{
  //return v;
  return u*sin(v);
}

double sphere3(double u,double v)
{
  //return u*u+v*v;
  return u*u;
}

double parabole1(double u,double v)
{
  return cos(u)*cos(v);
}

double parabole2(double u,double v)
{
  return cos(u)*sin(v);
}

double parabole3(double u,double v)
{
  return sin(u);
}

double surface1(double u,double v)
{
  //return 3*u+v;
  return sin(u/30)+cos(v/10)*cos(u/100);
}

double surface2(double u,double v)
{
  return u;
}

double surface3(double u,double v)
{
  return v;
}

// reanalyse l'objet a afficher
void relecture_objet(void)
{
  printf("relecture1\n");
  efface_objet(objet);printf("relecture1\n");
  if(etat==Ecourbe)
    {
      printf("relecture2\n");
      objet=triangulation(fonc1,fonc2,fonc3,
			  min_u,max_u,min_v,max_v,nb_u,nb_v);
      printf("relecture3\n");
    }
  else if(etat==Ebezier)
    {
      lecture_pt_control(nom_fichier);
      objet=triangulation(fonc_pt_contr1,fonc_pt_contr2,fonc_pt_contr3,
			  min_u,max_u,min_v,max_v,nb_u,nb_v);
    }
  else
    {
      printf("relecture4\n");
      //if(strcmp(nom_fichier,"cube2.txt")==0)
      if(0)
	objet=lecture_objet(nom_fichier);
      else
	objet=lecture_objet2(nom_fichier);
      printf("relecture5\n");
    }
  if(objet==NULL)
    {
      printf("Erreur\n");
      return;
    }
  initialiser_affichage();
}

void menu()
{
  float meilleur_psi,meilleur_theta;
  clock_t t1,t2,max_temp;
  int choix;
  int fin=0,i,tmp;
  struct sigaction sa_old,sa_new;
  double phi2,tmp2;
  char c,s[5];
  Tobjet *obj;

  objet=NULL;
  printf("coucou\n");
  initialisation();
  openpl();
  while(fin == 0)
    {
      if(etat==Ecourbe)
	{
	  printf("Courbe Parametree\n");
	}
      else
	{
	  printf("Fichier %s\n",nom_fichier);
	}
      if(etat_affiche==Etransparent)
	printf("Affichage en fils de fer ");
      else
	printf("Affichage avec face cachees ");
      if(perspective==1)
	printf("en perspective\n");
      else
	printf("en projection orthogonale\n");
      printf("Point de vise: (%g,%g,%g)\n",xo,yo,zo);
      printf("phi=%g, theta=%g, psi=%g\n",phi,theta,psi);
      printf("distance de projection: %.2g\n",d);
      printf("distance point de vue - point de visee actuel: %.2g\n",r);
      if(relatif==1)
	printf("Modification relative\n");
      else
	printf("Modification en absolu\n");
      printf("vous pouvez choisir :\n");
      printf(" 0.quitter\n");
      printf(" 1.afficher\n");
      printf(" 2.autre fichier\n");
      printf(" 3.changer point de visee\n");
      printf(" 4.changer angles point de vue\n");
      printf(" 5.changer projection ortho/perspect\n");
      printf(" 6.changer distance pt vue\n");
      printf(" 7.changer distance projection\n");
      printf(" 8.changer de vue a partir de la position courante/initiale\n");
      printf(" 9.changer de visualisation (fils de fer/ plein)\n");
      printf(" 10.choisir une equation parametrique\n");
      printf(" 11.fais tourner!\n");
      printf(" 12.parametres predefinis\n");
      printf(" 13.choisir une courbe de bezier\n");
      printf("choix:");
      scanf("%d",&choix);
      switch(choix)
	{
	case 0: // fin du programme
	  printf("fin du programme\n");
	  fin=1;
	  break;
	  
	case 1: // affichage de l'objet
	  if(objet!=NULL)
	    {
	      affiche_objet(objet);
	      afficher();
	    }
	  break;
	case 2: // lecture d'un nouvel objet
	  printf(" fichier actuel: %s\nQuel objet voulez-vous"
		 " visualiser ? :\n",nom_fichier);
	  printf("0. Annulation\n");
	  printf("1. Cube\n");
	  printf("2. Avion\n");
	  printf("3. Canard\n");
	  printf("4. Crane\n");
	  printf("5. Voiture\n");
	  printf("6. Test1\n");
	  printf("7. Autre\n");
	  printf("Choisissez:");
	  scanf("%d",&choix);
	  switch(choix)
	    {
	    case 1: // cube
	      fichier_actuel=Ecube;
	      obj=recherche_objet(liste_config,fichier_actuel);
	      STOP(obj!=NULL,printf("Err"));
	      strcpy(nom_fichier,obj->nom);
	      choix_coordonnees(0);
	      break;
	    case 2: //avion
	      fichier_actuel=Eavion;
	      obj=recherche_objet(liste_config,fichier_actuel);
	      STOP(obj!=NULL,printf("Err"));
	      strcpy(nom_fichier,obj->nom);
	      choix_coordonnees(0);
	      break;
	    case 3: //canard
	      fichier_actuel=Ecanard;
	      obj=recherche_objet(liste_config,fichier_actuel);
	      STOP(obj!=NULL,printf("Err"));
	      strcpy(nom_fichier,obj->nom);
	      choix_coordonnees(0);
	      break;
	    case 4: // crane
	      fichier_actuel=Ecrane;
	      obj=recherche_objet(liste_config,fichier_actuel);
	      STOP(obj!=NULL,printf("Err"));
	      strcpy(nom_fichier,obj->nom);
	      choix_coordonnees(0);
	      break;
	    case 5: // voiture
	      fichier_actuel=Evoiture;
	      obj=recherche_objet(liste_config,fichier_actuel);
	      STOP(obj!=NULL,printf("Err"));
	      strcpy(nom_fichier,obj->nom);
	      choix_coordonnees(0);
	      break;
	    case 6:
	      fichier_actuel=Etest1;
	      obj=recherche_objet(liste_config,fichier_actuel);
	      STOP(obj!=NULL,printf("Err"));
	      strcpy(nom_fichier,obj->nom);
	      choix_coordonnees(0);
	      break;
	    case 7: // autre
	      fichier_actuel=Eautre;
	      printf("Entrez le fichier:");
	      scanf("%s",&nom_fichier);
      	      choix_coordonnees(0);
	      break;
	    default:
	      break;
	    }
	  if(choix<1||choix>7)
	    break;
	  relecture_objet();
	  break;
	  
	case 3: // changement du point de vise
	  if(relatif)
	    printf("Modification relative\n");
	  printf("actuel point de visee: (%.2lf,%.2lf,%.2lf)\nnouveau:\n",
		 xo,yo,zo);
	  printf("xo:");
	  scanf("%lf",&tmp2);
	  if(relatif)
	    xo+=tmp2;
	  else
	    xo=tmp2;
	  printf("yo:");
	  scanf("%lf",&tmp2);
	  if(relatif)
	    yo+=tmp2;
	  else
	    yo=tmp2;
	  printf("zo:");
	  scanf("%lf",&tmp2);
	  if(relatif)
	    zo+=tmp2;
	  else
	    zo=tmp2;
	  relecture_objet();
	  break;
	  
	case 4: // changement de l'angle
	  if(relatif)
	    printf("Modification relative\n");
	  printf("angles actuels: (%.2lf,%.2lf,%.2lf)\nnouveaux:\n",
		 phi,theta,psi);
	  printf("phi:");
	  scanf("%lf",&tmp2);
	  if(relatif)
	    phi+=tmp2;
	  else
	    phi=tmp2;
	  printf("theta:");
	  scanf("%lf",&tmp2);
	  if(relatif)
	    theta+=tmp2;
	  else
	    theta=tmp2;
	  printf("psi:");
	  scanf("%lf",&tmp2);
	  if(relatif)
	    psi+=tmp2;
	  else
	    psi=tmp2;
	  relecture_objet();
	  break;
	  
	case 5://changer projection ortho/perspect
	  perspective=(perspective==1)?0:1;
	  relecture_objet();
	  break;
	  
	case 6: // changement de la distance point de vue-point de visee
	  if(relatif)
	    printf("Modification relative\n");
	  printf("point de vue - point de visee actuel: %.2lf\nnouveau :",r);
	  scanf("%lf",&tmp2);
	  if(relatif)
	    r+=tmp2;
	  else
	    r=tmp2;
	  relecture_objet();
	  break;
	  
	case 7: // changement de la distance de projection
	  if(relatif)
	    printf("Modification relative\n");
	  printf("distance de projection actuelle: %.2lf\nnouveau :",d);
	  scanf("%lf",&tmp2);
	  if(relatif)
	    d+=tmp2;
	  else
	    d=tmp2;
	  relecture_objet();
	  
	  break;
	  
	case 8: // changer de modification relatif/absolu
	  relatif=(relatif==0)?1:0;
	  break;
	  
	case 9: // changer de visualisation
	  etat_affiche=(etat_affiche==Etransparent)?Ecache:Etransparent;
	  break;
	  
	case 10: // choix de courbes parametres
	  printf("sphere\n");
	  min_v=-M_PI;max_v=M_PI;
	  min_u=-M_PI;max_u=M_PI;
	  nb_v=50;nb_u=50;
	  printf("Quelle courbe voulez-vous ? :\n");
	  printf(" 0.annuler\n");
	  printf(" 1.parabole\n");
	  printf(" 2.sphere\n");
	  printf(" 3.surface\n");
	  printf(" Choisissez:");
	  scanf("%d",&choix);
	  switch(choix)
	    {
	    case 0:
	      break;
	    case 1:
	      fichier_actuel=Eparabole;
	      fonc1=sphere1;
	      fonc2=sphere2;
	      fonc3=sphere3;
	      break;
	    case 2:
	      fichier_actuel=Esphere;
	      fonc1=parabole1;
	      fonc2=parabole2;
	      fonc3=parabole3;
	      break;
	    case 3:
	      fichier_actuel=Esurface;
	      fonc1=surface1;
	      fonc2=surface2;
	      fonc3=surface3;
	      break;
	    default:
	      break;
	    }
	  if(choix<1||choix>3)
	    break;
	  fflush(stdin);
	  printf("Les parametres sont:\n");
	  printf("u min=%g, u max=%g, v min=%g, v max=%g\n",min_u,max_u,
		 min_v,min_u);
	  printf("Nombre de points en u:%d\nNombre de point en v:%d\n",nb_u,nb_v);
	  printf("Voulez-vous les changer ? (o/n):");
	  fflush(stdin);
	  scanf("%s",s);
	  if(s[0]=='o')
	    {
	      printf("u min:");
	      scanf("%g",&min_u);
	      printf("u max:");
	      scanf("%g",&max_u);
	      printf("v min:");
	      scanf("%g",&min_v);
	      printf("v max:");
	      scanf("%g",&max_v);
	      printf("Nombre de u:");
	      scanf("%d",&nb_u);
	      printf("Nombre de v:");
	      scanf("%d",&nb_v);
	    }
	  relecture_objet();
	  objet=triangulation(fonc1,fonc2,fonc3,
			      min_u,max_u,min_v,max_v,nb_u,nb_v);
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      break;
	    }
	  etat=Ecourbe;
	  choix_coordonnees(0);
	  initialiser_affichage();
	  break;
	  
	case 11: // pivotage de la vue
	  printf("Pivotage...\n");
	  /*
	  sa_new.sa_handler=catch_sigint;
	  sigemptyset(&sa_new.sa_mask);
	  sa_new.sa_flags=0;
	  sigaction(SIGINT,&sa_new,&sa_old);
	  */
	  openpl();
	  max_temp=0;
	  fin_tourner=0;
	  for(phi2=0;phi2<2*M_PI;phi+=.02,phi2+=.02)
	    {
	      theta+=.02;
	      psi+=.02;
	      relecture_objet();
	      t1=clock();
	      erase();
	      afficher();
	      t2=clock()-t1;
	      if(t2>max_temp)
		{
		  meilleur_theta=theta;
		  meilleur_psi=psi;
		  max_temp;
		}
	      printf("phi=%g theta=%g psi=%g\n",phi,theta,psi);
	      t1=clock();
	      while(clock()-t1<20000);
	      if(fin_tourner==1)
		{
		  break;
		}
	    }
	  //sigaction(SIGINT,&sa_old,&sa_new);
	  printf("meilleur choix: psi=%lf theta=%lf\n",
		 meilleur_psi,meilleur_theta);
	  break;
	case 12:
	  printf("Init\n");
	  obj=recherche_objet(liste_config,fichier_actuel);
	  STOP(obj!=NULL,printf("Err"));
	  printf("Choisissez les parametres predefinies que vous voulez (1-%d):",
		 obj->nb_config);
	  scanf("%d",&choix);
	  if(choix>0&&choix<=obj->nb_config)
	    {
	      printf("Ok\n");
	      choix_coordonnees(choix-1);
	      relecture_objet();
	    }
	  printf("suite(%d,%d,%d)\n",choix,
		 fichier_actuel,obj->nb_config);
	  getchar();
	  break;
	case 13:
	  min_v=0.0;max_v=1;
	  min_u=0.0;max_u=1;
	  nb_v=50;nb_u=50;
	  printf("Quelle courbe voulez-vous ? :\n");
	  printf(" 0.annuler\n");
	  printf(" 1.choix1\n");
	  printf(" Choisissez:");
	  scanf("%d",&choix);
	  switch(choix)
	    {
	    case 0:
	      break;
	    case 1:
	      fichier_actuel=Ebezier1;
	      break;
	    default:
	      break;
	    }
	  if(choix<1||choix>1)
	    break;
	  fflush(stdin);
	  strcpy(nom_fichier,"pt_control");
	  printf("Les parametres sont:\n");
	  /*printf("u min=%g, u max=%g, v min=%g, v max=%g\n",min_u,max_u,
	    min_v,min_u);*/
	  printf("Nombre de points en u:%d\nNombre de point en v:%d\n",nb_u,nb_v);
	  printf("Voulez-vous les changer ? (o/n):");
	  fflush(stdin);
	  scanf("%s",s);
	  if(s[0]=='o')
	    {
	      /*printf("u min:");
	      scanf("%g",&min_u);
	      printf("u max:");
	      scanf("%g",&max_u);
	      printf("v min:");
	      scanf("%g",&min_v);
	      printf("v max:");
	      scanf("%g",&max_v);*/
	      printf("Nombre de u:");
	      scanf("%d",&nb_u);
	      printf("Nombre de v:");
	      scanf("%d",&nb_v);
	    }
	  etat=Ebezier;
	  relecture_objet();
	  objet=triangulation(fonc_pt_contr1,fonc_pt_contr2,fonc_pt_contr3,
			      min_u,max_u,min_v,max_v,nb_u,nb_v);
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      break;
	    }
	  //etat=Eparam;
	  choix_coordonnees(0);
	  initialiser_affichage();

	  break;
	default:
	  printf("erreur dans le menu\n");
	  break;
	}
    }
  closepl();
  efface_objet(objet);
  efface_configs(liste_config);
}

int main(int argc,char *argv[])
{
  menu();
  return 0;
}
