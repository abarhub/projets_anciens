#include<stdio.h>
#include<string.h>
//#include<malloc.h>
#include<stdlib.h>
#include"type.h"
#include"arbast.h"
#include<stdarg.h>
#include"erreur.h"
#include "debugage.h"
#include"lexer.h"
#include"gc.h"


/* 
ce module fait l'annalyse sementique de l'arbre abstrait
grace a la fonction verification
les fonctions sont exportes par "verif.h"
*/



/* un maillon de la table des symbols */
typedef struct SYMBOL
{
  enum code_symbol {Evar_globale,Evar_locale,Efonction,Eparametre,Efonction_speciale} code;
  DECL *decl_var; // pointe sur la declaration si c'est une variable
  STF *decl_fonct; // pointe sur la declaration si c'est une fonction
  STFP *decl_param; // pointe sur la declaration si c'est un parametre
  char *nom; // nom du symbol
  int no_ligne; // le numero de ligne du symbol
  struct SYMBOL *suivant,*precedant; //  symbol suivant et precedant
} SYMBOL;

/* un element de la pile d'expression */
typedef struct pile_exp
{
  EXPR* exp; /* une expression empilee */
} pile_exp;

static SYMBOL *table_symbol; // la table des symbols
static STF *fonc_main=NULL; /* la fonction principale */

static enum VF verifie_bloc(IB *bloc,int bloc_principal,TYPE typer);
TYPE verifie_variable(EXPR *exp);
static void add_fonc_speciale(char *nom,int type_retour, int nb_param, ...);
static enum VF pile_exp_vide(pile_exp *pile);
static void add_fonc(char *nom,STF *decl);

// retourne une representation en chaine de caracter du code
static char *str_code_symbol(enum code_symbol code)
{
  CHECK_MSG(code>=Evar_globale&&code<=Efonction_speciale,("Code inconnue(%d)",code));
  
  return strdup((code==Evar_globale)?"la variable globale":
		((code==Evar_locale)?"la variable locale":
		 ((code==Efonction)?"la fonction":
		  ((code==Eparametre)?"le parametre":
		   ((code==Efonction_speciale)?"fonction speciale":"code inconnu")))));
}

/*********************************************
fonctions pour la gestion de la table des symbols
**********************************************/

enum VF table_symbol_vide(void)
{
  if(table_symbol==NULL)
    return true;
  else
    return false;
}

static void ajout_fonc_predefini(void)
{
  // ajout de la fonction predefinie load
  add_fonc_speciale("load",Timg,1,Tstring);
  
  // ajout de la fonction predefinie unload
  add_fonc_speciale("unload",Tvoid,1,Timg);
  
  // ajout de la fonction predefinie map
  add_fonc_speciale("map",Tvoid,3,Tint,Tint,Timg);
  
  // ajout de la fonction predefinie unmap
  add_fonc_speciale("unmap",Tvoid,1,Timg);
}

// initialise la table des symbols
static void init_table_symbol(void)
{
  table_symbol=NULL;
  ajout_fonc_predefini();
}

// determine si le nom "nom" est deja present dans la table de symbol
static SYMBOL *recherche(char *nom)
{
  SYMBOL *s;
  
  if(table_symbol_vide())
    return NULL;
  
  s=table_symbol->precedant;
  do
    {
      if(strcmp(s->nom,nom)==0)
	{
	  return s;
	}
      s=s->precedant;
    }
  while(s!=table_symbol->precedant);
  
  return NULL;
}

/* efface les structures creer si c'est une fonction predefinie */
static void efface_fonc_speciale(SYMBOL *symbol)
{
  STF *decl;
  STFP *param1,*param2;
	
  CHECK_MSG(!table_symbol_vide(),("Table des symbols vide"));
  
  if((symbol==NULL)||(symbol->code!=Efonction_speciale))
    return;
	
  decl=symbol->decl_fonct;
  if(decl!=NULL)
    {
      param1=decl->par;
      while(param1!=NULL)
	{
	  param2=param1->next;
	  free(param1);
	  param1=param2;
	}
      free(decl);
    }
}

