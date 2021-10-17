#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<string.h>
#include"type.h"
#include"lexer.h"
#include"affich_rec.h"
#include"debugage.h"
#include"erreur.h"
#include"verif.h"
#include"gc.h"

/*extern LS *liste;
extern LS *liste_dernier;
extern LS *dernier_marque;
extern ST *programme;*/

ST *programme=NULL;
LS *liste=NULL,*liste_dernier=NULL,*dernier_marque=NULL;


void affiche_LS(void);
TYPE creer_type(enum ETYPE type,int taille);

void affiche(ST *p)
{
  CHECK_MSG(p!=NULL,("L'arbre est vide"));
  
  printf("Affichage de l'arbre abstrait:\n");
  
  //nb_tab=0;
  //affiche_list_fonc(p->listef);
  
  affichage_rec(p);
  
  affiche_LS();
}

/***************** fonctions pour construire la liste LS *********************/

/* remplit la structure ls qui doit avoir deja ete alloue */
void complete_LS(LS *ls,void *e,void (*free1)(void*e), 
		 void (*print)(void*e))
{
  CHECK_MSG(ls!=NULL,("Completement de ls vide"));  
  
  ls->contenu=e;
  ls->marque=0;
  ls->free1=free1;
  ls->print=print;
  ls->dernier_marque=NULL;
  ls->suivie_de=Erien;
  ls->parent_fermante=0;
  ls->retour_ligne=false;
  ls->parent_ouvrante=0;
}

// ajoute une structure a la var globale "liste"
void add_liste(void *e,void (*free1)(void*e), 
	       void (*print)(void*e))
{
  LS *tmp;

  tmp=(LS*)malloc(sizeof(LS));
  tmp->suivant=NULL;
  complete_LS(tmp,e,free1,print);

  if(liste==NULL)
    {
      liste=liste_dernier=tmp;
    }
  else
    {
      liste_dernier->suivant=tmp;
      liste_dernier=tmp;
    }
}

// ajoute un element vide a la structure liste et le retourne
LS *add_vide(void)
{
  add_liste(NULL,NULL,NULL);
  
  return liste_dernier;
}

/*************** fonctions d'affichage de l'arbre abstrait avec LS *****************/

// affiche l'arbre abstrait en parcourant la liste lineaire liste de type LS
void affiche_LS(void)
{
  LS *l;

  printf("Affichage avec LS de l'arbre abstrait :\n");
  l=liste;
  while(l!=NULL)
    {
    CHECK_MSG(l->print!=NULL,("vide"));
    //  if(l->print!=NULL)
	l->print(l->contenu);
    //  else
	//printf("vide");
	
	if(l->parent_ouvrante>0)
	{
		int i;
		for(i=0;i<l->parent_fermante;i++)
			printf("(");
	  }
            
      if(l->parent_fermante>0)
      {
		int i;
		for(i=0;i<l->parent_fermante;i++)
			printf(")");
	  }

      if(l->suivie_de==Evirgule)
	printf(",");
      else if(l->suivie_de==Ept_virgule)
	printf(";");
      
      if(l->retour_ligne==true)
	printf("\n");
      
      if(l->fin_bloc==true)
	printf(" end {}\n");
      
      if(l->suivie_else)
	printf(" else ");
      

      l=l->suivant;
    }
  printf("Fin affichage LS\n");
}

// affiche le nom d'une fonction passe en parametre de type STF
// la structure LS peut pointer sur cette fonction
static void affiche_fonct(void *e)
{
  STF *fonc;
  
  CHECK_MSG(e!=NULL,("fonction vide"));
  fonc=e;
  
  if(fonc->code==fonc_principale)
    printf("Program");
  else
    affiche_type(fonc->rtype);
  printf(" %s ",fonc->nom);
}

// affiche un parametre passe en parametre de type STFP
// la structure LS peut pointer sur cette fonction
static void affiche_param(void *e)
{
  STFP *param=e;
  
  if(e==NULL)
    return;
  
  affiche_type(param->type);
  printf(" ");
}

// affiche un bloc passe en parametre de type IB
// la structure LS peut pointer sur cette fonction
static void affiche_bloc1(void *e)
{
  IB *tmp;

  if(e==NULL)
    return;

  tmp=e;
  
  printf("begin {%s}",tmp->nom);
}

