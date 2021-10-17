
%{
#open "arbre";;

%}

%token <float> VAL
%token <string> ID
%token PLUS MOINS FOIS DIV MOINSU
%token EOL PARENTG PARENTD SIN LOG
%token FIN_EXP VIRGULE COS ABS

%left PLUS MOINS
%left FOIS DIV
%nonassoc MOINS_U PLUS_U

%start Main

%type <arbre__arbre list> Main

%%

Main: 
	Lignes			{ $1 }
;

Lignes: Lignes Ligne		{ $1 @ [$2] }
	| Ligne			{ [ $1 ] }

Ligne:
	Exp VIRGULE ID FIN_EXP	{ Arbre($1,$3) }
;

Exp:
	VAL			{ Flotant($1) }
	| PARENTG Exp PARENTD	{ $2 }
	| Exp PLUS Exp		{ Plus($1,$3) }
	| Exp MOINS Exp		{ Moins($1,$3) }
	| Exp FOIS Exp		{ Fois($1,$3) }
	| Exp DIV Exp		{ Div($1,$3) }
	| MOINS Exp  %prec MOINS_U
				{ Moins_u($2) }
	| PLUS Exp  %prec PLUS_U
				{ Plus_u($2) }
	| ID			{ Var($1) }
	| LOG PARENTG Exp PARENTD	{ Log($3) }
	| COS PARENTG Exp PARENTD	{ Cos($3) }
	| SIN PARENTG Exp PARENTD	{ Sin($3) }
	| ABS PARENTG Exp PARENTD	{ Abs($3) }
;
