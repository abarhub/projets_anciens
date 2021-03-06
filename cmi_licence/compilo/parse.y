%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "type.h"
#include "arbast.h"
#include "verif.h"
#include "lexer.h"
#include "erreur.h"
#include "debugage.h"
#include "gcbase.h"

// ce module annalyse la syntaxe du fichier

/* il faut enlever les "etrange" */

int yyerror(char*);
int yyparse(void);

int dans_type_retour=0;

%}

%union {
char *string;
int val;
STF *fonc;
STFP *param;
IB *ib;
INSTR *instr;
EXPR *expr;
TYPE type;
DECL *decl;
LS *ls;
};

%token TK_PROGRAM TK_BEGIN TK_END TK_FOR TK_IF TK_THEN TK_ELSE
%token TK_BREAK TK_CONTINUE
%token TK_VOID TK_BOOL TK_INT TK_IMG TK_STRING 
%token TK_LOAD TK_UNLOAD TK_MAP TK_UNMAP
%token TK_TRUE TK_FALSE TK_RETURN

%token <string> ID CHAINE
%token <val> NOMBRE
%token TEST_ET TEST_OU TEST_NOT
%token TEST_INF_S TEST_SUP_S TEST_INF TEST_SUP TEST_EGAL TEST_DIFFERENT   

%left TEST_INF_S TEST_SUP_S TEST_INF TEST_SUP TEST_EGAL TEST_DIFFERENT
%left TEST_ET TEST_OU

%right UNAIRE_TEST

%left '+' '-'

%left '*' '/' '%'

%right UNAIRE

%start fichier

%type <fonc> liste_declaration_fonctions declaration_fonctions
%type <param> parametre liste_parametres parametres_ou_vide
%type <type> type type_retour
%type <val> crochet
%type <string> nom_fonction id_ou_mot_cle fin_bloc nom_bloc
%type <decl> liste_variable declaration liste_declaration
%type <instr> liste_instruction instruction instr_sans_pt_virgule
%type <instr> inst_if inst_for affectation 
%type <ib> inst_else bloc
%type <expr> expression fonction liste_expression liste_expression_ou_vide
%type <expr> variable expression_ou_vide crochet2

%%

/* un fichier complet */
fichier : /* fichier vide */ { programme=NULL; affiche_erreur("Le fichier est vide\n");YYABORT; }
		| liste_declaration_fonctions	
				{ 
					programme=(ST*)gc_malloc(sizeof(ST));
					programme->listef=$1;
				}
| error			{affiche_erreur("erreur dans le prog\n");YYABORT;}
        ;

/* une liste de declaration de fonctions suivie 
   d'une declaration du programme principal */
liste_declaration_fonctions : liste_declaration_fonctions declaration_fonctions
				{
					$$=ajoute_fonct($1,$2);
				}
                            | declaration_fonctions	{$$=$1;}
                            ;

/* une declaration de fonction */
declaration_fonctions : 
				{$<ls>$=add_vide();} type_retour nom_fonction/*ID*/ 
				'(' parametres_ou_vide ')' { $<ls>$=liste_dernier; }  bloc
				{
					char *s;
					
					switch($<val>3)
					{
						case 1: s=gc_strdup("load");break;
						case 2: s=gc_strdup("unload");break;
						case 3: s=gc_strdup("map");break;
						case 4: s=gc_strdup("unmap");break;
						default: s=$3;break;
					}
					$$=cree_fonction($2,s,$5,$8,$<ls>1,$<ls>7);
					yyerrok;
				}
				| {$<ls>$=add_vide();} TK_PROGRAM ID bloc
				{
					
					$$=cree_fonction_principale($3,$4,
								    $<ls>1);
				}
                      ;

/* une liste de parametres de fonction ou le mot cle "void" ou vide */
parametres_ou_vide : liste_parametres	{$$=$1;}
                   | TK_VOID 			{$$=NULL;}
                   |					{$$=NULL;}
                   ;

