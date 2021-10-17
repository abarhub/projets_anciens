#include<stdio.h>
#include"type.h"
#include "debugage.h"
#include"gc.h"

/*
ce module affiche l'arbre syntaxique
les fonctions exportes sont dans le fichier affich_rec.h
*/

/* c'est le nombre courant de tabulations
pour afficher une instruction */
static int nb_tab=0;

static void affiche_expression2(EXPR *e,int pri);

// affiche (nb_tab) tabulations
static void tab(void)
{
  int i;

  for(i=0;i<nb_tab;i++)
    printf("  ");
}

// affiche un type t
// cette fonction est exportee
void affiche_type(TYPE t)
{
  switch(t.type)
    {
    case Tint:
      printf("int");
      break;
    case Tvoid:
      printf("void");
      break;
    case Timg:
      printf("img");
      break;
    case Tstring:
      printf("string");
      break;
    case Tbool:
      printf("bool");
      break;
    case TerreurType:
      printf("type errone");
      break;
    default:
      STOP(("Type inconnue(%d,%d)",t.type,t.taille));
      break;
    }
  if(t.taille>-1)
    printf("[%d]",t.taille);
}

// affiche une expression
static void affiche_expression(EXPR *e)
{
  affiche_expression2(e,0);
}

// retourne le numero de priorite suivant le code de l'operation
static int priorite(int code)
{
 int no=0;
 switch(code)
 {
   case code_plus: return no+4;
   case code_moins: return no+4;
   case code_mult: return no+5;
   case code_div: return no+5;
   case code_mod: return no+5;
   case code_var: return no+7;
   case code_val: return no+7;
   case code_et: return no+2;
   case code_ou: return no+2;
   case code_non: return no+3;
   case code_moins_u: return no+6;
   case code_chaine: return no+7;
   case code_sup_s: return no+1;
   case code_sup: return no+1;
   case code_inf_s: return no+1;
   case code_inf: return no+1;
   case code_egal: return no+1;
   case code_diff: return no+1;
   case code_true: return no+7;
   case code_false: return no+7;
   case code_appel: return no+7;
   case code_map: return no+7;
   case code_unmap: return no+7;
   case code_load: return no+7;
   case code_unload: return no+7;
   default: STOP(("Code inconnue"));
 }

 return 0;
}

// affiche une expression en mettant les parentheses si necessaire
static void affiche_expression2(EXPR *e,int pri)
{
  EXPR *e1;
  int prio,parent=0;

  if(e==NULL)
    return;

  prio=priorite(e->code);
  if((pri>prio)||(parent))
    printf("(");
  
  switch(e->code)
    {
    case code_var :
      printf("%s",e->nom);
      if(e->e1!=NULL)
	  {
		printf("[");
		affiche_expression(e->e1);
		printf("]");
	  }
         break;
    case code_true :
      printf("true");
      break;
    case code_false :
      printf("false");
      break;
    case code_val :
      printf("%d",e->valeur);
      break;
    case code_plus :
      affiche_expression2(e->e1,prio);
      printf("+");
      affiche_expression2(e->e2,prio);
      break;
    case code_moins :
      affiche_expression2(e->e1,prio);
      printf("-");
      affiche_expression2(e->e2,prio);
      break;
    case code_mult :
      affiche_expression2(e->e1,prio);
      printf("*");
      affiche_expression2(e->e2,prio);
      break;
    case code_div :
      affiche_expression2(e->e1,prio);
      printf("/");
      affiche_expression2(e->e2,prio);
      break;
    case code_mod :
      affiche_expression2(e->e1,prio);
      printf("%%");
      affiche_expression2(e->e2,prio);
      break;
    case code_et :
      affiche_expression2(e->e1,prio);
      printf("&&");
      affiche_expression2(e->e2,prio);
      break;
    case code_ou :
      affiche_expression2(e->e1,prio);
      printf("||");
      affiche_expression2(e->e2,prio);
      break;
    case code_inf :
      affiche_expression2(e->e1,prio);
      printf("<=");
      affiche_expression2(e->e2,prio);
      break;
    case code_inf_s :
      affiche_expression2(e->e1,prio);
      printf("<");
      affiche_expression2(e->e2,prio);
      break;
    case code_sup :
      affiche_expression2(e->e1,prio);
      printf(">");
      affiche_expression2(e->e2,prio);
      break;
    case code_sup_s :
      affiche_expression2(e->e1,prio);
      printf(">");
      affiche_expression2(e->e2,prio);
      break;
    case code_egal :
      affiche_expression2(e->e1,prio);
      printf("==");
      affiche_expression2(e->e2,prio);
      break;
    case code_diff :
      affiche_expression2(e->e1,prio);
      printf("!=");
      affiche_expression2(e->e2,prio);
      break;
    case code_non :
      printf("!");
      affiche_expression2(e->e1,prio);
      break;
    case code_moins_u :
      printf("-");
      affiche_expression2(e->e1,prio);
      break;
    case code_chaine :
      printf("%s",e->nom);
      break;
    case code_map :
    case code_unmap :
    case code_load :
    case code_unload :
    case code_appel :
      if(e->code==code_map)
	printf("map(");
      else if(e->code==code_unmap)
	printf("unmap(");
      else if(e->code==code_load)
	printf("load(");
      else if(e->code==code_unload)
	printf("unload(");
      else
	printf("%s(",e->nom);
      e1=e->e1;
      while(e1!=NULL)
	{
	  affiche_expression2(e1,0);
	  
	  if(e1->suivant!=NULL)
	    printf(",");
	  
	  e1=e1->suivant;
	}
      printf(")");
      break;
    default:
      STOP(("Expression inconnue"))
      break;
    }

  if((pri>prio)||(parent))
    printf(")");

}

