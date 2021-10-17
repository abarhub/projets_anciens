#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include"3d.h"
#include"2d.h"
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
enum {Etransparent,Epeintre,Ezbuffer} etat_affiche=Etransparent;
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
	  conf=conf->suivant;
	}
      return conf;
    }
  else
    return NULL;
}

// affiche l'objet avec les parametres courants
void afficher()
{
  erase();
  switch(etat_affiche)
    {
    case Etransparent:// fils de fer
      visual_transpar(*objet);
      break;
    case Epeintre://  peintre
      visual_peintre(objet);
      break;
    default: // zbuffer
      z_buffer(objet);
      break;
    }
  flushpl();
}

// choisi les parametres predefinis numero 1, etc...
void choix_coordonnees(int n)
{
  Tconfig *conf;
 
  
  conf=recherche(liste_config,fichier_actuel,n);
  
  //STOP(conf!=NULL,printf("Err"));
  if(conf==NULL) conf=recherche(liste_config,Eautre,n);
  
  STOP(conf!=NULL,printf("Err"));
  
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
}

// initialise l'affichage et calcul la cloture
void initialiser_affichage()
{
  int i;
  double minx,maxx,miny,maxy;

  minx=maxx=miny=maxy=0;
  remplir_M15(xo,yo,zo,phi,theta,psi);
      remplir_Mt(d,r);
      changement_repere(objet);
      if(perspective==1)
	{
	  transformation_perpective(objet);
	  objet_cartesiennes(objet);
	}
      else
	{
	  transformation_orthogonale(objet);
	}

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

  clo.min.x=minx;
  clo.min.y=miny;
  clo.max.x=maxx;
  clo.max.y=maxy;

  calculcoeff(clo,fen2);
}

