{
#open "parser";;
#open "excep";;

}

rule Token = parse
	[` ` `\t` `\n`]	{ Token lexbuf }
	| [`0`-`9`]+`.`[`0`-`9`]*	
			{ VAL(float_of_string(get_lexeme lexbuf)) }
	| `.`[`0`-`9`]* { VAL(float_of_string(get_lexeme lexbuf)) }
	| [`0`-`9`]+	{ VAL(float_of_string(get_lexeme lexbuf)) }
	| "sin"		{ SIN }
	| "cos"		{ COS }
	| "abs"		{ ABS }
	| "log"		{ LOG }
	| [`a`-`z` `A`-`Z`][`a`-`z` `A`-`Z` `_` `0`-`9`]*
			{ ID(get_lexeme lexbuf) }
	| `+`		{ PLUS }
	| `-`		{ MOINS }
	| `*`		{ FOIS }
	| `/`		{ DIV }
	| `(`		{ PARENTG }
	| `)`		{ PARENTD }
	| `,`		{ VIRGULE }
	| `;`		{ FIN_EXP }
	| eof		{ raise Eof }
	| _		{ failwith ("caractere inconnu:"^(get_lexeme lexbuf)) }
;;
