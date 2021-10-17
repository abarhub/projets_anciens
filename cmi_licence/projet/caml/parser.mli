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
;;
value Main :
  (lexing__lexbuf  -> token) -> lexing__lexbuf -> arbre__arbre list;;