// effacement du dernier symbol insere
static void efface_dernier(void)
{
  SYMBOL *s;
  
  CHECK_MSG(!table_symbol_vide(),//table_symbol!=NULL,
	    ("Erreur: Tentative d'effacement dans une table de symbol vide"));
  CHECK_MSG((table_symbol->suivant!=NULL)&&(table_symbol->precedant!=NULL),
  	("La liste circulaire pointe vers une reference nulle"));
  
  if(table_symbol->suivant==table_symbol)
    {// la table des symbols n'a qu'un seul element
      efface_fonc_speciale(table_symbol);
      if(table_symbol->nom!=NULL)
	free(table_symbol->nom);
      free(table_symbol);
      table_symbol=NULL;
    }
  else
    {// la table a plusieurs elements
      s=table_symbol->precedant;
      s->precedant->suivant=table_symbol;
      table_symbol->precedant=s->precedant;
      efface_fonc_speciale(s);
      if((s->code==Efonction_speciale)&&(s->nom!=NULL))
	free(s->nom);
      free(s);
    }
}

// efface toute la table des symbols
void efface_table_symbol(void)
{
  CHECK_MSG(!table_symbol_vide()//table_symbol!=NULL
	    ,("Effacement d'une table de symbol vide"));
  
  table_symbol=NULL;
  //return;
  
  /*while(table_symbol!=NULL)
    {
    efface_dernier();
    }*/
}

// ajoute le symbol symbol a la table de symbol
static void add_symbol(SYMBOL *symbol)
{
  SYMBOL *s;
  
  if(table_symbol_vide())//table_symbol==NULL)
    {
      table_symbol=symbol;
      symbol->precedant=symbol;
      symbol->suivant=symbol;
    }
  else
    {
      s=table_symbol->precedant;
      s->suivant=symbol;
      symbol->precedant=s;
      symbol->suivant=table_symbol;
      table_symbol->precedant=symbol;
    }
}

// ajoute une variable globale a la table des symbols
static void add_var_globale(char *nom,DECL *decl)
{
  SYMBOL *s1;
  
  s1=(SYMBOL*)malloc(sizeof(SYMBOL));
  s1->code=Evar_globale;
  s1->decl_var=decl;
  s1->nom=nom;
  s1->no_ligne=decl->no_ligne;
  
  add_symbol(s1);
}

// ajoute une variable locale a la table des symbols
static void add_var_locale(char *nom,DECL *decl)
{
  SYMBOL *s1;
  
  s1=(SYMBOL*)malloc(sizeof(SYMBOL));
  s1->code=Evar_locale;
  s1->decl_var=decl;
  s1->nom=nom;
  s1->no_ligne=decl->no_ligne;
  
  add_symbol(s1);
}

// ajoute un parametre a la table des symbols
static void add_param(char *nom,STFP *decl)
{
  SYMBOL *s1;
  
  s1=(SYMBOL*)malloc(sizeof(SYMBOL));
  s1->code=Eparametre;
  s1->decl_param=decl;
  s1->nom=nom;
  s1->no_ligne=decl->no_ligne;
  
  add_symbol(s1);
}

// ajoute une fonction a la table des symbols
static void add_fonc(char *nom,STF *decl)
{
  SYMBOL *s1;
  
  s1=(SYMBOL*)malloc(sizeof(SYMBOL));
  s1->code=Efonction;
  s1->decl_fonct=decl;
  s1->nom=nom;
  s1->no_ligne=decl->no_ligne;
  
  add_symbol(s1);
}

// ajoute une fonction speciale a la table des symbols
static void add_fonc_speciale(char *nom,int type_retour, int nb_param, ...)
{
  va_list arg;
  SYMBOL *s1;
  STF *decl;
  STFP *param1,*liste_param;
  TYPE t1,t2;
  char *nom1;
  int i,j;
	
  CHECK_MSG(nb_param>=0,
	    ("Erreur: Le nombre de parametres ne doit pas etre negatif"));
  
  /* cree dans decl une signature pour la fonction
     en parcourant les parametres */
	
  va_start(arg,nb_param);
  
  liste_param=NULL;
  t1=creer_type(type_retour,-1);
  nom1=strdup(nom);
  
  for(i=0;i<nb_param;i++)
    {
      j=va_arg(arg,int);
      t2=creer_type(j,-1);
      param1=creer_param(t2,NULL,NULL);
      liste_param=ajoute_param(liste_param,param1);
    }
  
  decl=cree_fonction(t1,nom1,liste_param,NULL,NULL,NULL);
  
  va_end(arg);
  
  /* ajout du symbol */
  
  s1=(SYMBOL*)malloc(sizeof(SYMBOL));
  s1->code=Efonction_speciale;
  s1->decl_fonct=decl;
  s1->nom=nom1;
  s1->no_ligne=decl->no_ligne;
  
  add_symbol(s1);
}