// affiche une instruction passe en parametre de type INSTR
// la structure LS peut pointer sur cette fonction
static void affiche_instr(void *e)
{
  INSTR *tmp;

  if(e==NULL)
    return;

  tmp=e;
  switch(tmp->code)
    {
    case code_affectation:
      printf("=");
      break;
    case code_if:
      printf(" if ");
      break;
    case code_for:
      printf(" for ");
      break;
    case code_appel_fonction:
      //printf("Appel fonction ");
      break;
    case code_instr_vide:
      //printf("Instruction vide ");
      break;
    case code_break:
      printf(" break ");
      break;
    case code_continue:
      printf(" continue ");
      break;
    case code_else:
      printf("else");
      break;
    case code_bloc:
      printf(" bloc ");
      break;
    case code_return:
      printf(" return ");
      break;
    default:
      printf("Instruction inconnue\n");
    }
}

// affiche une declaretion de var passe en parametre de type DECL
// la structure LS peut pointer sur cette fonction
static void affiche_decl(void *e)
{
  DECL *tmp;

  if(e==NULL)
    return;

  tmp=e;
  
  affiche_type(tmp->type);

  printf(" ");
}

// affiche une expression passe en parametre de type EXPR
// la structure LS peut pointer sur cette fonction
static void affiche_expr(void *e)
{
  EXPR *tmp;

  if(e==NULL)
    return;

  tmp=e;
  
  switch(tmp->code)
    {
    case code_var:
      printf(" %s ",tmp->nom);
      break;
     case code_plus:
      printf(" + ");
      break;
     case code_moins:
      printf(" - ");
      break;
    case code_mult:
      printf(" * ");
      break;
    case code_div:
      printf(" / ");
      break;
    case code_mod:
      printf(" % ");
      break;
    case code_et:
      printf(" && ");
      break;
    case code_ou:
      printf(" || ");
      break;
    case code_non:
      printf(" ! ");
      break;
    case code_moins_u:
      printf(" (- unaire) ");
      break;
    case code_chaine:
      printf(" %s ",tmp->nom);
      break;
    case code_sup:
      printf(" >= ");
      break;
    case code_sup_s:
      printf(" > ");
      break;
    case code_inf:
      printf(" <= ");
      break;
    case code_inf_s:
      printf(" < ");
      break;
    case code_egal:
      printf(" == ");
      break;
    case code_diff:
      printf(" != ");
      break;
    case code_true:
      printf(" true ");
      break;
    case code_false:
      printf(" false ");
      break;
    case code_appel:
      printf(" %s",tmp->nom);
      break;
    case code_map:
      printf(" map ");
      break;
    case code_unmap:
      printf(" unmap ");
      break;
    case code_load:
      printf(" load ");
      break;
    case code_unload:
      printf(" unload ");
      break;
    case code_val:
      printf(" %d ",tmp->valeur);
      break;
    default:
      printf("Expression inconnue ");
    }
}

/************** fonctions pour effacer LS et l'arbre abstrait **************/

/* efface tous les ls et l'arbre abstrait */
void efface_ls(void)
{
  LS *l,*l2;
  
  printf("Effacement de LS:\n");
  free(programme);
  programme=NULL;
  l=liste;
  while(l!=NULL)
    {
      CHECK_MSG(((l->free1!=NULL)&&(l->print!=NULL))||(nb_erreurs>0),
		("Erreur: Pas de fonction free**************"));
      /*l->print(l->contenu);
	printf("\n");*/
      
      if(l->free1!=NULL)
	{
	  l->free1(l->contenu);
	}
      
      l2=l->suivant;
      free(l);
      l=l2;
    }
  
  if(!table_symbol_vide())//table_symbol!=NULL)
    efface_table_symbol();
  
  gc();
  printf("Fin d'effacement de LS:\n");
}

static void efface_fonc(void *e)
{
  STF *f1;
  
  if(e==NULL)
    return;
  
  f1=(STF*)e;
  
  /*if(f1->code==fonc_principale)
    printf("effacement fonction principale:%s\n",f1->nom);
    else
    printf("effacement fonction non principale:%s\n",f1->nom);
  */
  
  if((int)f1->nom>4)
    free(f1->nom);
  
  free(f1);
}

