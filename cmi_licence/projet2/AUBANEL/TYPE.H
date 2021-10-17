#ifndef TYPE_H
#define TYPE_H

// ce module defini la structure de l'arbre abstrait

//#include"gc.h"

enum ETYPE {Tint,Tvoid,Tbool,Timg,Tstring,TerreurType};

enum VF {false=0,true=1};

typedef struct LS
{
  enum {Ept_virgule,Evirgule,Erien/*,Efin_bloc*/} suivie_de;
  int parent_fermante,parent_ouvrante;
  enum VF retour_ligne;
  enum VF fin_bloc;
  enum VF suivie_else;
  void *contenu;
  int marque;
  struct LS *suivant,*dernier_marque;
  void (*free1)(void *e);
  void (*print)(void *e);
} LS;

typedef struct TYPE
{
  enum ETYPE type;	/* le type */
  int taille;		/* la taille du tableau ou -1 si ce n'en est pas */
  int no_ligne;
} TYPE;

typedef struct EXPR
{
  char *nom;
  int valeur;
  enum {code_var,code_plus,code_moins,code_mult,code_div,code_mod,
	code_et,code_ou,code_non,code_moins_u,code_chaine,code_sup_s,code_sup,
	code_inf_s,code_inf,code_egal,code_diff,
	code_true,code_false,code_appel,code_map,code_unmap,
	code_load,code_unload,code_val} code;
  struct EXPR *e1,*e2;
  int nb_param;
  int no_ligne;
  struct LS *ls;
  int marque;
  TYPE type;
  struct EXPR *suivant; // liste d'expression
} EXPR;

typedef struct DECL
{
  EXPR *exp;
  TYPE type;
  int no_ligne;
  struct DECL *suivant;
} DECL;

typedef struct IB
{
  char *nom,*nom2;
  void *listeinstr;
  DECL *declare;
  int no_ligne;
} IB;

typedef struct INSTR
{
  enum {code_affectation,code_if,code_for,code_appel_fonction,
	code_instr_vide,code_break,code_continue,code_else,
	code_bloc,code_return} code;
  EXPR *e1,*e2,*e3;
  struct IB *ib1,*ib2;
  struct INSTR *instr1,*instr2;
  int no_ligne;
  struct INSTR *suivant;
} INSTR;

/*typedef struct IB
{
	char *nom;
	void *listeinstr;
} IB;*/

typedef struct STFP
{
  TYPE type;
  EXPR *exp;
  int no_ligne;
  struct STFP *next;
} STFP;

/* le type d'une fonction */
typedef struct STF
{
  char *nom; /* nom de la fonction ou 1,2,3,4 si c'est une fonction speciale */
  TYPE rtype; /* type de retour de la fonction */
  STFP *par;
  IB *ib;
  enum {fonc_principale,fonc_normale} code;
  int no_ligne;
  struct STF *suivant;
} STF;

typedef struct ST
{
  STF *listef;
} ST;

#endif