// affiche la table des symbol dans l'ordre
// utilise pour le debugage
static void affiche_table_symbol(void)
{
  SYMBOL *s1;
  char *s;
  
  if(table_symbol_vide())//table_symbol==NULL)
    {
      printf("Table de symbol vide\n");
      return;
    }
  
  printf("Affichage de la table des symbols (les fonctions et var globales):\n");
  
  s1=table_symbol;
  do
    {
      s=str_code_symbol(s1->code);
      printf("%s ",s);
      printf("%s\n",s1->nom);
      free(s);
      s1=s1->suivant;
    }
  while(s1!=table_symbol);
  
  printf("fin de la table des symbols\n");
  
}

/***********************************
fonctions de verification de l'arbre abstrait
************************************/

// affiche un message d'erreur (conflit entre symbol 
// et nom qui est de type code_symbol)
static void erreur_conflit(SYMBOL *symbol,enum code_symbol code,char *nom,int noligne)
{
  char *s,*s2;
  
  CHECK_MSG(symbol!=NULL,("Erreur:appel de conflit avec symbol NULL"));
  
  if(symbol->code==code)
    {
      s=str_code_symbol(code);
      affiche_erreur("%s %s a ete defini plusieurs fois (aux lignes %d et %d)\n",
		     s,nom,noligne,symbol->no_ligne);
      free(s);
    }
  else
    {
      s=str_code_symbol(symbol->code);
      s2=str_code_symbol(code);
      affiche_erreur("Il y a un conflit entre %s %s (a la ligne %d) et %s %s (a la ligne %d)\n",
		     s,symbol->nom,symbol->no_ligne,
		     s2,nom,noligne);
      free(s);free(s2);
    }
}

// verifie que les 2 type sont compatibles
static int type_compatible(TYPE type1,TYPE type2)
{
  if((type1.type==TerreurType)||(type2.type==TerreurType))
    return 1; // pour eviter les erreurs en cascade
  
  if((type1.type==type2.type)&&(type1.type!=TerreurType)&&
     (type1.taille==type2.taille))
    return 1;
  else
    return 0;
}

// retourne le type du symbol
// dans le cas d'une fonction, c'est son type de retour
static TYPE type_symbol(SYMBOL *symbol)
{
  TYPE res;
  
  res.type=TerreurType;
  
  if(symbol==NULL)
    return res;
  
  switch(symbol->code)
    {
    case Evar_globale:
    case Evar_locale:
      /* une variable locale ou globale */
      res=symbol->decl_var->type;
      break;
    case Efonction:
    case Efonction_speciale:
      /* une fonction speciale ou non */
      res=symbol->decl_fonct->rtype;
      break;
    case Eparametre:
      /* un parametre */
      res=symbol->decl_param->type;
      break;
    default:
      STOP(("Erreur:code de symbol inconnue"));
      break;
    }
  
  return res;
}

// cree et retourne une pile de la taille donne
// la pile est vide
static pile_exp *creer_pile_exp(int taille)
{
  pile_exp *pile;
  
  CHECK_MSG(taille>1,("Erreur: la pile d'expression doit avoir"
		      " au minimum une taille de 2"));
  
  pile=(pile_exp*)calloc(sizeof(pile_exp),taille);
  
  CHECK_MSG(pile_exp_vide(pile),("La pile cree n'est pas vide"));
  
  return pile;
}