static void efface_param(void *e)
{
  STFP *p;
  
  if(e==NULL)
    return;
  
  p=(STFP *)e;
  free(p);
}

static void efface_instr(void *e)
{
  INSTR *i;
  
  if(e==NULL)
    return;
  
  i=(INSTR *)e;
  free(i);
}

static void efface_decl(void *e)
{
  DECL *d;
  
  if(e==NULL)
    return;
  
  d=(DECL*)e;
  free(d);
}

static void efface_bloc(void *e)
{
  IB *ib;
  
  if(e==NULL)
    return;
  
  ib=(IB*)e;
  if(ib->nom!=NULL)
    free(ib->nom);
  
  if(ib->nom2!=NULL)
    free(ib->nom2);
  
  free(ib);
}

static void efface_expr(void *e)
{
  EXPR *exp;
  
  if(e==NULL)
    return;
  
  //printf("effacement d'une expression\n");
  exp=(EXPR*)e;
  //printf("a%s\n",NULL/*exp->nom*/);
  //printf("f=%p\n",exp);
  //printf("d=%s\n",exp->nom);
  //printf("e\n");
  if//(exp->nom!=NULL)
    ((int)(exp->nom)>4)
    free(exp->nom);
  //printf("b\n");
  free(exp);
  //printf("c\n");
}

/************** fonctions pour creer l'arbre abstrait *****************/

// cree une fonction pas principale
STF *cree_fonction(TYPE rtype,char *nom,STFP *param,IB *bloc,LS *ls,LS *ls2)
{
  STF *fonction;
  
  fonction=(STF *)malloc(sizeof(STF));
  fonction->par=param;
  fonction->rtype=rtype;
  fonction->nom=nom;
  fonction->ib=bloc;
  fonction->suivant=NULL;
  fonction->code=fonc_normale;
  fonction->no_ligne=no_ligne;
  
  if(ls!=NULL)
    {
      complete_LS(ls,fonction,efface_fonc,affiche_fonct);
      ls->parent_ouvrante++;
      ls2->parent_fermante++;
      ls2->retour_ligne=true;
    }
  
  return fonction;
}

// cree une fonction principale
STF *cree_fonction_principale(char *nom,IB *bloc,LS*ls)
{
  STF *fonction;
  
  fonction=(STF *)malloc(sizeof(STF));
  fonction->par=NULL;
  fonction->rtype=creer_type(Tvoid,-1);
  fonction->nom=nom;
  fonction->ib=bloc;
  fonction->suivant=NULL;
  fonction->code=fonc_principale;
  fonction->no_ligne=no_ligne;
		
  complete_LS(ls,fonction,efface_fonc,affiche_fonct);
  ls->retour_ligne=true;
  //ls->parent_ouvrante=true;
  
  return fonction;
}

// retourn la liste de fonction "liste" ajoute de la fonction "fonc"
STF *ajoute_fonct(STF *liste,STF *fonc)
{
  STF *p;
  
  if(fonc==NULL)
    {
      return liste;
    }
  else
    {
      if(liste==NULL) return fonc;
      p=liste;
      while(p->suivant!=NULL) p=p->suivant;
      p->suivant=fonc;
      return liste;
    }
}

// cree et retourn un parametre
STFP *creer_param(TYPE type,EXPR *exp,LS*ls)
{
  STFP*res;
  
  res=(STFP*)malloc(sizeof(STFP));
  res->exp=exp;
  res->type=type;
  res->next=NULL;
  res->no_ligne=no_ligne;
  if(ls!=NULL)
    {
      complete_LS(ls,res,efface_param,affiche_param);
      liste_dernier->suivie_de=Evirgule;
    }
  
  return res;
}

// ajoute a la liste de parametre "liste" le parametre "param" et le retourne
STFP *ajoute_param(STFP *liste,STFP *param)
{
  STFP *p;
  
  if(param==NULL)
    {
      return liste;
    }
  else
    {
      if(liste==NULL) return param;
      p=liste;
      while(p->next!=NULL) p=p->next;
      p->next=param;
      return liste;
    }
}

