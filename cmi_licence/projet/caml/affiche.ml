#open "arbre";;

(* fonction d'affichage de l'arbre *)

(* retourne la "priorite" de l'operateur racine de l'expression *)
let prio = function
	  Plus(_,_) -> 1
	| Fois(_,_) -> 2
	| Moins(_,_) -> 1
	| Div(_,_) -> 2
	| Moins_u(_) -> 3
	| Plus_u(_) -> 3
	| Flotant (_) -> 0
	| Var(_) -> 0
	| _ ->0;;

(* si l'operateur racine dans l'expr e1 a une priorite
inferieur a l'operateur racine de l'expression ancetre,
affiche le chaine s *)
let prio_inf e1 priorite_ancetre s = 
	let x=prio e1
		in if x<priorite_ancetre then print_string s else ();;

(* affiche l'expression *)
let rec affiche_rec exp pri =match (exp,pri) with
	| (Flotant(f),pri)	-> 
		print_string " ";
		print_float f;
		print_string " ";
		flush std_out;
	| (Var(nom),pri)	-> 
		print_string (" "^nom^" ");
		flush std_out;
	| (Plus(e1,e2) as op,pri)	->
		prio_inf op pri "(";
		affiche_rec e1 (prio op);
		(* prio_inf op pri ")"; *)
		print_string "+";
		(* prio_inf op pri "("; *)
		affiche_rec e2 (prio op);
		prio_inf op pri ")";
		flush std_out;
	| (Fois(e1,e2) as op,pri)	->
		prio_inf op pri "(";
		affiche_rec e1 (prio op);
		(* prio_inf op pri ")"; *)
		print_string "*";
		(* prio_inf op pri "("; *)
		affiche_rec e2 (prio op);
		prio_inf op pri ")";
		flush std_out;
	| (Div(e1,e2) as op,pri)	->
		prio_inf op pri "(";
		affiche_rec e1 (prio op);
		(* prio_inf op pri ")"; *)
		print_string "/";
		(* prio_inf op pri "("; *)
		affiche_rec e2 (prio op);
		prio_inf op pri ")";
		flush std_out;
	| (Moins(e1,e2) as op,pri)	->
		prio_inf op pri "(";
		affiche_rec e1 (prio op);
		(* prio_inf op pri ")"; *)
		print_string "-";
		(* prio_inf op pri "("; *)
		affiche_rec e2 (prio op);
		prio_inf op pri ")";
		flush std_out;
	| (Moins_u(exp) as op,pri)	->
		(*prio_inf op pri "(";*)
		print_string("-");
		affiche_rec exp (prio op);
		(*prio_inf op pri ")";*)
		flush std_out;
	| (Plus_u(exp) as op,pri)	->
		print_string("+");
		(*prio_inf op pri "(";*)
		affiche_rec exp (prio op);
		(*prio_inf op pri ")";*)
		flush std_out;
	| (Sin(exp) as op,pri)	->
		print_string("sin(");
		affiche_rec exp (prio op);
		print_string ")"; 
		flush std_out;
	| (Log(exp) as op,pri)	->
		print_string("log(");
		affiche_rec exp (prio op);
		print_string ")";
		flush std_out;
	| (Cos(exp) as op,pri)	->
		print_string("cos(");
		affiche_rec exp (prio op);
		print_string ")";
		flush std_out;
	| (Abs(exp) as op,pri)	->
		print_string("abs(");
		affiche_rec exp (prio op);
		print_string ")";
		flush std_out;
;;

(* affiche l'expression *)
let affiche exp =
	affiche_rec exp (prio exp);
	print_newline ();
	flush std_out
;;