// retourne 1 ssi la pile est vide
// et 0 sinon
static enum VF pile_exp_vide(pile_exp *pile)
{
  CHECK_MSG(pile!=NULL,("Erreur: La pile d'expression est vide"));
  
  if(pile[0].exp==NULL) return 1;
	else return 0;
}

// ajoute une expression a la pile
static void ajoute(pile_exp *pile,EXPR *exp)
{
  pile_exp *p;
  
  CHECK_MSG(exp!=NULL,("Ajout d'une expression vide"));
  
  p=pile;
  while(p->exp!=NULL)
    {
      p++;
    }
  
  p->exp=exp;
}

// retourne le dernier element ajoute dans la pile
static pile_exp *dernier_pile(pile_exp *pile)
{
  pile_exp *p;
  int i;
  
  CHECK_MSG(!pile_exp_vide(pile),("Erreur:la pile est vide"));
  
  if(pile[1].exp==NULL)
    {
      return pile;
    }
  else
    {
      p=pile;i=0;
      while(p[i+1].exp!=NULL)
	{
	  i++;
	}
    }
  
  return p+i;
}

// enleve le sommet de la pile
static void eleve_dernier(pile_exp *pile)
{
  pile_exp *p;
  
  CHECK_MSG(!pile_exp_vide(pile),("Effacement d'une pile vide"));
  
  p=dernier_pile(pile);
  p->exp=NULL;
}

// efface toute la pile des expressions 
// la pile est passee en parametre
static void efface_pile_exp(pile_exp *exp)
{
  CHECK_MSG(exp!=NULL,("Effacement d'une pile vide"));
  
  free(exp);
}