/* une liste de parametres de fonction */
liste_parametres : liste_parametres ',' parametre
					{
						$$=ajoute_param($1,$3);
					}
                 | liste_parametres ',' error suite
                    {
                     affiche_erreur("Parametre invalide\n");
                     $$=$1;//yyerrok;
                    }
                 | parametre
                 			{ $$=$1; }
                 | error suite
                    {
                      affiche_erreur("Parametre invalide\n");
                      $$=NULL;//yyerrok;
                    }
                 ;

suite: ',' | ')'
    ;

/* un parametre de fonction */
/* la 1ere action est mise apres type pour eviter les conflits */
parametre : type {$<ls>$=add_vide();} variable
					{
						$$=creer_param($1,$3,$<ls>2);
					}
//          | error {
//                affiche_erreur("Parametre invalide\n");$$=NULL;/*yyerrok;*/}
          ;

/* mots valides pour nommer un bloc */
id_ou_mot_cle:
            ID          { $$=$1; }
            | TK_IF     { $$=(char*)gc_strdup("if"); }
            | TK_ELSE   { $$=(char*)gc_strdup("else"); }
            | TK_FOR    { $$=(char*)gc_strdup("for"); }
            | error //'}'
                {
                    affiche_erreur("Il faut un mot\n");
                    $$=NULL;//yyerrok;
                }
            ;

/* un bloc d'instructions */
bloc : TK_BEGIN {$<ls>$=add_vide();} nom_bloc
       liste_declaration
       liste_instruction
       fin_bloc
       			{
       				IB *ib;
       				
       				ib=creer_bloc($3,$4,$5,$6,$<ls>2);
				if((ib->nom!=NULL)&&(ib->nom2!=NULL)
					&&(strcmp(ib->nom,ib->nom2)!=0))
				{
                    	affiche_erreur("un bloc doit avoir le"
					" meme nom au debut et a la fin (%s!=%s)\n",
					ib->nom,ib->nom2);
				}
                    $$=ib;//yyerrok;
       			}
//     | TK_BEGIN '{' ID '}' error TK_END '{' ID '}' 
//          { affiche_erreur("erreur dans le bloc\n"); }
		/*| error '}' 
			{ affiche_erreur("Le bloc se termine mal\n");yyclearin;yyerrok; }*/
     ;

fin_bloc: TK_END nom_bloc
				{ $$=$2; }
			| error  nom_bloc
				{ affiche_erreur("Il manque un end\n");$$=$2;/*yyerrok;*/ }
			| error '}'
				{ $$=NULL; affiche_erreur("Erreur dans le bloc\n");  }
		;

nom_bloc:
		'{' id_ou_mot_cle '}' 
			{ $$=$2; }
		| id_ou_mot_cle '}' 
			{ $$=$1;affiche_erreur("Il manque {\n"); }
		| '{' id_ou_mot_cle
			{ $$=$2;affiche_erreur("Il manque }\n"); }
		| error 
			{ $$=NULL;affiche_erreur("Il faut un nom de bloc\n");yyerrok; }
		;

liste_declaration : liste_declaration declaration
			{
				$$=ajoute_declaration($1,$2);
			}
                  |		{$$=NULL;}
                  ;

/* une declaration de variables avec un type */
declaration : type liste_variable ';'	
		{
			$$=complete_decl($1,$2);
		}
            ;

/* un type de retour de fonction */
/* si la variable globale dans_type_retour est a 1
la "regle" type n'affiche pas d'erreur si c'est un void */
type_retour : { dans_type_retour=1; }
              type
              { dans_type_retour=0;$$=$<type>2; }
            ;

/* un type sauf void si la var globale dans_type_retourest a 0 */
type : TK_INT crochet		{ $$=creer_type(Tint,$2); }
     | TK_STRING crochet	{ $$=creer_type(Tstring,$2); }
     | TK_BOOL crochet	{ $$=creer_type(Tbool,$2); }
     | TK_IMG crochet	{ $$=creer_type(Timg,$2); }
     | TK_VOID crochet  {
			  if(dans_type_retour==0)
			  {
                affiche_erreur("type void interdit sauf"
					" pour le type de retour\n");
				$$=creer_type(TerreurType,-1);
			  }
			else
			{
				$$=creer_type(Tvoid,$2);
				if($2>-1) 
                			affiche_erreur("le type void ne"
					" peut etre un tableau\n");
			}
			}
     ;

