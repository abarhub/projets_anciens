#open "arbre";;
#open "excep";;

(* fonctions de simplification *)

(* type de l'arbre pour faciliter les simplifications *)
type arbre_liste =
	LPlus of arbre_liste list
	| LFois of arbre_liste list
	| LDiv of arbre_liste (* 1/exp *)
	| LMoins_u of arbre_liste
	| LSin of arbre_liste
	| LCos of arbre_liste
	| LLog of arbre_liste
	| LAbs of arbre_liste
	| LFlottant of float
	| LVar of string
	| LMoins of arbre_liste * arbre_liste
	| LDiv2 of arbre_liste * arbre_liste
;;

(* construit une liste d'expression qui correspond a une somme de plusieurs
	termes *)
let rec ajoute_somme e1 e2 = match (e1,e2) with
	(LPlus exp1,LPlus exp2) -> LPlus (exp1 @ exp2)
	| (LPlus exp1, exp2) -> LPlus ( exp1 @ [exp2] )
	|(exp1,LPlus exp2) -> LPlus ( [exp1] @ exp2 )
	| (exp1,exp2) -> LPlus ( exp1::[exp2] )
;;

(* construit une liste d'expression qui correspond a un produit de plusieurs
	facteurs *)
let rec ajoute_produit e1 e2 = match (e1,e2) with
	(LFois exp1,LFois exp2) -> LFois (exp1 @ exp2)
	| (LFois exp1, exp2) -> LFois ( exp1 @ [exp2] )
	|(exp1,LFois exp2) -> LFois ( [exp1] @ exp2 )
	| (exp1,exp2) -> LFois ( exp1::[exp2] )
;;	

(* transforme l'arbre en arbre_liste pour permettres les simplifications *)
let rec arbre_liste_of_arbre = function
	| Plus(e1,e2) -> ajoute_somme (arbre_liste_of_arbre e1) 
		(arbre_liste_of_arbre e2)
	| Moins (e1,e2) -> ajoute_somme (arbre_liste_of_arbre e1) 
		(arbre_liste_of_arbre (Moins_u e2))
	| Flottant f -> LFlottant f
	| Var v -> LVar v
	| Fois (e1,e2) -> ajoute_produit (arbre_liste_of_arbre e1)
		(arbre_liste_of_arbre e2)
	| Div (e1,e2) -> ajoute_produit (arbre_liste_of_arbre e1)
		(LDiv (arbre_liste_of_arbre e2))
	| Plus_u e -> arbre_liste_of_arbre e
	| Sin e -> LSin (arbre_liste_of_arbre e)
	| Cos e -> LCos (arbre_liste_of_arbre e)
	| Log e -> LLog (arbre_liste_of_arbre e)
	| Abs e -> LAbs (arbre_liste_of_arbre e)
	| Moins_u (Flottant f) -> LFlottant (-. f)
	| Moins_u e -> LMoins_u (arbre_liste_of_arbre e)
;;

(* transforme l'arbre_liste en arbre pour permettres l'affichage *)
let rec arbre_of_arbre_liste = function
	| LPlus [a] -> arbre_of_arbre_liste (a)
	| LPlus [] -> raise LPlus_vide
	| LPlus (*l*) (a::q) -> 
		(*construit_plus_arbre l*)
		 Plus (arbre_of_arbre_liste a,
			arbre_of_arbre_liste (LPlus q)) 
	| LFois [a] -> arbre_of_arbre_liste (a)
	| LFois [] -> raise LFois_vide
	| LFois (*l*) (a::q) -> 
		(*construit_fois_arbre l*)
		Fois (arbre_of_arbre_liste a,
			arbre_of_arbre_liste (LFois q)) 
	| LDiv d -> Div (Flottant 1.,arbre_of_arbre_liste d)
	| LVar v -> Var v
	| LFlottant f -> Flottant f
	| LMoins_u e -> Moins_u (arbre_of_arbre_liste e)
	| LSin e -> Sin (arbre_of_arbre_liste e)
	| LCos e -> Cos (arbre_of_arbre_liste e)
	| LLog e -> Log (arbre_of_arbre_liste e)
	| LAbs e -> Abs (arbre_of_arbre_liste e)
	| LMoins (e1,e2) -> Moins (arbre_of_arbre_liste e1,
				arbre_of_arbre_liste e2)
	| LDiv2 (e1,e2) -> Div (arbre_of_arbre_liste e1,
				arbre_of_arbre_liste e2)
(* construit les Plus de type arbre, et 
regroupe le moins a la fin*)
(*and construit_plus_arbre l =
	(* retourne un couple de liste avec pour 2eme composante, les termes
	qui commence par LMoins_u et pour la 1ere composante, les autres *)
	let rec separe suite plus moins = match suite with
		[] -> (rev plus,rev moins)
		| ((LMoins_u exp)::q) -> separe q plus (exp::moins)
		| (exp::q) -> separe q (exp::plus) moins
	(* transforme la liste en plusieurs Plus binaire *)
	and construit_somme = function
			[] -> raise LPlus_vide
			|[a] -> arbre_of_arbre_liste a
			(*| [a,b] -> Plus(arbre_of_arbre_liste a,
						arbre_of_arbre_liste b)
			*)| ((a:arbre_liste)::q) -> Plus(arbre_of_arbre_liste a,
						construit_somme q)
	in match separe l [] [] with
		([],[]) -> raise LPlus_vide
		| ([],l) -> Moins_u (construit_plus_arbre l)
		| (l,[]) -> construit_somme l
		| (plus,moins) -> 
			Moins(construit_plus_arbre  plus,
				construit_plus_arbre  moins)
(* construit le Fois de type arbre, et regroupe les div a la fin *)
and construit_fois_arbre l =
	(* retourne un couple de liste avec pour 2eme composante, les termes
	qui commence par LDiv et pour la 1ere composante, les autres *)
	let rec separe suite plus moins = match suite with
		[] -> (rev plus,rev moins)
		| ((LDiv exp)::q) -> separe q plus (exp::moins)
		| (exp::q) -> separe q (exp::plus) moins
	(* transforme la liste en plusieurs Fois binaire *)
	and construit_produit = function
			[] -> raise LFois_vide
			|[a] -> arbre_of_arbre_liste a
			| ((a:arbre_liste)::q) -> Fois(arbre_of_arbre_liste a,
						construit_produit q)
	in match separe l [] [] with
		([],[]) -> raise LFois_vide
		| ([],l) -> Div (Flottant 1.,construit_fois_arbre l)
		| (l,[]) -> construit_produit l
		| (plus,moins) -> 
			Div(construit_fois_arbre  plus,
				construit_fois_arbre  moins)
*)
;;

(* retourne la 1ere , 2eme et 3eme composante du triplet *)
let prm = function (a,_,_) -> a;;
let sec = function (_,a,_) -> a;;
let trs = function (_,_,a) -> a;;

(* retourne un couple de liste tel que tous elements qui est dans les
2 ne soit enleve de l'un et de l'autre une fois (donc enleve <> de subtract ) 
ne marche pas ####################*)
let diff a b =
	let res= intersect a b in
	let rec enleve l = function
		[] -> l
		| (a::q) -> enleve (except a l) q 
	in
	(enleve a res,enleve b res)
;;

(* ces 3 fonctions recursives s'appellent les unes les autres,
et retourne un couple dont la seconde composante est a true ssi
la fonction a effectuer des simplifications, ce qui signifie que
le point fixe n'a pas encore ete attend. *)

(* simplifie la liste de terme (on suppose que c'est une liste de somme) *)
let rec simplifie_plus l =
	let rec simple_plus l val c = match (l,c) with
		| ([],c) -> ([],val,c)
		| ((LFlottant f)::q,_) -> 
			if val=0. then (* la 1ere valeur non nulle de la liste *)
				simple_plus q f false
				else simple_plus q (val+.f) true
		| ((LMoins_u(LPlus l))::q,_) ->
			(* transforme l'oppose d'une somme en somme d'opposee *)
			((map (fun x-> LMoins_u x) l)@q,val,true)
		| (a::q,c) -> let res=simple_plus q val c and
				res2=simple a in
			((fst res2)::(prm res),sec res,trs res or snd res2)
	in let res=simple_plus l 0. false in
		if((prm res)=[]) then ([LFlottant (sec res)],trs res)
			else if((sec res)=0.) then (prm res,trs res)
			else ((LFlottant (sec res))::(prm res),trs res)
(* simplifie la liste de terme (on suppose que c'est une liste de somme) *)
and simplifie_fois l =
	let rec simple_fois l val c = match (l,c) with
		| ([],c) -> ([], val,c)
		| (((LFlottant f)::q),_) -> 
			if val=1. then 
				simple_fois q f false
				else simple_fois q (val*.f) true
		| (((LDiv (LFlottant 0.))::q),_) -> raise Division_par_zero 
		| (((LDiv (LFlottant f))::q),_) -> simple_fois q (val /. f) true
		| ((a::q),c) -> let res=simple_fois q val c and
				res2=simple a in
			((fst res2)::(prm res),sec res,trs res or snd res2)
	in let res=simple_fois l 1. false in
	if((prm res)=[]) then ([LFlottant (sec res)],trs res)
			else if((sec res)=1.) then (prm res, trs res)
			else if((sec res)=0.) then ([LFlottant (0.)], true)
			else ((LFlottant (sec res))::(prm res),trs res)
(* effectue les simplifications simples ou appelle l'une des 2 fonctions
precedantes. Si on veut  simplifier, c'est elle qu'il faut appeller,
et elle appellera les autres fonctions si necessaire *)
and simple = function
	| LPlus l -> 
		let res=simplifie_plus l in
			(*print_string ("simple_iter log1"^
				(string_of_bool (snd res)));*)
			flush stdout;
			if list_length (fst res)=1 then
				(* il n'y a qu'une seule valeur dans la liste
				donc l'opperateur LPlus est enleve *)
				(hd (fst res),snd res)
				else (LPlus (fst res),snd res)
	| LFois l -> 
		let res=simplifie_fois l in
			(*print_string "simple_iter log2";
			flush stdout;*)
			if list_length (fst res)=1 then
				(* il n'y a qu'une seule valeur dans la liste
				donc l'opperateur LFois est enleve *)
				(hd (fst res),true) 
				else (LFois (fst res),snd res)
	| LDiv (LFlottant 0.) -> (* un divise par zero *)
			raise Division_par_zero
	| LDiv (LFlottant f) -> (* un divise par un nombre *)
			(LFlottant (1./.f),true)
	| LDiv exp -> 
		let res=simple exp in
			(*print_string "simple_iter log3";
			flush stdout;*)
			(LDiv (fst res),snd res)
	| LMoins_u (LFlottant f) -> (LFlottant (-. f),true)
	| LMoins_u exp -> 
		let res=simple exp in
			(*print_string "simple_iter log4";
			flush stdout;*)
			(LMoins_u (fst res),snd res)
	| LAbs (LFlottant f) -> (LFlottant (abs_float(f)),true)
	| LAbs exp -> 
		let res=simple exp in
			(*print_string "simple_iter log6";
			flush stdout;*)
			(LAbs (fst res),snd res)
	| LSin exp -> 
		let res=simple exp in
			(*print_string "simple_iter log5";
			flush stdout;*)
			(LSin (fst res),snd res)
	| LCos exp -> 
		let res=simple exp in
			(*print_string "simple_iter log7";
			flush stdout;*)
			(LCos (fst res),snd res)
	| LLog exp -> 
		let res=simple exp in 
			(*print_string "simple_iter log8";
			flush stdout;*)
			(LLog (fst res),snd res)
	| exp -> (*print_string "simple_iter9";
		flush stdout;*)
		(exp,false)
;;

(* regroupe le moins a la fin*)
let rec construit_plus_arbre2 l =
	(* retourne un couple de liste avec pour 2eme composante, les termes
	qui commence par LMoins_u et pour la 1ere composante, les autres *)
	let rec separe suite plus moins = match suite with
		[] -> (rev plus,rev moins)
		| ((LMoins_u exp)::q) -> 
			separe q plus (remettre_moins_div exp::moins)
		| (exp::q) -> separe q (remettre_moins_div exp::plus) moins
	in let (plus,moins)= separe l [] [] in 
	(* enleve ce qui est dans plus et moins *)
	let (res_plus,res_moins)= diff plus moins in
	match (res_plus,res_moins) with
		([],[]) -> LFlottant 0.
		| ([],l) -> LMoins_u (construit_plus_arbre2 l)
		| (l,[]) -> LPlus l
		| (plus,moins) ->  
			LMoins(construit_plus_arbre2  plus,
				construit_plus_arbre2  moins)
(* regroupe les div a la fin *)
and construit_fois_arbre2 l =
	(* retourne un couple de liste avec pour 2eme composante, les termes
	qui commence par LDiv et pour la 1ere composante, les autres *)
	let rec separe suite plus moins = match suite with
		[] -> (rev plus,rev moins)
		| ((LDiv exp)::q) -> 
			separe q plus (remettre_moins_div exp::moins)
		| (exp::q) -> separe q (remettre_moins_div exp::plus) moins
	in let (plus,moins)= separe l [] [] in
	(* enleve ce qui est dans plus et moins *)
	let inter= intersect plus moins in
		let res_plus= subtract plus inter and
			res_moins= subtract moins inter in
	match (res_plus,res_moins) with
		([],[]) -> LFlottant 1.
		| ([],l) -> LDiv2 (LFlottant 1.,construit_fois_arbre2 l)
		| (l,[]) -> LFois l
		| (plus,moins) -> 
				LDiv2(construit_fois_arbre2  res_plus,
					construit_fois_arbre2  res_moins)
(* regroupe les produit, les quotients, les sommes et les differences *)
and remettre_moins_div l= match l with
	LPlus list_exp -> construit_plus_arbre2(list_exp)
	| LFois list_exp -> construit_fois_arbre2(list_exp)
	| LDiv exp -> LDiv (remettre_moins_div exp)
	| LMoins_u exp -> LMoins_u (remettre_moins_div exp)
	| LSin exp -> LSin (remettre_moins_div exp)
	| LCos exp -> LCos (remettre_moins_div exp)
	| LLog exp -> LLog (remettre_moins_div exp)
	| LAbs exp -> LAbs (remettre_moins_div exp)
	| LVar var -> LVar var
	| LFlottant f -> LFlottant f
	| LMoins (exp1,exp2) -> 
		LMoins ( remettre_moins_div exp1, 
			remettre_moins_div exp2)
	| LDiv2 (exp1,exp2) -> 
		LDiv2 ( remettre_moins_div exp1, 
			remettre_moins_div exp2)
;;

(* simplifie jusqu'a arrive a un point fixe qui est alors l'expression simplifie *)
let rec simple_iter exp = match simple exp with
	(exp,simplifie) -> 
			(*print_string "simple_iter";
			print_string (string_of_bool simplifie);
			flush stdout;*)
			if simplifie then simple_iter exp
			else exp
;;

(* simplifie, remet les div et moins, puis resimplifi le resultat *)
let simplifie_completement exp =
	simple_iter (remettre_moins_div (simple_iter exp))
;;

(* elle simplifie un arbre de type arbre en le convertissant en arbre_liste,
puis fait des simplifications, puis convertie le resultat en arbre et le retourne.
C'est la seule fonction qui est exportee *)
let simplifie_init exp = 
	arbre_of_arbre_liste ( simplifie_completement (arbre_liste_of_arbre exp ))
;;