// verifie que l'expression est correcte et retourne son type
static TYPE verifie_expression(EXPR *exp)
{
  TYPE type_err,type_tmp;
  pile_exp *pile,*p;
  SYMBOL *symbol;
  STFP *param1;
  EXPR *exp1;
  
  CHECK_MSG(exp!=NULL||nb_erreurs>0,("Erreur:expression NULL"));
  
  /* la variable type_err sert affecter
     une erreur de type */
  type_err.type=TerreurType;
  type_err.taille=-1;
  
  if((nb_erreurs>0)&&(exp==NULL))
    return type_err;
  
  pile=creer_pile_exp(100);
  if(exp!=NULL)
    ajoute(pile,exp);
  
  while(!pile_exp_vide(pile))
    {
      p=dernier_pile(pile);
      exp=p->exp;
      
      switch(exp->code)
	{
	case code_var:
	    if((exp->e1!=NULL)&&(exp->e1->marque==0))
	    { // mise dans la pile de la 1ere operande
	      //if(exp->e1!=NULL)
	      ajoute(pile,exp->e1);
	    }
	  else
	    {
		exp->marque=1;
		symbol=recherche(exp->nom);
		if(symbol!=NULL)
		    {
		      type_tmp=type_symbol(symbol);
			if(exp->e1!=NULL)
			  { // il y a un indice de tableau
			    if(type_tmp.taille<0)
				{ // ce n'est pas un tableau
				  affiche_erreur_ligne(exp->no_ligne,
					"Ce n'est pas un tableau\n");
				}
			    else
				{
				  if(exp->e1->type.type!=Tint)
					{ // l'indice doit etre un entier
					  affiche_erreur_ligne(exp->e1->no_ligne,
						"L'indice du tableau ne peut etre qu'entier\n");
					}
				  type_tmp.taille=-1;
				}
			    }
     		        exp->type=type_tmp;
			  
		    }
		else
		  {
		   affiche_erreur_ligne(exp->no_ligne,
				   "La variable %s n'a pas ete declare\n",
				   exp->nom);
	         exp->type=type_err;
	        }
	     }
	  break;
	case code_true: case code_false:
	  exp->marque=1;
	  exp->type.taille=-1;
	  exp->type.type=Tbool;
	  break;
	case code_chaine:
	  exp->marque=1;
	  exp->type.taille=-1;
	  exp->type.type=Tstring;
	  break;
	case code_val:
	  exp->marque=1;
	  exp->type.taille=-1;
	  exp->type.type=Tint;
	  break;
	case code_plus:case code_moins:
	case code_mult:case code_div:
	case code_mod:case code_et:
	case code_ou:case code_sup:
	case code_sup_s:case code_inf:
	case code_inf_s:case code_egal:
	case code_diff: // operation binaire
	  if(exp->e1->marque==0)
	    { // mise dans la pile de la 1ere operande
	      ajoute(pile,exp->e1);
	    }
	  else if(exp->e2->marque==0)
	    {// mise dans la pile de la 2eme operande
	      ajoute(pile,exp->e2);
	    }
	  else
	    {// fin de verification de l'operation
	      exp->marque=1;
	      if(!type_compatible(exp->e1->type,
				  exp->e2->type))
		{
		  affiche_erreur_ligne(exp->no_ligne,"Type incompatible\n");
		  exp->type=type_err;
		}
	      else
		{
		  // mise dans exp->type du type de retour de l'operation
		  if((exp->code==code_sup)||(exp->code==code_sup_s)
		     ||(exp->code==code_inf)||(exp->code==code_inf_s)
							||(exp->code==code_egal)||(exp->code==code_diff))
		    { // le type de l'op est different des operandes
		      exp->type.type=Tbool;
		      exp->type.taille=-1;
		    }
		  else // type de l'op est le meme que les operandes
		    exp->type=exp->e1->type;
						
		  // verification que l'operation peut se faire sur types corrects
		  switch(exp->e1->type.type)
		    {
		    case Tint:
		      if((exp->code==code_et)||
			 (exp->code==code_ou))
			{
			  no_ligne=exp->no_ligne;
			  affiche_erreur_ligne(exp->no_ligne,"Impossible d'appliquer"
					       " cette operation sur le type int\n");
			  no_ligne=-1;
			}
		      break;
		    case Tvoid:
		      no_ligne=exp->no_ligne;
		      affiche_erreur_ligne(exp->no_ligne,
					   "Impossible de faire une operation sur le type void\n");
		      no_ligne=-1;
		      break;
		    case Tbool:
		      if((exp->code!=code_et)&&(exp->code!=code_ou)&&
			 (exp->code!=code_egal)&&(exp->code!=code_diff))
			{
			  no_ligne=exp->no_ligne;
			  affiche_erreur_ligne(exp->no_ligne,
					       "Impossible d'appliquer"
					       " cette operation sur ce"
					       " type bool\n");
			  no_ligne=-1;
			}
		      
		      break;
		    case Tstring:
		      if((exp->code!=code_plus)&&
			 (exp->code!=code_moins)&&(exp->code!=code_egal)
			 &&(exp->code!=code_diff))
			{
			  no_ligne=exp->no_ligne;
			  affiche_erreur_ligne(exp->no_ligne,
					       "Impossible d'appliquer"
					       " cette operation sur le"
					       " type string\n");
			  no_ligne=-1;
			}
		      break;
		    case Timg:
		      if((exp->code!=code_egal)&&(exp->code!=code_diff))
			{
			  no_ligne=exp->no_ligne;
			  affiche_erreur_ligne(exp->no_ligne,"Impossible d'appliquer"
					       " cette operation sur le type img\n");
			  no_ligne=-1;
			}
		      break;
		    case TerreurType:/* ne fait rien pour eviter
					les erreurs en cascade */
		      break;
		    }
		}
	    }
	  break;
	case code_moins_u: case code_non:
	  if(exp->e1->marque==0)
	    {
	      ajoute(pile,exp->e1);
	    }
	  else 
	    {
	      exp->marque=1;
	      exp->type=exp->e1->type;
	      if((exp->code==code_non)&&(exp->type.type!=Tbool))
		{
		  affiche_erreur_ligne(exp->no_ligne,
				       "Impossible de faire la negation de ce type\n");
		}
	      if((exp->code==code_moins_u)&&( 
					     (exp->type.type==Tbool)||
					     (exp->type.type==Tvoid)||
					     (exp->type.type==Timg) ))
		{
		  affiche_erreur_ligne(exp->no_ligne,
				       "Impossible de faire l'oppose de ce type\n");
		}
	    }
	  break;
	case code_appel: case code_map:
	case code_unmap: case code_load:
	case code_unload:
	  exp1=exp->e1;
	  while((exp1!=NULL)&&(exp1->marque==1))
	    exp1=exp1->suivant;
	  if(exp1==NULL)
	    {
	      switch(exp->code)
		{
		case code_load:
		  symbol=recherche("load");
		  break;
		case code_unload:
		  symbol=recherche("unload");
		  break;
		case code_map:
		  symbol=recherche("map");
		  break;
		case code_unmap:
		  symbol=recherche("unmap");
		  break;
		default:
		  symbol=recherche(exp->nom);
		  break;
		}
	      if(symbol==NULL)
		{
		  affiche_erreur_ligne(exp->no_ligne,"fonction non definie\n");
		  exp->type=type_err;
		}
	      else
		{
		  if((symbol->code!=Efonction)&&
		     (symbol->code!=Efonction_speciale))
		    {
		      affiche_erreur_ligne(exp->no_ligne,
					   "ce n'est pas une fonction declaree\n");
		      exp->type=type_err;
		    }
		  else
		    {
		      param1=symbol->decl_fonct->par;
		      exp1=exp->e1;
		      while((param1!=NULL)&&(exp1!=NULL))
			{
			  param1=param1->next;
			  exp1=exp1->suivant;
			}
		      if((param1!=NULL)||(exp1!=NULL))
			{
			  affiche_erreur_ligne(exp->no_ligne,
					       "il n'y a pas le bon nombre de parametres\n");
			}
		      exp->type=symbol->decl_fonct->rtype;
		    }
		}
	      exp->marque=1;
	    }
	  else
	    {
	      ajoute(pile,exp1);
	    }
	  break;
	default: 
	  STOP(("expression inconnue(%d)",exp->code));
	  break;
	}
      if(exp->marque==1)
	{
	  eleve_dernier(pile);
	}
    }
  
  efface_pile_exp(pile);
  
  return exp->type;
}