// affiche une instruction simple sans point virgule
static void affiche_sans_pt_v(INSTR *i)
{
  if(i==NULL)
    return;
    
  switch(i->code)
    {
    case code_continue:
      printf("continue");
      break;
    case code_break:
      printf("break");
      break;
    case code_affectation:
      affiche_expression(i->e1);
      printf("=");
      affiche_expression(i->e2);
      break;
    case code_instr_vide:
      printf(" ");
      break;
    case code_appel_fonction:
      affiche_expression(i->e1);
      break;
    case code_return:
      printf("return ");
      affiche_expression(i->e1);
      break;
    default:
      printf("instruction simple");
      break;
  }
}

/* affichage d'un bloc */
static void affiche_bloc(IB *bloc)
{
 INSTR *i1;
 DECL *d1;

 if(bloc==NULL)
    return;

 tab();
 printf("begin { %s }\n",bloc->nom);
 nb_tab++;
 
 d1=bloc->declare;
 while(d1!=NULL)
 {
  tab();
  affiche_type(d1->type);
  printf(" ");
  affiche_expression(d1->exp);
  printf(";\n");
  d1=d1->suivant;
 }

 i1=(INSTR *)bloc->listeinstr;

 while(i1!=NULL)
 {
  tab();
  switch(i1->code)
  {
    case code_if:
        printf("if(");
        affiche_expression(i1->e1);
        printf(") then\n");
        affiche_bloc(i1->ib1);
        if(i1->ib2!=NULL)
           affiche_bloc(i1->ib2);
        break;
    case code_for:
        printf("for(");
        affiche_sans_pt_v(i1->instr1);
        printf(";");
        affiche_expression(i1->e1);
        printf(";");
        affiche_sans_pt_v(i1->instr2);
        printf(")\n");
        affiche_bloc(i1->ib1);
        break;
    case code_bloc:
        affiche_bloc(i1->ib1);
        break;
    default:
        affiche_sans_pt_v(i1);
        printf(";\n");
        break;
  }

  i1=i1->suivant;
 }

 nb_tab--;tab();
 printf("end { %s }\n",bloc->nom);
}

/* affichage des fonctions */
static void affiche_list_fonc(STF *f)
{
    STFP *param;

	while(f!=NULL)
	{
	if(f->code==fonc_normale)
	{ /* une fonction normale */
        	affiche_type(f->rtype);
        	printf("%s (",f->nom);
        	if(f->par!=NULL)
        	  {
        	    param=f->par;
        	    while(param!=NULL)
        	    {
        	      affiche_type(param->type);
        	      affiche_expression(param->exp);

	              if(param->next!=NULL)
	                printf(",");
	              param=param->next;
	            }
	
	          }
	        printf(")\n");
        }
        else
        { /* la fonction pricipale */
        	printf("Program %s\n",f->nom);
        }
        affiche_bloc(f->ib);
		f=f->suivant;
	}
}

/* cette fonction affiche l'arbre
 par methode recursive */
void affichage_rec(ST *arbre)
{
	CHECK_MSG(arbre!=NULL,("affichage recursif d'un arbre vide"));
	
	nb_tab=0;
	
	printf("Affichage de l'arbre abstrait par recursivite :\n");
	affiche_list_fonc(arbre->listef);
}