crochet: '[' NOMBRE ']' { $$=$2; /*yyerrok;*/}
	| NOMBRE ']'	{
                affiche_erreur("Il manque un crochet ouvrant\n");$$=$1;
				}
	| '[' NOMBRE	{
                affiche_erreur("Il manque un crochet fermant\n");$$=$2;
				}
    | '[' error ']' {
                        affiche_erreur("Il faut un nombre entre les crochets\n");
                        $$=-1;yyerrok;
                    }
	| /* vide c.a.d sans crochet */	{ $$=-1; }
	;

/* un acces a un element d'un tableau */
crochet2: '[' {$<ls>$=liste_dernier;} expression ']' {$<ls>$=liste_dernier;}
		{
		    $$=$3;
		    $<ls>2->crochet_ouvrant++;
		    $<ls>5->crochet_fermant++;
		    /*yyerrok;*/
		}
    | '[' error ']' {
                        affiche_erreur("Il faut un nombre entre les crochets\n");
                        $$=NULL;yyerrok;
                    }
	| /* vide c.a.d sans crochet */	{ $$=NULL; }
	;

liste_variable : liste_variable ',' {$<ls>$=add_vide();} variable
			{
               			$$=ajoute_declaration2($1,$4,$<ls>3);
               		}
               | {$<ls>$=add_vide();} variable	
               		{
               			$$=creer_declaration2($2,$<ls>1);
               		}
               ;

variable : ID {$<ls>$=add_vide();} crochet2 
		{ 
			$$=creer_variable($1,$3,$<ls>2);
		}
         ;

/* une suite d'instructions ou rien */
liste_instruction : liste_instruction instruction
					{
						$$=ajoute_instr($1,$2);
					}
                  |			{$$=NULL;}
                  ;

/* une instruction complete */
instruction : inst_if   /* un if */
				{$$=$1;yyerrok;}
            | inst_for  /* un for */
				{$$=$1;yyerrok;}
            | instr_sans_pt_virgule pt_virgule_obligatoire 
            /*';' /* une instruction simple sans point virgule */
				{$$=$1;yyerrok;}
            | bloc      /* un bloc d'instructions */
				{
                  $$=creer_instr_bloc($1);
                }
            /*| TK_RETURN expression { $$=creer_return($2); }
            | TK_RETURN { $$=creer_return(NULL); }*/
            | error suite_pt_virgule //';'
                {
                    affiche_erreur("Instruction invalide\n");
                    $$=NULL;//yyerrok;
                }
            /*| error
            	{
            		affiche_erreur("");
            	}*/
            ;

pt_virgule_obligatoire:
			';'
			//| { affiche_erreur("Il faut un point virgule a la fin d'une instruction\n"); }
			;

suite_pt_virgule: ';' //| TK_END  | '{'
				{ /*yyerrok;*/ }
			;

instr_sans_pt_virgule : affectation
				{$$=$1;}
                      | fonction
                {
                    			$$=creer_instr_appel($1);
                }
                      | TK_CONTINUE
				{ /* le mot cle continue */
					$$=creer_instr_continue();
				}
                      | TK_BREAK
				{ /* le mot cle break */
					$$=creer_instr_break();
				}
                      | TK_RETURN {$<ls>$=add_vide();} expression_ou_vide
                       {
			 $$=creer_instr_return($3,$<ls>2);
                       }
                      | /* instruction vide */
				{
					$$=creer_instr_vide();
				}
                      ;

inst_if : TK_IF {$<ls>$=add_vide();} '(' expression ')' {$<ls>$=liste_dernier;} TK_THEN 
          bloc
          inst_else
          	{
          		$$=creer_if($4,$8,$9,$<ls>2);
			$<ls>6->parent_fermante++;
          		if($4==NULL)
			{
                		affiche_erreur("Erreur dans le test du if\n");
			}
			if($$->ib1==NULL)
                	{
                 	 affiche_erreur("Erreur dans le bloc du if\n");
                	}
                	else
                	{
                	  if(strcmp($$->ib1->nom,"if")!=0)
                	  {
                	    affiche_erreur("Un bloc if doit avoir un nom if\n");
                	  }
                	}
          	}
        ;