/* verifie une instruction
typer est le type de la fonction qui contient l'instruction
la fonction retourne true ssi l'instruction contient un return
et false sinon
*/
static enum VF verifie_instr(INSTR *instr,TYPE typer)
{
  TYPE type1,type2;
  enum VF return_trouve=false,tmp1;
  
  if(instr==NULL)
    return return_trouve;
  
  switch(instr->code)
    {
    case code_bloc: /* un bloc */
      tmp1=verifie_bloc(instr->ib1,0,typer);
      return_trouve=return_trouve||tmp1;
      break;
      
    case code_affectation: /* l'instruction d'affectation */
      type1=verifie_expression(instr->e1);
      //printf("************\n");
      type2=verifie_expression(instr->e2);
      //printf("###############\n");
      if(!type_compatible(type1,type2))
	{// type incompatible
	  affiche_erreur_ligne(instr->no_ligne,
	  	"type incompatible dans l'affectation\n");
	}
	else if(instr->e1->code!=code_var)
	{ /* le membre de gauche n'est pas
	 une variable (c.a.d. une left-value) */
		affiche_erreur_ligne(instr->no_ligne,
			"Il faut une variable a gauche d'une affectation\n");
	}
      break;
      
    case code_if: /* l'instruction if */
      type1=verifie_expression(instr->e1);
      if((type1.type!=Tbool)||(type1.taille!=-1))
	{
	  affiche_erreur_ligne(instr->no_ligne,
	  	"La condition n'est pas un booleen\n");
	}
      /* verification du 1er bloc */
	  tmp1=verifie_bloc(instr->ib1,0,typer);
	  return_trouve=return_trouve||tmp1;
	  if(instr->ib2==NULL)
	  { /* il n'y a pas de else */
	  	affiche_erreur_ligne(instr->no_ligne,"Le bloc else est obligatoire\n");
	  }
	  else
	  {  /* verification du bloc else */
	  	tmp1=verifie_bloc(instr->ib2,0,typer);
	  	return_trouve=return_trouve||tmp1;
	  }
	  break;
	  
    case code_for: /* l'instruction for */
      /* verification de la 1ere instruction */
      tmp1=verifie_instr(instr->instr1,typer);
      return_trouve=return_trouve||tmp1;
      
      /* verification de l'expression */
      type2=verifie_expression(instr->e1);
      if((type1.type!=Tbool)||(type1.taille!=-1))
	{
	  affiche_erreur_ligne(instr->no_ligne,
	  	"La condition n'est pas un booleen\n");
	}
	  /* verification de la 2eme instruction */
      tmp1=verifie_instr(instr->instr2,typer);
      return_trouve=return_trouve||tmp1;
      
      /* verification du bloc */
      tmp1=verifie_bloc(instr->ib1,0,typer);
      return_trouve=return_trouve||tmp1;
      break;
    case code_appel_fonction: /* l'instruction appel de fonction */
      type1=verifie_expression(instr->e1);
      if((type1.type!=Tvoid)&&(type1.type!=TerreurType))
      { /* l'instruction appel de fonction ne peut etre
      que de type void */
      	affiche_erreur_ligne(instr->no_ligne,"cette fonction n'est pas de type void\n");
      }
      break;
      
    case code_return: /* l'instruction return */
    	if(instr->e1!=NULL)
    	{ /* retourne une expression */
    		type1=verifie_expression(instr->e1);
    		if(type1.type==Tvoid)
    		{ /* l'expression ne peut pas etre de type void */
    			affiche_erreur_ligne(instr->no_ligne,
    			"Type void interdit dans un return\n");
    		}
    	}
    	else
    	{ /* ne retourn rien */
    		type1.type=Tvoid;
    		type1.taille=-1;
    	}
    	
    	if(!type_compatible(typer,type1))
    	{ /* le return ne retourne pas le meme type
    	 que le type de retour de la fonction englobante */
    		affiche_erreur_ligne(instr->no_ligne,"return retourne un mauvais type\n");
    	}
    	return_trouve=true;
    	break;
    	
    case code_instr_vide:
    case code_continue:
    case code_break:
    	break;
    	
    default: /* instruction inconnue */
      //CHECK_MSG(0,("Instruction inconnue %d",instr->code));
      STOP(("Instruction inconnue %d",instr->code));
      break;
    }
  