// initialise pour la 1ere fois les variables globales
// et lecture de l'objet representer par la variable fichier_actuel
void initialisation()
{
  Tobjet *obj;
  
  efface_objet(objet);

  /****************************/
  obj=cree_objet("cube2.txt",Efichier,Ecube);
  ajoute_config(obj,0/*xo*/,0/*yo*/,7/*zo*/,0/*phi*/,0.628319/*theta*/,1.0472/*psi*/,
		1/*r*/,10/*d*/,0/*min_v*/,0/*max_v*/,0/*min_u*/,0/*max_u*/,0/*nb_u*/,
		0/*nb_v*/);

  liste_config=ajoute_objet(liste_config,obj);

  obj=cree_objet("avion",Efichier,Eavion);
  ajoute_config(obj,0,0,7,0,1.0472,0.628319,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,0,1.0,1.0472,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,0,1.5,1.0472,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,1,.5,1.4,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,0,1.2,1.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,1,-.1,1.4,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,7,1.4,-.3,1.5,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);

  obj=cree_objet("canard",Efichier,Ecanard);
  ajoute_config(obj,0,0,3000,0,.628319,1.0472,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,2,1,0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,-2.2,-2.0,0.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,-1,-2,0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,-1,-1.57,0.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,-1,1.57,0.0,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);

  obj=cree_objet("crane",Efichier,Ecrane);
  ajoute_config(obj,0,0,3000,0,0.628319,1.0472,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,1.5,0.628319,0.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,1.5,0.0,0.0,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,3000,1.57,0.0,-1.57,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);

  obj=cree_objet("voiture",Efichier,Evoiture);
  ajoute_config(obj,0,0,3000,0.0,0.628319,1.0472,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);

  obj=cree_objet(NULL,Ecourbe,Esphere);
  ajoute_config(obj,0,0,200,0.8,0.82,.82,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);

  obj=cree_objet(NULL,Ecourbe,Eparabole);
  ajoute_config(obj,0,0,200,0.8,0.82,.82,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);

  obj=cree_objet(NULL,Ecourbe,Esurface);
  ajoute_config(obj,0,0,200,0.8,0.82,.82,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);

 obj=cree_objet(NULL,Ecourbe,Ebezier);
  ajoute_config(obj,0,0,200,0.8,0.82,.82,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
  
  obj=cree_objet("test1.txt",Efichier,Etest1);
  ajoute_config(obj,0,0,30,0.72,1.40832,1.8272,1,10,0,0,0,0,0,0);
  ajoute_config(obj,0,0,30,0,0.628319,1.0472,1,10,0,0,0,0,0,0);
  liste_config=ajoute_objet(liste_config,obj);
  
  obj=cree_objet("",Efichier,Eautre);
  ajoute_config(obj,0,0,7,0,0.628319,1.0472,
		1,10,0,0,0,0,0,0);

  liste_config=ajoute_objet(liste_config,obj);
  
  /***********************************/

  obj=recherche_objet(liste_config,fichier_actuel);
  STOP(obj!=NULL,printf("err"));
  
  if(fichier_actuel!=Eautre)
    strcpy(nom_fichier,obj->nom);

  objet=lecture_objet2(nom_fichier);

  if(objet==NULL)
    {
      printf("Erreur\n");
      return;
    }
  else
    {
      etat=Efichier;
    }

  fen2.bas.c=0;fen2.bas.l=0;
  fen2.haut.c=largeur();fen2.haut.l=hauteur();

  // parametres par defaut
  choix_coordonnees(0);
  relatif=0;

  initialiser_affichage();
}

// les 3 equation de la sphere
double sphere1(double u,double v)
{
  return u*cos(v);
}

double sphere2(double u,double v)
{
  return u*sin(v);
}

double sphere3(double u,double v)
{
  return u*u;
}

// les 3 equation de la parabole
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

// les 3 equation du 3eme objet parametre
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
  efface_objet(objet);
  if(etat==Ecourbe)
    {
      objet=triangulation(fonc1,fonc2,fonc3,
			  min_u,max_u,min_v,max_v,nb_u,nb_v);
    }
  else if(etat==Ebezier)
    {
      lecture_pt_control(nom_fichier);
      objet=triangulation(fonc_pt_contr1,fonc_pt_contr2,fonc_pt_contr3,
			  min_u,max_u,min_v,max_v,nb_u,nb_v);
    }
  else
    {
      objet=lecture_objet2(nom_fichier);
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
  clock_t t1,max_temp;
  int choix;
  int fin=0;
  double phi2,tmp2;
  char s[5];
  Tobjet *obj;
  
  objet=NULL;
  initialisation();
  openpl();
  while(fin == 0)
    {
      // affichage du menu
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
      else if(etat_affiche==Epeintre)
	printf("Affichage avec face cachees ");
      else
	printf("Affichage avec z-buffer ");
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
      printf(" 9.changer de visualisation (fils de fer/peintre/zbuffer)\n");
      printf(" 10.choisir une equation parametrique\n");
      printf(" 11.fais tourner!\n");
      printf(" 12.parametres predefinis\n");
      printf(" 13.choisir une courbe de bezier\n");
      printf("choix:");
      scanf("%d",&choix);
      
      // traitement du choix de l'utilisateur
      switch(choix)
	{
	case 0: // fin du programme
	  printf("fin du programme\n");
	  fin=1;
	  break;
	  
	case 1: // affichage de l'objet
	  if(objet!=NULL)
	    {
	      //affiche_objet(objet);
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
	    case 6: // test1.txt
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
	  printf("Affichage actuel: ");
	  switch(etat_affiche)
	    {
	    case Etransparent:
	      printf("fils de fer\n");
	      break;
	    case Epeintre:
	      printf("peintre\n");
	      break;
	    default:
	      printf("zbuffer\n");
	      break;
	    }
	  printf("0 annuler\n");
	  printf("1 fils de fer\n");
	  printf("2 peintre\n");
	  printf("3 zbuffer\n");
	  printf("Quelle visualisation voulez-vous?\n ");
	  scanf("%d",&choix);
	  switch(choix)
	    {
	    case 1: // fils de fer
	      etat_affiche=Etransparent;
	      break;
	    case 2: // peintre
	      etat_affiche=Epeintre;
	      break;
	    case 3: // zbuffer
	      etat_affiche=Ezbuffer;
	      break;
	    }
	  break;
	  
	case 10: // choix de courbes parametres
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
	    case 1: // la parabole
	      fichier_actuel=Eparabole;
	      fonc1=sphere1;
	      fonc2=sphere2;
	      fonc3=sphere3;
	      break;
	    case 2: // la sphere
	      fichier_actuel=Esphere;
	      fonc1=parabole1;
	      fonc2=parabole2;
	      fonc3=parabole3;
	      break;
	    case 3: // la surface
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
	      //t2=clock()-t1;
	      t1=clock();
	      while(clock()-t1<20000);
	    }
	  break;
	case 12: // selection des parametres predefinies
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
	  break;
	case 13: // courbe de bezier (ne marche pas)
	  break;
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
	  printf("Nombre de points en u:%d\nNombre de point en v:%d\n",nb_u,nb_v);
	  printf("Voulez-vous les changer ? (o/n):");
	  fflush(stdin);
	  scanf("%s",s);
	  if(s[0]=='o')
	    {
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
  
  if(argc==2)
    {
      fichier_actuel=Eautre;
      strcpy(nom_fichier,argv[1]);
    }
  else
    fichier_actuel=Etest1;
    
  menu();
  return 0;
}
