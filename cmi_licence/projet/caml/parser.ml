#open "obj";;
#open "parsing";;
#open "arbre";;

(* Line 6, file parser.ml *)
let yytransl = [|
  257 (* VAL *);
  258 (* ID *);
  259 (* PLUS *);
  260 (* MOINS *);
  261 (* FOIS *);
  262 (* DIV *);
  263 (* MOINSU *);
  264 (* EOL *);
  265 (* PARENTG *);
  266 (* PARENTD *);
  267 (* SIN *);
  268 (* LOG *);
  269 (* FIN_EXP *);
  270 (* VIRGULE *);
  271 (* COS *);
  272 (* ABS *);
    0|];;

let yylhs = "\255\255\
\001\000\002\000\002\000\003\000\004\000\004\000\004\000\004\000\
\004\000\004\000\004\000\004\000\004\000\004\000\004\000\004\000\
\004\000\000\000";;

let yylen = "\002\000\
\001\000\002\000\001\000\004\000\001\000\003\000\003\000\003\000\
\003\000\003\000\002\000\002\000\001\000\004\000\004\000\004\000\
\004\000\002\000";;

let yydefred = "\000\000\
\000\000\000\000\005\000\013\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\018\000\000\000\003\000\000\000\012\000\
\011\000\000\000\000\000\000\000\000\000\000\000\002\000\000\000\
\000\000\000\000\000\000\000\000\006\000\000\000\000\000\000\000\
\000\000\000\000\000\000\009\000\010\000\000\000\016\000\014\000\
\015\000\017\000\004\000";;

let yydgoto = "\002\000\
\012\000\013\000\014\000\015\000";;

let yysindex = "\004\000\
\022\255\000\000\000\000\000\000\022\255\022\255\022\255\253\254\
\002\255\003\255\018\255\000\000\022\255\000\000\004\255\000\000\
\000\000\039\255\022\255\022\255\022\255\022\255\000\000\022\255\
\022\255\022\255\022\255\011\255\000\000\049\255\057\255\065\255\
\073\255\254\254\254\254\000\000\000\000\017\255\000\000\000\000\
\000\000\000\000\000\000";;

let yyrindex = "\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\032\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\025\255\037\255\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000";;

let yygindex = "\000\000\
\000\000\000\000\023\000\251\255";;

let YYTABLESIZE = 83;;
let yytable = "\016\000\
\017\000\018\000\026\000\027\000\001\000\019\000\024\000\025\000\
\026\000\027\000\020\000\021\000\038\000\030\000\031\000\032\000\
\033\000\028\000\034\000\035\000\036\000\037\000\003\000\004\000\
\005\000\006\000\022\000\007\000\007\000\043\000\007\000\001\000\
\008\000\009\000\007\000\023\000\010\000\011\000\007\000\008\000\
\008\000\024\000\025\000\026\000\027\000\000\000\008\000\000\000\
\029\000\000\000\008\000\024\000\025\000\026\000\027\000\000\000\
\000\000\000\000\039\000\024\000\025\000\026\000\027\000\000\000\
\000\000\000\000\040\000\024\000\025\000\026\000\027\000\000\000\
\000\000\000\000\041\000\024\000\025\000\026\000\027\000\000\000\
\000\000\000\000\042\000";;

let yycheck = "\005\000\
\006\000\007\000\005\001\006\001\001\000\009\001\003\001\004\001\
\005\001\006\001\009\001\009\001\002\001\019\000\020\000\021\000\
\022\000\014\001\024\000\025\000\026\000\027\000\001\001\002\001\
\003\001\004\001\009\001\003\001\004\001\013\001\009\001\000\000\
\011\001\012\001\010\001\013\000\015\001\016\001\014\001\003\001\
\004\001\003\001\004\001\005\001\006\001\255\255\010\001\255\255\
\010\001\255\255\014\001\003\001\004\001\005\001\006\001\255\255\
\255\255\255\255\010\001\003\001\004\001\005\001\006\001\255\255\
\255\255\255\255\010\001\003\001\004\001\005\001\006\001\255\255\
\255\255\255\255\010\001\003\001\004\001\005\001\006\001\255\255\
\255\255\255\255\010\001";;

let yyact = [|
  (fun () -> failwith "parser")
(* Rule 1, file parser.mly, line 24 *)
; (fun () -> repr(( (peek_val 0 : 'Lignes) ) : arbre__arbre list))
(* Rule 2, file parser.mly, line 27 *)
; (fun () -> repr(( (peek_val 1 : 'Lignes) @ [(peek_val 0 : 'Ligne)] ) : 'Lignes))
(* Rule 3, file parser.mly, line 28 *)
; (fun () -> repr(( [ (peek_val 0 : 'Ligne) ] ) : 'Lignes))
(* Rule 4, file parser.mly, line 31 *)
; (fun () -> repr(( Arbre((peek_val 3 : 'Exp),(peek_val 1 : string)) ) : 'Ligne))
(* Rule 5, file parser.mly, line 35 *)
; (fun () -> repr(( Flotant((peek_val 0 : float)) ) : 'Exp))
(* Rule 6, file parser.mly, line 36 *)
; (fun () -> repr(( (peek_val 1 : 'Exp) ) : 'Exp))
(* Rule 7, file parser.mly, line 37 *)
; (fun () -> repr(( Plus((peek_val 2 : 'Exp),(peek_val 0 : 'Exp)) ) : 'Exp))
(* Rule 8, file parser.mly, line 38 *)
; (fun () -> repr(( Moins((peek_val 2 : 'Exp),(peek_val 0 : 'Exp)) ) : 'Exp))
(* Rule 9, file parser.mly, line 39 *)
; (fun () -> repr(( Fois((peek_val 2 : 'Exp),(peek_val 0 : 'Exp)) ) : 'Exp))
(* Rule 10, file parser.mly, line 40 *)
; (fun () -> repr(( Div((peek_val 2 : 'Exp),(peek_val 0 : 'Exp)) ) : 'Exp))
(* Rule 11, file parser.mly, line 42 *)
; (fun () -> repr(( Moins_u((peek_val 0 : 'Exp)) ) : 'Exp))
(* Rule 12, file parser.mly, line 44 *)
; (fun () -> repr(( Plus_u((peek_val 0 : 'Exp)) ) : 'Exp))
(* Rule 13, file parser.mly, line 45 *)
; (fun () -> repr(( Var((peek_val 0 : string)) ) : 'Exp))
(* Rule 14, file parser.mly, line 46 *)
; (fun () -> repr(( Log((peek_val 1 : 'Exp)) ) : 'Exp))
(* Rule 15, file parser.mly, line 47 *)
; (fun () -> repr(( Cos((peek_val 1 : 'Exp)) ) : 'Exp))
(* Rule 16, file parser.mly, line 48 *)
; (fun () -> repr(( Sin((peek_val 1 : 'Exp)) ) : 'Exp))
(* Rule 17, file parser.mly, line 49 *)
; (fun () -> repr(( Abs((peek_val 1 : 'Exp)) ) : 'Exp))
(* Entry Main *)
; (fun () -> raise (yyexit (peek_val 0)))
|];;
let yytables =
  { actions=yyact;
    transl=yytransl;
    lhs=yylhs;
    len=yylen;
    defred=yydefred;
    dgoto=yydgoto;
    sindex=yysindex;
    rindex=yyrindex;
    gindex=yygindex;
    tablesize=YYTABLESIZE;
    table=yytable;
    check=yycheck };;
let Main lexfun lexbuf = yyparse yytables 1 lexfun lexbuf;;
