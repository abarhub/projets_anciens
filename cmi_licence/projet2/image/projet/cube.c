#include<stdio.h>
#include"3d.h"

#include"2d.h"
#include<math.h>
#include<string.h>
#include<time.h>
//#include"lgx.c"

extern MATRICE M15,Mt;

char nom_fichier[50];
OBJET *objet;
CLOTURE clo;
FENETRE fen2;
POINT p1={-10.0,-10.0,0.0,1.0},p2={10.0,10.0,0.0,1.0};
PT_CTL *ctl;
double xo,yo,zo,phi,theta,psi;
double r,d;


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
  visual_transpar(*objet); //utilise na, ar, tso
  flushpl();
  
  //affiche_objet(objet);
}

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
  remplir_M15(xo,yo,zo,phi,theta,psi);//affiche_mat(M15,"M15");
  remplir_Mt(d,r);//affiche_mat(Mt,"Mt");
  
  changement_repere(objet); //utilise ns, so, M15
  transformation_perpective(objet); //utilise ns, so, tso, Mt
  objet_cartesiennes(objet); //utilise ns, tso

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
  //clo.min.y=0;
  clo.min.y=miny;
  //clo.max.x=largeur();
  clo.max.x=maxx;
  //clo.max.y=hauteur();
  clo.max.y=maxy;

  calculcoeff(clo,fen2);
}

void initialisation()
{
  strcpy(nom_fichier,"cube2.txt");
  
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

  xo=0.0; yo=0.0; zo=7.0;
  phi=0.0; theta=M_PI/5; psi=M_PI/3;

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

void menu()
{
  float t1;
  int choix;
  int fin=0;

  initialisation();
  
  while(fin == 0)
    {
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
      printf("choix:");
      scanf("%d",&choix);
      switch(choix)
	{
	case 0:
	  printf("fin du programme\n");
	  fin=1;
	  break;
	  
	case 1:
	  affiche_objet(objet);
	  openpl();
	  afficher();
	  //sleep(1);
	  //closepl();
	  //PsDump();
	  break;
	case 2:
	  printf(" fichier actuel: %s\nnouveau fichier: ",nom_fichier);
	  scanf("%s",&nom_fichier);
	  objet=lecture_objet2(nom_fichier);
	  if(objet==NULL)
	    {
	      printf("Erreur");
	      return;
	    }
	  else
	    {
	      initialiser_affichage();
	      //afficher();
	    }
	  break;
	  
	case 3:
	  printf("actuel point de visee: (%.2lf,%.2lf,%.2lf)\nnouveau:\n",
		 xo,yo,zo);
	  printf("xo:");
	  scanf("%lf",&xo); //printf("xo=%.2lf\n",xo);
	  printf("yo:");
	  scanf("%lf",&yo); //printf("yo=%.2lf\n",yo);
	  printf("zo:");
	  scanf("%lf",&zo); //printf("zo=%.2lf\n",zo);
	  objet=lecture_objet2(nom_fichier);
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  initialiser_affichage();
	  //afficher();
	  break;
	  
	case 4:
	  printf("angles actuels: (%.2lf,%.2lf,%.2lf)\nnouveaux:\n",
		 phi,theta,psi);
	  printf("phi:");
	  scanf("%lf",&phi);
	  printf("theta:");
	  scanf("%lf",&theta);
	  printf("psi:");
	  scanf("%lf",&psi);
	  objet=lecture_objet2(nom_fichier);
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  initialiser_affichage();
	  break;
	  
	case 5:
	  break;
	  
	case 6:
	  printf("point de vue - point de visee actuel: %.2lf\nnouveau :",r);
	  scanf("%lf",&r);
	  objet=lecture_objet2(nom_fichier);
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  initialiser_affichage();
	  break;
	  
	case 7:
	  printf("distance de projection actuelle: %.2lf\nnouveau :",d);
	  scanf("%lf",&d);
	  objet=lecture_objet2(nom_fichier);
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  initialiser_affichage();
	  break;
	  
	case 8:
	  break;
	  
	case 9:
	  break;

	case 10:
	  printf("sphere\n");
	  objet=triangulation(sphere1,sphere2,sphere3,
			      -M_PI,M_PI,-M_PI,+M_PI,5,5);
	  if(objet==NULL)
	    {
	      printf("Erreur\n");
	      return;
	    }
	  affiche_objet_ds_fichier(objet);
	  printf("./essai cree\n");
	  //initialiser_affichage();
	  break;

	case 11:
	  openpl();
	  theta=0;
	  psi=0;
	  for(phi=0;phi<2*M_PI;phi+=.02)
	    {
	      theta+=.02;
	      psi+=.02;
	      objet=lecture_objet2(nom_fichier);
	      initialiser_affichage();
	      erase();
	      afficher();
	      t1=clock();
	      while(clock()-t1<20000);
	    }
	  break;
	default:
	  printf("erreur dans le menu\n");
	}
    }
  closepl();
}

int main(int argc,char *argv[])
{
  menu();
  return 0;
}









