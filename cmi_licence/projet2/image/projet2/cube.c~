#include<stdio.h>
#include"3d.h"
#include<math.h>
#include<string.h>

extern MATRICE M15,Mt;

char nom_fichier[50];
OBJET *objet;
CLOTURE clo;
FENETRE fen;
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
  visual_transpar(*objet);
  flushpl();
  
  affiche_objet(objet);
}

void initialiser_affichage()
{
  remplir_M15(xo,yo,zo,phi,theta,psi);//affiche_mat(M15,"M15");
  remplir_Mt(d,r);//affiche_mat(Mt,"Mt");
  changement_repere(objet);
  transformation_perpective(objet);
  objet_cartesiennes(objet);
}

void initialisation()
{
  strcpy(nom_fichier,"cube2.txt");
  
  
  objet=lecture_objet2(nom_fichier/*"cube2.txt"*/);
  if(objet==NULL)
    {
      printf("Erreur");
      return;
    }
  else
    {
      affiche_objet(objet);
      printf("Ok\n");
      //getchar();
      //objet=lecture_objet2("cube2.txt");
      affiche_objet(objet);
      ctl=lecture_pt_control("pt_control.txt");
    }

  //return 0;
  openpl();
  
  clo.min.x=-3;clo.min.y=-3;
  clo.max.x=3/*largeur()*/;clo.max.y=3/*hauteur()*/;
  
  fen.bas.c=0;fen.bas.l=0;
  fen.haut.c=largeur();fen.haut.l=hauteur();

  calculcoeff(clo,fen);

  xo=0.0; yo=0.0; zo=4.0;
  phi=0.0; theta=M_PI/5; psi=M_PI/3;

  d=10.0;r=1.0;

  /*remplir_M15(0.0,0.0,4.0,0.0,M_PI/5, M_PI/3);affiche_mat(M15,"M15");
  remplir_Mt(10.0,1.0);affiche_mat(Mt,"Mt");
  
  //printf("initial------------------------\n");
  //affiche_objet(objet);
  
  changement_repere(objet);
  //printf("apres chgt repere---------------\n");
  //affiche_objet(objet);

  transformation_perpective(objet);
  //printf("apres tr perpective--------------\n");
  //affiche_objet2(objet);

  //retour_cartesiennes(objet);
  objet_cartesiennes(objet);*/
  initialiser_affichage();
  //printf("apres objet_cartesiennes---------\n");
  //affiche_objet2(objet);
  afficher();
}

void menu()
{
  char c;
  int fin=0;

  initialisation();

  while(!fin)
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
  printf("choix:");
  scanf("%d",&c);
  switch(c)
    {
    case 0:
      printf("fin du programme\n");
      fin=1;
      break;
      
    case 1:
      afficher();
      break;
    case 2:
      printf("nom actuel: %s\nnouveau nom:",nom_fichier);
      scanf("%s",&nom_fichier);
      objet=lecture_objet2(nom_fichier/*"cube2.txt"*/);
      if(objet==NULL)
	{
	  printf("Erreur");
	  return;
	}
      else
	{
	  initialiser_affichage();
	  afficher();
	}
      break;

    case 3:
      printf("actuel point de visee: (%.2lf,%.2lf,%.2lf)\nnouveau:\n",
	     xo,yo,zo);
      printf("xo:");
      scanf("%lf",&xo);
      printf("yo:");
      scanf("%lf",&yo);
      printf("zo:");
      scanf("%lf",&zo);
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
      initialiser_affichage();
      break;

    case 5:
      break;

    case 6:
      printf("point de vue - point de visee actuel: %.2lf\nnouveau :",r);
      scanf("%lf",&r);
      initialiser_affichage();
      break;

    case 7:
      printf("distance de projection actuelle: %.2lf\nnouveau :",d);
      scanf("%lf",&d);
      initialiser_affichage();
      break;

    case 8:
      break;

    case 9:
      break;

    default:
      printf("erreur dans le menu\n");
    }
    }
  closepl();
}

int main(int argc,char *argv[])
{
  OBJET *objet;
  CLOTURE clo;
  FENETRE fen;
  POINT p1={-10.0,-10.0,0.0,1.0},p2={10.0,10.0,0.0,1.0};
  PT_CTL *ctl;

  menu();
  return 0;

  if(argc!=2)
    objet=lecture_objet2("cube2.txt");
  else
    objet=lecture_objet(argv[1]);
  
  if(objet==NULL)
    {
      printf("Erreur");
    }
  else
    {
      affiche_objet(objet);
      printf("Ok\n");
      //getchar();
      objet=lecture_objet2("cube2.txt");
      affiche_objet(objet);
      ctl=lecture_pt_control("pt_control.txt");
    }

  //return 0;
  openpl();
  
  clo.min.x=-3;clo.min.y=-3;
  clo.max.x=3/*largeur()*/;clo.max.y=3/*hauteur()*/;
  
  fen.bas.c=0;fen.bas.l=0;
  fen.haut.c=largeur();fen.haut.l=hauteur();

  calculcoeff(clo,fen);
  
  remplir_M15(0.0,0.0,4.0,0.0,M_PI/5, M_PI/3);affiche_mat(M15,"M15");
  remplir_Mt(10.0,1.0);affiche_mat(Mt,"Mt");
  
  printf("initial------------------------\n");
  affiche_objet(objet);
  
  changement_repere(objet);
  printf("apres chgt repere---------------\n");
  affiche_objet(objet);

  transformation_perpective(objet);
  printf("apres tr perpective--------------\n");
  affiche_objet2(objet);

  //retour_cartesiennes(objet);
  objet_cartesiennes(objet);
  printf("apres objet_cartesiennes---------\n");
  affiche_objet2(objet);
  
  printf("Affichage objet:\n");
  visual_transpar(*objet);
  flushpl();
  
  affiche_objet(objet);
  
  getchar();
  closepl();
}









