#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include"3d.h"

#include"2d.h"
#include<math.h>
#include<string.h>
#include<time.h>
//#include"lgx.c"

extern MATRICE M15,Mt;

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
enum ETAT {Efichier,Eparam} etat;
int nb_v,nb_u;
double min_v,max_v,min_u,max_u;
char *liste_fichier[]={"cube2.txt","avion","canard","crane","voiture"};
double (*fonc1)(double,double);
double (*fonc2)(double,double);
double (*fonc3)(double,double);
double sphere1(double u,double v);
double parabole1(double u,double v);
enum {Etransparent,Ecache} etat_affiche=Etransparent;
int perspective=1;

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

/*OBJET *objet;
  CLOTURE clo;
  FENETRE fen;
  POINT p1={-10.0,-10.0,0.0,1.0},p2={10.0,10.0,0.0,1.0};
  PT_CTL *ctl;*/

//if(argc!=2)
//    objet=lecture_objet2(nom_fichier/*"cube2.txt"*/);
/*else
  objet=lecture_objet(argv[1]);*/

void afficher()
{
  //printf("Affichage objet:\n");
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
  if(etat==Efichier)
    {
      if(strcmp(nom_fichier,liste_fichier[0])==0) // cube
	{
	  phi=0;psi=1.0472;theta=0.628319;
	  xo=0;yo=0;zo=7;
	  d=10;r=1;
	}
      else if(strcmp(nom_fichier,liste_fichier[1])==0) //avion
	{
	      phi=0;psi=1.0472;theta=0.628319;
	      xo=0;yo=0;zo=7;
	      d=10;r=1;
	}
      else if(strcmp(nom_fichier,liste_fichier[2])==0) //canard
	{
	      phi=0;psi=1.0472;theta=0.628319;
	      xo=0;yo=0;zo=3000;
	      d=10;r=1;
	}  
      else if(strcmp(nom_fichier,liste_fichier[3])==0) // crane
	{
	      phi=0;psi=1.0472;theta=0.628319;
	      xo=0;yo=0;zo=3000;
	      d=10;r=1;
	}
      else // voiture
	{
	      phi=0;psi=1.0472;theta=0.628319;
	      xo=0;yo=0;zo=3000;
	      d=10;r=1;
	}
    }
  else
    {
      if(fonc1==sphere1)
	{
	  phi=0.8;psi=0.82;theta=0.82;
	  xo=0;yo=0;zo=200;
	  d=10;r=1;
	}
      else
	{
	  phi=0.8;psi=0.82;theta=0.82;
	  xo=0;yo=0;zo=200;
	  d=10;r=1;
	}
    }
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
  */printf("tout2\n");
  
      remplir_M15(xo,yo,zo,phi,theta,psi);//affiche_mat(M15,"M15");
      remplir_Mt(d,r);//affiche_mat(Mt,"Mt");
      printf("tout3\n");
      changement_repere(objet); //utilise ns, so, M15
      printf("tout6\n");
      if(perspective==1)
	{
	  transformation_perpective(objet); //utilise ns, so, tso, Mt
	  printf("tout7\n");
	}
      else
	{
	  transformation_orthogonale(objet);
	}
      objet_cartesiennes(objet); //utilise ns, tso
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
  strcpy(nom_fichier,"cube2.txt");
  
  efface_objet(objet);
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
  xo=0.0; yo=0.0; zo=7.0;
  phi=0.0; theta=M_PI/5; psi=M_PI/3;
  perspective=1;
  d=10.0;r=1.0;

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

void relecture_objet(void)
{
  printf("relecture1\n");
  efface_objet(objet);printf("relecture1\n");
  if(etat==Eparam)
    {
      printf("relecture2\n");
      objet=triangulation(fonc1,fonc2,fonc3,
			  min_u,max_u,min_v,max_v,nb_u,nb_v);
      printf("relecture3\n");
    }
  else
    {
      printf("relecture4\n");
      objet=lecture_objet2(nom_fichier);
      printf("relecture5\n");
    }
}

void menu()
{
  float meilleur_psi,meilleur_theta;
  clock_t t1,t2,max_temp;
  int choix;
  int fin=0;
  struct sigaction sa_old,sa_new;

  objet=NULL;

  initialisation();
  
  while(fin == 0)
    {
      if(etat==Eparam)
	{
	  printf("Courbe Parametree\n");
	}
      else
	{
	  printf("Fichier %s\n",nom_fichier);
	}
      if(etat_affiche==Etransparent)
	printf("Affichage en fils de fer\n");
      else
	printf("Affichage avec face cachees\n");
      printf("Point de vise: (%g,%g,%g)\n",xo,yo,zo);
      printf("psi=%g, phi=%g, theta=%g\n",psi,phi,theta);
      printf("distance de projection: %.2g\n",d);
      printf("point de vue - point de visee actuel: %.2g\n",r);
      printf("vous pouvez choisir :\n");
      printf(" 0.quitter\n");
      printf(" 1.afficher\n");
      printf(" 2.autre fichier\n");
      printf(" 3.changer point de visee\n");
      printf(" 4.changer angles point de vue\n");
      printf(" 5.changer projection ortho/perspect\n");
      printf(" 6.changer distance pt vue\n");
      printf(" 7.changer distance projection\n");
      printf(" 8.changer de vue\n");
      printf(" 9.changer de visualisation\n");
      printf(" 10.choisir une equation parametrique\n");
      printf(" 11.fais tourner!\n");
      printf(" 12.parametres predefinis\n");
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
	      openpl();
	      afficher();
	      //sleep(1);
	      //closepl();
	      //PsDump();
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
	  printf("6. Autre\n");
	  printf("Choisissez:");
	  scanf("%d",&choix);
	  switch(choix)
	    {
	    case 1: // cube
	      strcpy(nom_fichier,liste_fichier[0]);
	      choix_coordonnees(0);
	      break;
	    case 2: //avion
	      strcpy(nom_fichier,liste_fichier[1]);
	      choix_coordonnees(0);
	      break;
	    case 3: //canard
	      strcpy(nom_fichier,liste_fichier[2]);
	      choix_coordonnees(0);
	      break;
	    case 4: // crane
	      strcpy(nom_fichier,liste_fichier[3]);
	      choix_coordonnees(0);
	      break;
	    case 5: // voiture
	      strcpy(nom_fichier,liste_fichier[4]);
	      choix_coordonnees(0);
	      break;
	    case 6: // autre
	      printf("Entrez le fichier:");
	      scanf("%s",&nom_fichier);
	      break;
	    default:
	      break;
	    }
	  printf("salut_tous4\n");
	  if(choix<1||choix>6)
	    break;
	  printf("salut_tous\n");
	  efface_objet(objet);printf("salut_tous2\n");
	  objet=lecture_objet2(nom_fichier);printf("salut_tous3\n");
	  if(objet==NULL)
	    {
	      printf("Erreur dans la lecture du fichier %s\n");
	      break;
	    }
	  else
	    {
	      initialiser_affichage();
	      //afficher();
	      etat=Efichier;
	    }
	  break;
	  
	case 3: // changement du point de vise
	  printf("actuel point de visee: (%.2lf,%.2lf,%.2lf)\nnouveau:\n",
		 xo,yo,zo);
	  printf("xo:");
	  scanf("%lf",&xo); //printf("xo=%.2lf\n",xo);
	  printf("yo:");
	  scanf("%lf",&yo); //printf("yo=%.2lf\n",yo);
	  printf("zo:");
	  scanf("%lf",&zo); //printf("zo=%.2lf\n",zo);
	  relecture_objet();
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  initialiser_affichage();
	  //afficher();
	  break;
	  
	case 4: // changement de l'angle
	  printf("angles actuels: (%.2lf,%.2lf,%.2lf)\nnouveaux:\n",
		 phi,theta,psi);
	  printf("phi:");
	  scanf("%lf",&phi);
	  printf("theta:");
	  scanf("%lf",&theta);
	  printf("psi:");
	  scanf("%lf",&psi);
	  relecture_objet();
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  initialiser_affichage();
	  break;
	  
	case 5://changer projection ortho/perspect
	  perspective=(perspective==1)?0:1;
	  break;
	  
	case 6: // changement du point de vue
	  printf("point de vue - point de visee actuel: %.2lf\nnouveau :",r);
	  scanf("%lf",&r);
	  relecture_objet();
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  initialiser_affichage();
	  break;
	  
	case 7: // changement de la distance de projection
	  printf("distance de projection actuelle: %.2lf\nnouveau :",d);
	  scanf("%lf",&d);
	  relecture_objet();
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  initialiser_affichage();
	  break;
	  
	case 8: // changer de vue
	  break;
	  
	case 9: // changer de visualisation
	  etat_affiche=(etat_affiche==Etransparent)?Ecache:Etransparent;
	  break;
	  
	case 10: // choix de courbes parametres
	  printf("sphere\n");
	  /*objet=triangulation(sphere1,sphere2,sphere3,
	    -M_PI,M_PI,-M_PI,+M_PI,5,5);*/
	  min_v=-M_PI;max_v=M_PI;
	  min_u=-M_PI;max_u=M_PI;
	  nb_v=50;nb_u=50;
	  printf("Quelle courbe voulez-vous ? :\n");
	  printf(" 0.annuler\n");
	  printf(" 1.parabole\n");
	  printf(" 2.sphere\n");
	  printf(" Choisissez:");
	  scanf("%d",&choix);
	  switch(choix)
	    {
	    case 0:
	      break;
	    case 1:
	      fonc1=sphere1;
	      fonc2=sphere2;
	      fonc3=sphere3;
	      break;
	    case 2:
	      fonc1=parabole1;
	      fonc2=parabole2;
	      fonc3=parabole3;
	      break;
	    default:
	      break;
	    }
	  if(choix<1||choix>2)
	    break;
	  objet=triangulation(fonc1,fonc2,fonc3,
			      min_u,max_u,min_v,max_v,nb_u,nb_v);
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      break;
	    }
	  etat=Eparam;
	  /*affiche_objet_ds_fichier(objet);
	    printf("./essai cree\n");*/
	  printf("init:\n");
	  choix_coordonnees(0);
	  initialiser_affichage();
	  break;
	  
	case 11: // pivotage de la vue
	  printf("Pivotage...\n");
	  sa_new.sa_handler=catch_sigint;
	  sigemptyset(&sa_new.sa_mask);
	  sa_new.sa_flags=0;
	  sigaction(SIGINT,&sa_new,&sa_old);
	  //printf("Pivotage2...\n");
	  openpl();
	  theta=0;
	  psi=0;
	  max_temp=0;
	  fin_tourner=0;
	  //printf("Pivotage3...\n");
	  for(phi=0;phi<2*M_PI;phi+=.02)
	    {
	      theta+=.02;
	      psi+=.02;//printf("Pivotage7...\n");
	      relecture_objet();
	      //printf("Pivotage8...\n");
	      t1=clock();
	      initialiser_affichage();
	      printf("Pivotage9...\n");
	      erase();//printf("Pivotage10...\n");
	      afficher();//printf("Pivotage11...\n");
	      t2=clock()-t1;
	      if(t2>max_temp)
		{
		  meilleur_theta=theta;
		  meilleur_psi=psi;
		  max_temp;
		}
	      /*if(kbhit())
		{
		printf("Touche!\n");
		getchar();
		}*///printf("Pivotage12...\n");
	      t1=clock();
	      while(clock()-t1<20000);//printf("Pivotage13...\n");
	      if(fin_tourner==1)
		{
		  break;
		}
	      /*else
		printf("coucou(%s)",c2);*/
	    }//printf("Pivotage5...\n");
	  sigaction(SIGINT,&sa_old,&sa_new);
	  printf("meilleur choix: psi=%lf theta=%lf\n",
		 meilleur_psi,meilleur_theta);
	  //printf("Pivotage6...\n");
	  break;
	case 12:
	  printf("Init\n");
	  choix_coordonnees(0);
	  break;
	default:
	  printf("erreur dans le menu\n");
	  break;
	}
    }
  closepl();
  efface_objet(objet);
}

int main(int argc,char *argv[])
{
  menu();
  return 0;
}