inst_else : TK_ELSE {$<ls>$=liste_dernier;}
            bloc
            	{
		  IB *i1;
            		
		  i1=$3;
		  if(i1==NULL)
		    {
		      affiche_erreur("Le bloc du else ne doit pas etre vide\n");
		    }
		  else
		    {
		      $<ls>2->suivie_else=true;
		      if(strcmp(i1->nom,"else")!=0)
			{
			      affiche_erreur("Le bloc else doit"
					     " avoir pour nom else\n");
			}
		    }
		  $$=i1;
            	}
          | 	{$$=NULL;}
          ;

inst_for : TK_FOR {$<ls>$=add_vide();} '(' instr_sans_pt_virgule ';' 
           {$<ls>$=liste_dernier;} expression {$<ls>$=liste_dernier;}
           ';' instr_sans_pt_virgule ')' {$<ls>$=liste_dernier;}
           bloc
            {
           		IB *ib1;
          		
          		$$=creer_for($4,$7,$10,$13,$<ls>2);
			ib1=$$->ib1;
			$<ls>6->retour_ligne=false;
			$<ls>6->suivie_de=Ept_virgule;
			$<ls>8->suivie_de=Ept_virgule;
			$<ls>12->parent_fermante++;
			$<ls>12->suivie_de=Erien;
			if(ib1!=NULL)
			{
				if(strcmp(ib1->nom,"for")!=0)
				{
                    		affiche_erreur("Le bloc du for doit"
						" avoir pour nom \"for\"\n");
				}
			}
			else
			{
                		affiche_erreur("For sans bloc d'instruction\n");
			}
           	}
         ;

affectation : variable '=' {$<ls>$=add_vide();} expression	
		{
			$$=creer_affectation($1,$4,$<ls>3);   
		}
            ;

fonction : nom_fonction  '(' {$<ls>$=add_vide();} liste_expression_ou_vide
			  ')' { $<ls>$=liste_dernier; }
		{
			$$=creer_fonction($1,$4,$<ls>3,$<ls>6);
		}
	| nom_fonction '(' error ')'
        { /* erreur dans les parametres de l'appel de fonction */
            affiche_erreur("erreur dans les parametres\n");
            if($<val>1>4) free($1);
            $$=NULL;yyerrok;
        }
         ;

nom_fonction : TK_LOAD		{$<val>$=1;/* convertion en entier */}
             | TK_UNLOAD	{$<val>$=2;/* convertion en entier */}
             | TK_MAP		{$<val>$=3;/* convertion en entier */}
             | TK_UNMAP		{$<val>$=4;}
             | ID 			{$$=$1;}
             ;

/* les parametres effectifs d'une fonction ou aucun parametre */
liste_expression_ou_vide: liste_expression 
			{
				$$=$1;
			}
		|
			{
				$$=NULL;
			}
		;

/* une liste d'expression separee par une virgule */        
liste_expression : liste_expression ',' {$<ls>$=liste_dernier;} expression
					{ /* des expressions separees par des virgules */
						$$=ajoute_expression($1,$4,$<ls>3);
					}
                 | expression
                 			{
                 				$$=$1;
                 			}
                 ;

expression_ou_vide: /* vide */ { $$=NULL; }
		| expression			{ $$=$1; }
		;