  return return_trouve;
}

// verifie les declarations du bloc
// si c'est le bloc principal(=1), n'ajoute pas les declarations
// retourne true ssi il a un return (correcte ou non)
static enum VF verifie_bloc(IB *bloc,int bloc_principal,TYPE typer)
{
  SYMBOL *symbol;
  int nb_local=0;
  DECL *decl;
  INSTR *instr;
  enum VF return_trouve=false,tmp1;
  
  if(bloc==NULL)
    return return_trouve;
  
  if(bloc_principal==0)
  {
  	// ajoute a la table des symbols les declarations locales
  decl=bloc->declare;
  while(decl!=NULL)
    {
      if(decl->exp->e1!=NULL)
	  {
	    affiche_erreur_ligne(decl->exp->e1->no_ligne,
		  "On ne declare pas un tableau de cette facon\n");
	  }
	
	symbol=recherche(decl->exp->nom);
      if(symbol!=NULL)
	{
	  erreur_conflit(symbol,Evar_locale,symbol->nom,decl->no_ligne);
	}
      else
	{
	  add_var_locale(decl->exp->nom,decl);
	  nb_local++;
	}
      
      decl=decl->suivant;
    }
  }
  
  // parcourt des instrcutions
  instr=(INSTR*)bloc->listeinstr;
  while(instr!=NULL)
    {
      tmp1=verifie_instr(instr,typer);
      return_trouve=return_trouve||tmp1;
      
      instr=instr->suivant;
    }
  
  if(bloc_principal==0)
  {
  	// enleve de la table de symbol les declarations locales
  while(nb_local>0)
    {
      nb_local--;
      efface_dernier();
    }
  }
  
  return return_trouve;//type_retour;
}

