%{
#include<string.h>
#include "type.h"
#include "parse.tab.h"
#include "debugage.h"
//#include"gc.h"

/* il faut enlever les commentaires pour le debugage */
//#define DEBUG_LEX

#ifdef DEBUG_LEX
/* afficheage des printf */
#define VIDE(X) X
#else
/* elimination des printf */
#define VIDE(X) 
#endif

int no_ligne;
//int yywrap();
//#define yywrap() {return 1;}
void *gc_strdup(char*);

%}

ID [a-zA-Z_][a-zA-Z_0-9]*
DIGIT [0-9]+

%x str com1 com2


%option noyywrap


%%

"Program"       {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_PROGRAM;}
"begin"         {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_BEGIN;}
"end"           {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_END;}
"for"           {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_FOR;}
"if"            {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_IF;}
"then"          {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_THEN;}
"else"          {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_ELSE;}
"break"         {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_BREAK;}
"continue"      {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_CONTINUE;}
"void"          {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_VOID;}
"bool"          {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_BOOL;}
"int"           {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_INT;}
"img"           {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_IMG;}
"string"        {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_STRING;}
"load"          {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_LOAD;}
"unload"        {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_UNLOAD;}
"map"           {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_MAP;}
"unmap"         {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_UNMAP;}
"true"          {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_TRUE;}
"false"         {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_FALSE;}
"return"        {VIDE(printf("MOT CLE\t\t%s\n",yytext));return TK_RETURN;}

{ID}            { 
            VIDE(printf("ID\t\t%s\n",yytext));
			yylval.string=(char*)gc_strdup(yytext);
			return ID; 
		}

{DIGIT}         {
                    VIDE(printf("NOMBRE\t\t%s\n",yytext));
                    yylval.val=atoi(yytext);return NOMBRE;
                }

\n              {
                    VIDE(printf("RETOUR LIGNE-----------------\n"));
                    no_ligne++;
                }

[ \t\r]+          {}

\"              { BEGIN(str);yymore(); }

<str>\\\\       { yymore(); }
<str>\\\"       { yymore(); }
<str>\"         {
                    VIDE(printf("CHAINE\t\t%s\n",yytext));
                    BEGIN(INITIAL);yylval.string=(char*)gc_strdup(yytext);
                    return CHAINE;
                }
<str>\n         { 
                    no_ligne++;
                    printf("Erreur: fin de ligne dans une"
                            " chaine de caractere\n");
                    BEGIN(INITIAL);
                }
<str><<EOF>>    { 
                    printf("Erreur: fin de fichier dans une"
                        " chaine de caractere\n");
                    BEGIN(INITIAL);
                }
<str>.          {yymore();}

"//"            { BEGIN(com1); }
<com1>.         {}
<com1>\n        { no_ligne++; BEGIN(INITIAL); }

"/*"            { BEGIN(com2);}
<com2>"*/"              {BEGIN(INITIAL);}
<com2>.|\n      { if(yytext[0]=='\n') no_ligne++; }

"<"             {return TEST_INF_S;}
">"             {return TEST_SUP_S;}
"<="            {return TEST_INF;}
">="            {return TEST_SUP;}
"<>"            {return TEST_DIFFERENT;}
"=="            {return TEST_EGAL;}
"||"            {return TEST_OU;}
"&&"            {return TEST_ET;}

.               { VIDE(printf("CARACTERE\t\t%c(%d)\n",yytext[0],yytext[0]));
                 return yytext[0];}

%%

/*int yywrap()
  {
  return 1;
  }*/
