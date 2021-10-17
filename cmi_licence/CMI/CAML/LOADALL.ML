(* compilation mais pas camllex ni camlyacc *)
compile "arbre.mli";;
compile "excep.mli";;
(* camllex lexer.mll *)
(* camlyacc parser.mly *)
compile "parser.mli";;
compile "parser.ml";;
compile "lexer.ml";;
compile "simpl.mli";;
compile "simpl.ml";;
compile "deriv.mli";;
compile "deriv.ml";;
compile "affiche.mli";;
compile "affiche.ml";;
compile "main.ml";;

(* chargement, et execution pendant la chargement de main *)
load_object "parser.zo";;
load_object "lexer.zo";;
load_object "simpl.zo";;
load_object "deriv.zo";;
load_object "affiche.zo";;
load_object "main.zo";;
