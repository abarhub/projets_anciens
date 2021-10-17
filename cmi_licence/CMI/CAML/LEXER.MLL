{
(* le scanner qui utilise camllex *)
#open "parser";;
#open "excep";;
}

rule Token = parse
	[` ` `\t` `\n`]	{ Token lexbuf }
	| [`0`-`9`]+`.`[`0`-`9`]*
			{ VAL(float_of_string(get_lexeme lexbuf))
			(* un nombre a virgule avec un 
			chiffre avant le point decimal *) }
	| `.`[`0`-`9`]* { VAL(float_of_string(get_lexeme lexbuf))
			(* un nombre a virgule sans chiffre
			avant le point decimal *) }
	| [`0`-`9`]+	{ VAL(float_of_string(get_lexeme lexbuf))
			(* un entier qui sera traite comme un flottant *) }
	| "sin"		{ SIN }
	| "cos"		{ COS }
	| "abs"		{ ABS }
	| "log"		{ LOG }
	| [`a`-`z` `A`-`Z`][`a`-`z` `A`-`Z` `_` `0`-`9`]*
			{ ID(get_lexeme lexbuf)
			(* une variable *) }
	| `+`		{ PLUS }
	| `-`		{ MOINS }
	| `*`		{ FOIS }
	| `/`		{ DIV }
	| `(`		{ PARENTG }
	| `)`		{ PARENTD }
	| `,`		{ VIRGULE }
	| `;`		{ FIN_EXP }
	| eof		{ print_string("fin du fichier\n");flush std_out;
			(* raise Eof *)	FIN_FICH } 
	| `$`		{ FIN_FICH }
	| "(*"		{ commentaire_caml lexbuf
			(* un commentaire (les commentaire ne sont pas imbriques) *)}
	| _		{ failwith ("caractere inconnu:"^(get_lexeme lexbuf)) }
and commentaire_caml = parse (* un commentaire (les commentaires ne sont
				pas imbriques) *)
	"*)"	{ Token lexbuf }
	| _	{ commentaire_caml lexbuf }
;;
