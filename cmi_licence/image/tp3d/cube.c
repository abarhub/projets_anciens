#include<stdio.h>
#include"3d.h"
#include<math.h>

extern MATRICE M15,Mt;

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

int main(int argc,char *argv[])
{
  OBJET *objet;
  CLOTURE clo;
  FENETRE fen;
  POINT p1={-10.0,-10.0,0.0,1.0},p2={10.0,10.0,0.0,1.0};
  PT_CTL *ctl;

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









