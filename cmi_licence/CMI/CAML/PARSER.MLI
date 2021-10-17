type token =
    VAL of float
  | ID of string
  | PLUS
  | MOINS
  | FOIS
  | DIV
  | MOINSU
  | EOL
  | PARENTG
  | PARENTD
  | SIN
  | LOG
  | FIN_EXP
  | VIRGULE
  | COS
  | ABS
  | FIN_FICH
;;
value Main :
  (lexing__lexbuf  -> token) -> lexing__lexbuf -> arbre__liste_expression;;
