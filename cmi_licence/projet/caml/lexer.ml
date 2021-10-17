#open "obj";;
#open "lexing";;


#open "parser";;
#open "excep";;


let rec action_18 lexbuf = ((
 failwith ("caractere inconnu:"^(get_lexeme lexbuf)) ) : 'Token)
and action_17 lexbuf = ((
 raise Eof ) : 'Token)
and action_16 lexbuf = ((
 FIN_EXP ) : 'Token)
and action_15 lexbuf = ((
 VIRGULE ) : 'Token)
and action_14 lexbuf = ((
 PARENTD ) : 'Token)
and action_13 lexbuf = ((
 PARENTG ) : 'Token)
and action_12 lexbuf = ((
 DIV ) : 'Token)
and action_11 lexbuf = ((
 FOIS ) : 'Token)
and action_10 lexbuf = ((
 MOINS ) : 'Token)
and action_9 lexbuf = ((
 PLUS ) : 'Token)
and action_8 lexbuf = ((
 ID(get_lexeme lexbuf) ) : 'Token)
and action_7 lexbuf = ((
 LOG ) : 'Token)
and action_6 lexbuf = ((
 ABS ) : 'Token)
and action_5 lexbuf = ((
 COS ) : 'Token)
and action_4 lexbuf = ((
 SIN ) : 'Token)
and action_3 lexbuf = ((
 VAL(float_of_string(get_lexeme lexbuf)) ) : 'Token)
and action_2 lexbuf = ((
 VAL(float_of_string(get_lexeme lexbuf)) ) : 'Token)
and action_1 lexbuf = ((
 VAL(float_of_string(get_lexeme lexbuf)) ) : 'Token)
and action_0 lexbuf = ((
 Token lexbuf ) : 'Token)
and state_0 lexbuf =
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`r`|`q`|`p`|`o`|`n`|`m`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`b`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A` -> state_14 lexbuf
 |  `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_12 lexbuf
 |  ` `|`\010`|`\t` -> action_0 lexbuf
 |  `s` -> state_18 lexbuf
 |  `l` -> state_17 lexbuf
 |  `c` -> state_16 lexbuf
 |  `a` -> state_15 lexbuf
 |  `;` -> action_16 lexbuf
 |  `/` -> action_12 lexbuf
 |  `.` -> state_10 lexbuf
 |  `-` -> action_10 lexbuf
 |  `,` -> action_15 lexbuf
 |  `+` -> action_9 lexbuf
 |  `*` -> action_11 lexbuf
 |  `)` -> action_14 lexbuf
 |  `(` -> action_13 lexbuf
 |  `\000` -> action_17 lexbuf
 |  _ -> action_18 lexbuf
and state_10 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_2;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_30 lexbuf
 |  _ -> backtrack lexbuf
and state_12 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_3;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_29 lexbuf
 |  `.` -> state_28 lexbuf
 |  _ -> backtrack lexbuf
and state_14 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  _ -> backtrack lexbuf
and state_15 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  `b` -> state_26 lexbuf
 |  _ -> backtrack lexbuf
and state_16 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  `o` -> state_24 lexbuf
 |  _ -> backtrack lexbuf
and state_17 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  `o` -> state_22 lexbuf
 |  _ -> backtrack lexbuf
and state_18 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  `i` -> state_20 lexbuf
 |  _ -> backtrack lexbuf
and state_19 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  _ -> backtrack lexbuf
and state_20 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  `n` -> state_21 lexbuf
 |  _ -> backtrack lexbuf
and state_21 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_4;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  _ -> backtrack lexbuf
and state_22 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  `g` -> state_23 lexbuf
 |  _ -> backtrack lexbuf
and state_23 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_7;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  _ -> backtrack lexbuf
and state_24 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  `s` -> state_25 lexbuf
 |  _ -> backtrack lexbuf
and state_25 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_5;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  _ -> backtrack lexbuf
and state_26 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_8;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  `s` -> state_27 lexbuf
 |  _ -> backtrack lexbuf
and state_27 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_6;
  match get_next_char lexbuf with
    `z`|`y`|`x`|`w`|`v`|`u`|`t`|`s`|`r`|`q`|`p`|`o`|`n`|`m`|`l`|`k`|`j`|`i`|`h`|`g`|`f`|`e`|`d`|`c`|`b`|`a`|`_`|`Z`|`Y`|`X`|`W`|`V`|`U`|`T`|`S`|`R`|`Q`|`P`|`O`|`N`|`M`|`L`|`K`|`J`|`I`|`H`|`G`|`F`|`E`|`D`|`C`|`B`|`A`|`9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_19 lexbuf
 |  _ -> backtrack lexbuf
and state_28 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_1;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_28 lexbuf
 |  _ -> backtrack lexbuf
and state_29 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_3;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_29 lexbuf
 |  `.` -> state_28 lexbuf
 |  _ -> backtrack lexbuf
and state_30 lexbuf =
  lexbuf.lex_last_pos <- lexbuf.lex_curr_pos;
  lexbuf.lex_last_action <- magic action_2;
  match get_next_char lexbuf with
    `9`|`8`|`7`|`6`|`5`|`4`|`3`|`2`|`1`|`0` -> state_30 lexbuf
 |  _ -> backtrack lexbuf
and Token lexbuf =
  start_lexing lexbuf;
  (state_0 lexbuf : 'Token)
;;