// cree et retourn un bloc d'instrutcion
IB *creer_bloc(char *nom,DECL *decl,INSTR *instr,char *nom2,LS*ls)
{
  IB *res;
  
  res=(IB*)malloc(sizeof(IB));
  res->nom=nom;
  res->nom2=nom2;
  res->declare=decl;
  res->listeinstr=(void*)instr;
  res->no_ligne=no_ligne;
  complete_LS(ls,res,efface_bloc,affiche_bloc1);
  liste_dernier->fin_bloc=true;
  ls->retour_ligne=true;
  
  return res;
}

// complete une liste de declaration en ajoutant le type "type"
// a chaque declaration
DECL *complete_decl(TYPE type,DECL *decl)
{
  DECL *d1,*d2;
  
  d2=decl;
  d1=d2;
  while(d1!=NULL)
    {
      d1->type=type;
      d1=d1->suivant;
    }
  
  return d2;
}

// cree et retourne une declaration
DECL *creer_declaration2(EXPR* exp,LS*ls)
{
  DECL *d1;
  
  d1=(DECL*)malloc(sizeof(DECL));
  d1->exp=exp;
  d1->suivant=NULL;
  d1->no_ligne=no_ligne;
  complete_LS(ls,d1,efface_decl,affiche_decl);
  liste_dernier->suivie_de=Ept_virgule;
  liste_dernier->retour_ligne=true;
  
  return d1;
}

/* ajoute a la liste de declaration "liste" la declaration "elt"
 et retourn la liste
 utilite: int a; bool b;
 devient une liste de declartion
*/
DECL *ajoute_declaration(DECL *liste,DECL *elt)
{
  DECL *d1,*d2;
  
  d1=d2=liste;
  if(d1==NULL)
    {
      return elt;
    }
  else
    {
      while(d1->suivant!=NULL) d1=d1->suivant;
      d1->suivant=elt;
      return d2;
    }
}

/* ajoute a la liste de declaration "liste" la variable "exp"
 en suposant que "exp" est une variable et retourne la liste resultat
 utilite: a,b,c 
 devient une liste de declaration
*/
DECL *ajoute_declaration2(DECL *liste,EXPR *exp,LS*ls)
{
  DECL *d1,*d2;
  
  d2=liste;
  d1=d2;
  while(d1->suivant!=NULL) d1=d1->suivant;
  d1->suivant=(DECL*)malloc(sizeof(DECL));
  d1=d1->suivant;
  d1->exp=exp;
  d1->suivant=NULL;
  d1->no_ligne=no_ligne;
  complete_LS(ls,d1,efface_decl,affiche_decl);
  liste_dernier->suivie_de=Ept_virgule;
  liste_dernier->retour_ligne=true;
  
  return d2;
}

// cree et retourne une variable
EXPR *creer_variable(char *nom,int no)
{
  EXPR *e;
  e=(EXPR*)malloc(sizeof(EXPR));
  e->nom=nom;
  e->valeur=no;
  e->no_ligne=no_ligne;
  add_liste(e,efface_expr,affiche_expr);
  e->ls=liste_dernier;
  e->marque=0;

  return e;
}

// retourn un type
TYPE creer_type(enum ETYPE type,int taille)
{
  TYPE t;
  
  t.type=type;
  t.taille=taille;
  t.no_ligne=no_ligne;
  return t;
}

// ajoute a la liste "liste" l'instruction "instr"
// et retourne la liste
INSTR *ajoute_instr(INSTR *liste,INSTR *instr)
{
  INSTR *i1,*i2;
  
  i1=i2=liste;
  if(i2!=NULL)
    {
      while(i1->suivant!=NULL) i1=i1->suivant;
      i1->suivant=instr;
      return i2;
    }
  else
    {
      return instr;
    }
}

// cree et retourne une instruction de bloc
INSTR *creer_instr_bloc(IB *bloc)
{
  INSTR *i1;
  
  i1=(INSTR*)malloc(sizeof(INSTR));
  i1->suivant=NULL;
  i1->code=code_bloc;
  i1->ib1=bloc;
  i1->no_ligne=no_ligne;
  add_liste(i1,efface_instr,affiche_instr);
  
  return i1;
}

