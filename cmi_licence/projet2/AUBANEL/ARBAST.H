#ifndef ARBRE_ABSTRAIT
#define ARBRE_ABSTRAIT

#include"type.h"

extern LS *liste;
extern LS *liste_dernier;
extern LS *dernier_marque;
extern ST *programme;

void affiche(ST *);
void affiche_LS(void);
void efface_ls(void);
STF *ajoute_fonct(STF *liste,STF *fonc);
STF *cree_fonction_principale(char *nom,IB *bloc,LS *ls);
STF *cree_fonction(TYPE rtype,char *nom,STFP *param,IB *bloc,LS*ls,LS *ls2);
STFP *creer_param(TYPE type,EXPR *exp,LS*ls);
STFP *ajoute_param(STFP *liste,STFP *param);
IB *creer_bloc(char *nom,DECL *decl,INSTR *instr,char *nom2,LS*ls);
DECL *complete_decl(TYPE type,DECL *decl);
DECL *ajoute_declaration(DECL *liste,DECL *elt);
DECL *creer_declaration2(EXPR* exp,LS*ls);
DECL *ajoute_declaration2(DECL *liste,EXPR *exp,LS*ls);
EXPR *creer_variable(char *nom,int no);
TYPE creer_type(enum ETYPE type,int taille);
INSTR *ajoute_instr(INSTR *liste,INSTR *instr);
INSTR *creer_instr_bloc(IB *bloc);
INSTR *creer_instr_appel(EXPR *fonc);
INSTR *creer_instr_continue(void);
INSTR *creer_instr_break(void);
INSTR *creer_instr_vide(void);
INSTR *creer_instr_return(EXPR *e,LS*ls);
INSTR *creer_if(EXPR *exp,IB *b1,IB *b2,LS*ls);
INSTR *creer_for(INSTR *instr,EXPR *exp,INSTR *instr2,IB *bloc,LS*ls);
INSTR *creer_affectation(EXPR *var,EXPR *exp,LS*ls);
EXPR *creer_fonction(char *nom,EXPR *exp,LS*ls,LS *ls2);
EXPR *ajoute_expression(EXPR *liste,EXPR *exp,LS*ls);
EXPR *creer_exp_binaire(int code,EXPR *exp1,EXPR *exp2,LS*ls);
EXPR *creer_exp_unaire(int code, EXPR *exp1,LS*ls);
EXPR *creer_chaine(char *str);
EXPR *creer_nombre(int nb);
EXPR *creer_bool(int code);

void marque();
void complete_dernier_marque(void *e,void (*free1)(void*e), 
			     void (*print)(void*e));
LS *add_vide(void);
void complete_LS(LS *ls,void *e,void (*free1)(void*e), 
		 void (*print)(void*e));


#endif