// annalyse les fonction et la fonction principale
// qui doit etre dans la var globale fonc_main
static void annalyse_context(STF *liste_fonctions)
{
  DECL *d1;
  STF *f1;
  int nb_param;
  STFP *p1;
  SYMBOL *symbol;
  
  // initialisation de la table des symbols
  init_table_symbol();
  
  printf("annalyse contextuelle\n");
  
  // ajout de la fonction principale
  add_fonc(fonc_main->nom,fonc_main);
  
  // ajout des variables globales
  if(fonc_main->ib!=NULL)
    {
      d1=fonc_main->ib->declare;
      while(d1!=NULL)
	{
	    if(d1->exp->e1!=NULL)
	    {
	      affiche_erreur_ligne(d1->exp->e1->no_ligne,
		    "On ne declare pas un tableau ainsi\n");
	    }
	  add_var_globale(d1->exp->nom,d1);
	  
	  d1=d1->suivant;
	}
    }
  
  f1=liste_fonctions;
  while(f1!=NULL)
  {
  	if(f1!=fonc_main)
  	{
  	symbol=recherche(f1->nom);
      if(symbol!=NULL)
	{// il y a un conflit dans le nom de la fonction
	  erreur_conflit(symbol,Efonction,f1->nom,f1->no_ligne);
	}
	else
	{     
      // ajout du nom de la fonction
      add_fonc(f1->nom,f1);
    }
    }
    f1=f1->suivant;
  }
  
  // verifie chaque fonction
  f1=liste_fonctions;
  while(f1!=NULL)
    {
      if(f1==fonc_main)
	{// saute la fonction main
	  if(!verifie_bloc(f1->ib,1,f1->rtype))
	  {
	  	affiche_erreur_ligne(f1->no_ligne,
	  		"La fonction %s n'a pas de return\n",f1->nom);
	  }
	  
	  f1=f1->suivant;
	  continue;
	}
      
      symbol=recherche(f1->nom);
      if(symbol==NULL)
	{// il y a un conflit dans le nom de la fonction
	  f1=f1->suivant;
	  continue;
	}
	
      nb_param=0;
      
      // ajout des parametres
      p1=f1->par;
      while(p1!=NULL)
	{
	    if(p1->exp->e1!=NULL)
	    {
		  affiche_erreur_ligne(p1->exp->e1->no_ligne,
			"Un tableau ne se declare pas comme cela\n");
	    }
	  symbol=recherche(p1->exp->nom);
	  if(symbol!=NULL)
	    {
	      erreur_conflit(symbol,Eparametre,p1->exp->nom,p1->no_ligne);
	    }
	  else
	    {
	      add_param(p1->exp->nom,p1);
	      nb_param++;
	    }
		p1=p1->next;
	}
      
      // ajout des variables locales
      if(!verifie_bloc(f1->ib,0,f1->rtype))
      {
	  	affiche_erreur_ligne(f1->no_ligne,"La fonction %s n'a pas de return\n",f1->nom);
	  }
	
      // effacement des symbols locaux
      while(nb_param>0)
	{
	  nb_param--;
	  efface_dernier();
	}
      
      f1=f1->suivant;
    }
  
  affiche_table_symbol();

  //efface_table_symbol();
  
  printf("fin d'annalyse contextuelle\n");
  
  //return 1;
}

/* cette fonction verifie si l'arbre syntaxique
 "program" est correcte */
int verification(ST *program)
{
  STF *f1;
  
  CHECK_MSG(program!=NULL,("Erreur: Programme vide"));

  gc_ajoute_global((void**)&table_symbol);
  gc_ajoute_global((void**)&fonc_main);
  
  /*table_symbol=malloc(sizeof(SYMBOL));
  table_symbol->nom=strdup("salut;");
  gc();
  
  free(table_symbol);*/
  
  //return 1;
  
  no_ligne=-1;
  
  // recherche s'il y a une seule fonction principale
  fonc_main=NULL;
  f1=program->listef;
  while(f1!=NULL)
    {
      if(f1->code==fonc_principale)
	{
	  if(fonc_main!=NULL)
	    {
	      affiche_erreur("Il y a plusieurs fonctions"
			     " principales\n");
	    }
	  else
	    {
	      fonc_main=f1;
	    }
	}
      
      f1=f1->suivant;
    }
  
  if(fonc_main==NULL)
    {
      affiche_erreur("Il n'y a pas de fonctions principales\n");
    }
  else
    {
      annalyse_context(program->listef);
    }
  
  
  
  return 1;
}
