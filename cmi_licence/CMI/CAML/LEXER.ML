#open "obj";;
#open "lexing";;


(* le scanner qui utilise camllex *)
#open "parser";;
#open "excep";;

let rec action_22 lexbuf = ((
 failwith ("caractere inconnu:"^(get_lexeme lexbuf)) ) : 'Token)
and action_21 lexbuf = ((
 commentaire_caml lexbuf
			(* un commentaire (les commentaire ne sont pas imbriques) *)) : 'Token)
and action_20 lexbuf = ((
 FIN_FICH ) : 'Token)
and action_19 lexbuf = ((
 print_string("fin du fichier\n");flush std_out;
			(* raise Eof *)	FIN_FICH ) : 'Token)
and action_18 lexbuf = ((
 FIN_EXP ) : 'Token)
and action_17 lexbuf = ((
 VIRGULE ) : 'Token)
and action_16 lexbuf = ((
 PARENTD ) : 'Token)
and action_15 lexbuf = ((
 PARENTG ) : 'Token)
and action_14 lexbuf = ((
 DIV ) : 'Token)
and action_13 lexbuf = ((
 FOIS ) : 'Token)
and action_12 lexbuf = ((
 MOINS ) : 'Token)
and action_11 lexbuf = ((
 PLUS ) : 'Token)
and action_10 lexbuf = ((
 ID(get_lexeme lexbuf)
			(* une variable *) ) : 'Token)
and action_9 lexbuf = ((
 LOG ) : 'Token)
and action_8 lexbuf = ((
 ABS ) : 'Token)
and action_7 lexbuf = ((
 COS ) : 'Token)
and action_6 lexbuf = ((
 SIN ) : 'Token)
and action_5 lexbuf = ((
 VAL(float_of_string(get_lexeme lexbuf))
			(* un entier qui sera traite comme un flottant *) ) : 'Token)
and action_4 lexbuf = ((
 VAL(float_of_string(get_lexeme lexbuf))
			(* un nombre a virgule sans chiffre
			avant le point decimal *) ) : 'Token)
and action_3 lexbuf = ((
 VAL(float_of_string(get_lexeme lexbuf))
			(* un nombre a virgule avec un 
			chiffre avant le point decimal *) ) : 'Token)
and action_2 lexbuf = ((
 Token lexbuf ) : 'Token)
and action_1 lexbuf = ((
 commentaire_caml lexbuf ) : 'commentaire_caml)
and action_0 lexbuf = ((
 Token lexbuf ) : 'commentaire_caml)
and state_0 lexbuf =
  match get_next_char lexbuf with
    `*` -> state_35 lexbuf
 |  `\000` -> backtrack lexbuf
 |  _ -> action_1 lexbuf
and state_1 lexbuf =
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`r`|`q`|`p`|`o`|`n`|`m`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`b`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A` -> state_16 lexbuf
 |  `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_14 lexbuf
 |  ` `|`\010`|`\t` -> action_2 lexbuf
 |  `s` -> state_20 lexbuf
 |  `l` -> state_19 lexbuf
 |  `c` -> state_18 lexbuf
 |  `a` -> state_17 lexbuf
 |  `;` -> action_18 lexbuf
 |  `/` -> action_14 lexbuf
 |  `.` -> state_12 lexbuf
 |  `-` -> action_12 lexbuf
 |  `,` -> action_17 lexbuf
 |  `+` -> action_11 lexbuf
 |  `*` -> action_13 lexbuf
 |  `)` -> action_16 lexbuf
 |  `(` -> state_6 lexbuf
 |  `$` -> action_20 lexbuf
 |  `\000` -> action_19 lexbuf
 |  _ -> action_22 lexbuf
and state_6 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_15;
  match get_next_char lexbuf with
    `*` -> action_21 lexbuf
 |  _ -> backtrack lexbuf
and state_12 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_4;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_32 lexbuf
 |  _ -> backtrack lexbuf
and state_14 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_5;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_31 lexbuf
 |  `.` -> state_30 lexbuf
 |  _ -> backtrack lexbuf
and state_16 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  _ -> backtrack lexbuf
and state_17 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  `b` -> state_28 lexbuf
 |  _ -> backtrack lexbuf
and state_18 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  `o` -> state_26 lexbuf
 |  _ -> backtrack lexbuf
and state_19 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  `o` -> state_24 lexbuf
 |  _ -> backtrack lexbuf
and state_20 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  `i` -> state_22 lexbuf
 |  _ -> backtrack lexbuf
and state_21 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  _ -> backtrack lexbuf
and state_22 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  `n` -> state_23 lexbuf
 |  _ -> backtrack lexbuf
and state_23 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_6;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  _ -> backtrack lexbuf
and state_24 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  `g` -> state_25 lexbuf
 |  _ -> backtrack lexbuf
and state_25 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_9;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  _ -> backtrack lexbuf
and state_26 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  `s` -> state_27 lexbuf
 |  _ -> backtrack lexbuf
and state_27 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_7;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  _ -> backtrack lexbuf
and state_28 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_10;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  `s` -> state_29 lexbuf
 |  _ -> backtrack lexbuf
and state_29 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_21 lexbuf
 |  _ -> backtrack lexbuf
and state_30 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_3;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_30 lexbuf
 |  _ -> backtrack lexbuf
and state_31 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_5;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_31 lexbuf
 |  `.` -> state_30 lexbuf
 |  _ -> backtrack lexbuf
and state_32 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_4;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_32 lexbuf
 |  _ -> backtrack lexbuf
and state_35 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_1;
  match get_next_char lexbuf with
    `)` -> action_0 lexbuf
 |  _ -> backtrack lexbuf
and Token lexbuf =
  start_lexing lexbuf;
  (state_1 lexbuf : 'Token)

and commentaire_caml lexbuf =
  start_lexing lexbuf;
  (state_0 lexbuf : 'commentaire_caml)
;;