/* une expression arithmetique ou conditionnelle ou de chaine */
expression : expression '*' {$<ls>$=add_vide();} expression
			{ /* produit */
				$$=creer_exp_binaire(code_mult,$1,$4,$<ls>3);
			}
           | expression '/' {$<ls>$=add_vide();} expression
			{ /* division */
				$$=creer_exp_binaire(code_div,$1,$4,$<ls>3);
			}
           | expression '+' {$<ls>$=add_vide();} expression
			{ /* addition */
				$$=creer_exp_binaire(code_plus,$1,$4,$<ls>3);
			}
           | expression '-' {$<ls>$=add_vide();} expression
			{ /* soustraction */
				$$=creer_exp_binaire(code_moins,$1,$4,$<ls>3);
			}
           | expression '%' {$<ls>$=add_vide();} expression
			{ /* modulo */
				$$=creer_exp_binaire(code_mod,$1,$4,$<ls>3);
			}
           | '-' {$<ls>$=add_vide();} expression %prec UNAIRE
			{ /* negation numerique */
				$$=creer_exp_unaire(code_moins_u,$3,$<ls>2);
			}
           | '(' {$<ls>$=liste_dernier;} expression ')' { $<ls>$=liste_dernier; }
			{ /* parenteses */
				$$=$3;
				$<ls>2->parent_ouvrante++;
				$<ls>5->parent_fermante++;
			}
           | expression TEST_INF_S {$<ls>$=add_vide();} expression
			{ /* test inferieur strictement */
				$$=creer_exp_binaire(code_inf_s,$1,$4,$<ls>3);
			}
           | expression TEST_SUP_S {$<ls>$=add_vide();} expression
			{ /* test superieur strictement */
				$$=creer_exp_binaire(code_sup_s,$1,$4,$<ls>3);
			}
           | expression TEST_INF {$<ls>$=add_vide();} expression
			{ /* test inferieur */
				$$=creer_exp_binaire(code_inf,$1,$4,$<ls>3);
			}
           | expression TEST_SUP {$<ls>$=add_vide();} expression
			{ /* test supperieur */
				$$=creer_exp_binaire(code_sup,$1,$4,$<ls>3);
			}
           | expression TEST_EGAL {$<ls>$=add_vide();} expression
			{ /* test egalite */
				$$=creer_exp_binaire(code_egal,$1,$4,$<ls>3);
			}
           | expression TEST_DIFFERENT {$<ls>$=add_vide();} expression
			{ /* test different */
				$$=creer_exp_binaire(code_diff,$1,$4,$<ls>3);
			}
           | expression TEST_ET {$<ls>$=add_vide();} expression
			{ /* ou booleen */
				$$=creer_exp_binaire(code_et,$1,$4,$<ls>3);
			}
	   | expression TEST_OU {$<ls>$=add_vide();} expression
			{ /* ou booleen */
				$$=creer_exp_binaire(code_ou,$1,$4,$<ls>3);
			}
	   | '!' {$<ls>$=add_vide();} expression %prec UNAIRE_TEST
			{ /* negation booleenne */
				$$=creer_exp_unaire(code_non,$3,$<ls>2);
			}
           | TK_TRUE
			{ /* le mot cle true */
				$$=creer_bool(code_true);
            }
           | TK_FALSE
			{ /* le mot cle false */
				$$=creer_bool(code_false);
				
			}
           | fonction
			{ /* une fonction */
				$$=$1;
			}
           | NOMBRE 	
			{ /* un nombre */
				$$=creer_nombre($1);
			}
       	   | variable
			{/* une variable */
				$$=$1;
			}
           | CHAINE	
			{ /* une chaine */
				$$=creer_chaine($1);
			}
           ;

%%

int yyerror(char *s)
{/* affiche une erreur pendant l'annalyse */
  printf("Erreur:%s\n",s);
  return 1;
}

/*
annalyse le fichier nom_fichier
retourne 0 si le fichier est correcte
         -1 si le fichier ne peut pas etre ouvert
         -2 si le fichier est incorrecte (erreur de grammaire non recupere)
	 n>0 s'il y a eut n erreurs
*/
int annalyse(char *nom_fichier)
{
  if(nom_fichier!=NULL)
    {// ouvre le fichier a annalyse par lex
      yyin=fopen(nom_fichier,"r");
      if(yyin==NULL)
	return -1;
    }
  
  nb_erreurs=0;dans_type_retour=0;
  no_ligne=1;liste=NULL;programme=NULL;
  if(yyparse()==1)
    {// erreur irrecuparable du fichier
      return -2;
    }
  else
    {
      if(verification(programme))
	{
	  
	}
    }
  
  if(nom_fichier!=NULL)
    {
      fclose(yyin);
    }
  
  return nb_erreurs;
}