// cree et retourne une instruction d'appel de fonction
INSTR *creer_instr_appel(EXPR *fonc/*,LS*ls*/)
{
  INSTR *i1;
  
  if(fonc!=NULL)
    {
      i1=(INSTR*)malloc(sizeof(INSTR));
      i1->code=code_appel_fonction;
      i1->e1=fonc;
      i1->suivant=NULL;
      i1->no_ligne=no_ligne;
      add_liste(i1,efface_instr,affiche_instr);
      liste_dernier->suivie_de=Ept_virgule;
      liste_dernier->retour_ligne=true;
      return i1;
    }
  else
    {
      return NULL;
    }
}

// cree et retourne une instruction continue
INSTR *creer_instr_continue()
{
  INSTR *i1;
  
  i1=(INSTR*)malloc(sizeof(INSTR));
  i1->code=code_continue;
  i1->suivant=NULL;
  i1->no_ligne=no_ligne;
  add_liste(i1,efface_instr,affiche_instr);
  liste_dernier->suivie_de=Ept_virgule;
  liste_dernier->retour_ligne=true;
  return i1;
}

// cree et retourne une instruction break
INSTR *creer_instr_break()
{
  INSTR *i1;
  
  i1=(INSTR*)malloc(sizeof(INSTR));
  i1->code=code_break;
  i1->suivant=NULL;
  i1->no_ligne=no_ligne;
  add_liste(i1,efface_instr,affiche_instr);
  liste_dernier->suivie_de=Ept_virgule;
  liste_dernier->retour_ligne=true;
  return i1;
}

// cree et retourne une instruction return
INSTR *creer_instr_return(EXPR *e,LS*ls)
{
  INSTR *i1;
  
  i1=(INSTR*)malloc(sizeof(INSTR));
  i1->code=code_return;
  i1->e1=e;
  i1->suivant=NULL;
  i1->no_ligne=no_ligne;
  complete_LS(ls,i1,efface_instr,affiche_instr);
  liste_dernier->suivie_de=Ept_virgule;
  liste_dernier->retour_ligne=true;
  
  return i1;
}

// cree et retourne une instruction vide
INSTR *creer_instr_vide()
{
  INSTR *i1;
  
  i1=(INSTR*)malloc(sizeof(INSTR));
  i1->code=code_instr_vide;
  i1->suivant=NULL;
  i1->no_ligne=no_ligne;
  add_liste(i1,efface_instr,affiche_instr);
  liste_dernier->suivie_de=Ept_virgule;
  liste_dernier->retour_ligne=true;
  return i1;
}

// cree et retourne une instruction if
INSTR *creer_if(EXPR *exp,IB *b1,IB *b2,LS*ls)
{
  INSTR *i1;
  
  i1=(INSTR*)malloc(sizeof(INSTR));
  i1->code=code_if;
  i1->suivant=NULL;
  
  i1->e1=exp;
  i1->ib1=b1;
  i1->ib2=b2;
  i1->no_ligne=no_ligne;
  complete_LS(ls,i1,efface_instr,affiche_instr);
  ls->parent_ouvrante++;
	
  return i1;
}

// cree et retourne une instruction for
INSTR *creer_for(INSTR *instr,EXPR *exp,INSTR *instr2,IB *bloc,LS *ls)
{
  INSTR *i1;
  
  i1=(INSTR*)malloc(sizeof(INSTR));
  i1->code=code_for;
  i1->suivant=NULL;
  i1->e1=exp;
  i1->instr1=instr;
  i1->instr2=instr2;
  i1->ib1=bloc;
  i1->no_ligne=no_ligne;
  complete_LS(ls,i1,efface_instr,affiche_instr);
  
  return i1;
}

// cree et retourne une instruction d'affectation
INSTR *creer_affectation(EXPR *var,EXPR *exp,LS*ls)
{
  INSTR *i1;
  
  i1=(INSTR*)malloc(sizeof(INSTR));
  i1->code=code_affectation;
  i1->suivant=NULL;
  i1->e1=var;
  i1->e2=exp;
  i1->no_ligne=no_ligne;
  complete_LS(ls,i1,efface_instr,affiche_instr);
  liste_dernier->suivie_de=Ept_virgule;
  liste_dernier->retour_ligne=true;
  
  return i1;
}

