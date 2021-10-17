#open "arbre";;

(* fonctions pour deriver *)

let rec deriv_rec var exp =
             match exp with
               Flotant(f) -> Flotant 0.
	| Var (var1) -> if eq_string var var1 then
                            Flotant 1. else Flotant 0.
	| Plus (e1,e2) -> Plus(deriv_rec var e1, deriv_rec var e2)
	| Fois (e1,e2) -> Plus (Fois(deriv_rec var e1,e2),Fois(deriv_rec var e2,e1))
	| Div (e1,e2) -> Div(
			Moins(
				Fois(deriv_rec var e1,e2),Fois(deriv_rec var e2,e1)),
				Fois(e2,e2))
	| Moins (e1,e2) -> Moins(deriv_rec var e1, deriv_rec var e2)
	| Moins_u (e) -> Moins_u (deriv_rec var e)
	| Plus_u (e) -> Plus_u (deriv_rec var e)
	| Sin (e) -> Fois(deriv_rec var e,Cos (e))
	| Log (e) -> Div (Abs(deriv_rec var e),e)
	| Cos (e) -> Fois(deriv_rec var e,Moins_u (Sin (e)))
	| Abs (e) -> e
;;

let derivation arbre =
   match arbre with
          Arbre (exp,var) ->
               deriv_rec var exp
          | Vide -> failwith "Impossible de deriver Rien"
          ;;