// cree et retourne un appel de fonction 
EXPR *creer_fonction(char *nom,EXPR *exp,LS*ls,LS *ls2)
{
  EXPR *e1;
  
  e1=(EXPR*)malloc(sizeof(EXPR));
  /* il faut convertir car il est de type (char*) */
  switch((int)nom)
    {
    case 1:
      e1->code=code_load;
      break;
    case 2:
      e1->code=code_unload;
      break;
    case 3:
      e1->code=code_map;
      break;
    case 4:
      e1->code=code_unmap;
      break;
    default: /* c'est une chaine de caractere */
      e1->code=code_appel;
      e1->nom=nom;
      break;
    }
  e1->e1=exp;
  e1->suivant=NULL;
  e1->no_ligne=no_ligne;
  
  complete_LS(ls,e1,efface_expr,affiche_expr);
  e1->ls=liste_dernier;
  e1->marque=0;
  ls2->parent_fermante++;
  ls->parent_ouvrante++;
	
  return e1;
}

// ajoute a la liste "liste" l'expression "exp" et la retourne
EXPR *ajoute_expression(EXPR *liste,EXPR *exp,LS*ls)
{
  EXPR *e1,*e2;
  
  CHECK_MSG(ls!=NULL,("ls NULL"));
  //CHECK_MSG()
  
  e1=e2=liste;
  if(exp!=NULL)
    exp->suivant=NULL;
  
  if((e1!=NULL))
    {
      ls->suivie_de=Evirgule;
      while(e1->suivant!=NULL) e1=e1->suivant;
      e1->suivant=exp;
      
      return e2;
    }
  else
    {
      return exp;
    }
}

// cree et retourne une expression binaire
EXPR *creer_exp_binaire(int code,EXPR *exp1,EXPR *exp2,LS*ls)
{
  EXPR *e1;
  
  e1=(EXPR*)malloc(sizeof(EXPR));
  e1->code=code;
  e1->e1=exp1;e1->e2=exp2;
  e1->suivant=NULL;
  e1->no_ligne=no_ligne;
  complete_LS(ls,e1,efface_expr,affiche_expr);
  e1->ls=liste_dernier;
  e1->marque=0;
  
  return e1;
}

// cree et retourn une expression unaire
EXPR *creer_exp_unaire(int code, EXPR *exp1,LS*ls)
{
  EXPR *e1;
  
  e1=(EXPR*)malloc(sizeof(EXPR));
  e1->code=code;
  e1->e1=exp1;
  e1->suivant=NULL;
  e1->no_ligne=no_ligne;
  complete_LS(ls,e1,efface_expr,affiche_expr);
  e1->ls=liste_dernier;
  e1->marque=0;
  
  return e1;
}

/* cree une chaine */
EXPR *creer_chaine(char *str)
{
  EXPR *e1;
  
  e1=(EXPR*)malloc(sizeof(EXPR));
  e1->code=code_chaine;
  e1->nom=str;
  e1->suivant=NULL;
  e1->no_ligne=no_ligne;
  add_liste(e1,efface_expr,affiche_expr);
  e1->ls=liste_dernier;
  e1->marque=0;
  return e1;
}

/* cree un nombre */
EXPR *creer_nombre(int nb)
{
  EXPR *e1;
  
  e1=(EXPR*)malloc(sizeof(EXPR));
  e1->code=code_val;
  e1->valeur=nb;
  e1->suivant=NULL;
  e1->no_ligne=no_ligne;
  add_liste(e1,efface_expr,affiche_expr);
  e1->ls=liste_dernier;
  e1->marque=0;
  return e1;
}

/* cree une expression booleen (true ou false) */
EXPR *creer_bool(int code)
{
  EXPR *e1;
  
  e1=(EXPR*)malloc(sizeof(EXPR));
  e1->code=code;
  e1->suivant=NULL;
  e1->no_ligne=no_ligne;
  add_liste(e1,efface_expr,affiche_expr);
  e1->ls=liste_dernier;
  e1->marque=0;
  return e1;
}
