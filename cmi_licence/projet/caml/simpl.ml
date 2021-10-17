#open "arbre";;
#open "excep";;

(* fonctions de simplification *)

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
;;

let rec ajoute_somme e1 e2 = match (e1,e2) with
	(LPlus exp1,LPlus exp2) -> LPlus (exp1 @ exp2)
	| (LPlus exp1, exp2) -> LPlus ( exp1 @ [exp2] )
	|(exp1,LPlus exp2) -> LPlus ( [exp1] @ exp2 )
	| (exp1,exp2) -> LPlus ( exp1::[exp2] )
;;

let rec ajoute_produit e1 e2 = match (e1,e2) with
	(LFois exp1,LFois exp2) -> LFois (exp1 @ exp2)
	| (LFois exp1, exp2) -> LFois ( exp1 @ [exp2] )
	|(exp1,LFois exp2) -> LFois ( [exp1] @ exp2 )
	| (exp1,exp2) -> LFois ( exp1::[exp2] )
;;	

let rec arbre_liste_of_arbre = function
	| Plus(e1,e2) -> ajoute_somme (arbre_liste_of_arbre e1) 
		(arbre_liste_of_arbre e2)
	| Moins (e1,e2) -> ajoute_somme (arbre_liste_of_arbre e1) 
		(arbre_liste_of_arbre (Moins_u e2))
	| Flotant f -> LFlottant f
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
	| Moins_u (Flotant f) -> LFlottant (-. f)
	| Moins_u e -> LMoins_u (arbre_liste_of_arbre e)
;;
	
let rec arbre_of_arbre_liste = function
	| LPlus [a] -> arbre_of_arbre_liste (a)
	| LPlus [] -> raise LPlus_vide
	| LPlus (a::q) -> 
		Plus (arbre_of_arbre_liste a,
			arbre_of_arbre_liste (LPlus q))
	| LFois [a] -> arbre_of_arbre_liste (a)
	| LFois [] -> raise LFois_vide
	| LFois (a::q) -> 
		Fois (arbre_of_arbre_liste a,
			arbre_of_arbre_liste (LFois q))
	| LDiv d -> Div (Flotant 1.,arbre_of_arbre_liste d)
	| LVar v -> Var v
	| LFlottant f -> Flotant f
	| LMoins_u e -> Moins_u (arbre_of_arbre_liste e)
	| LSin e -> Sin (arbre_of_arbre_liste e)
	| LCos e -> Cos (arbre_of_arbre_liste e)
	| LLog e -> Log (arbre_of_arbre_liste e)
	| LAbs e -> Abs (arbre_of_arbre_liste e)
;;

let rec simplifie_plus l =
	let rec simple_plus l val = match l with
		| [] -> ([],val)
		| ((LFlottant f)::q) -> simple_plus q (val+.f) 
		| (a::q) -> let res=simple_plus q val in
			((simple a)::(fst res),snd res)
	in let res=simple_plus l 0. in
		if((fst res)=[]) then [LFlottant (snd res)]
			else if((snd res)=0.) then fst res
			else (LFlottant (snd res))::(fst res)
and simplifie_fois l =
	let rec simple_fois l val = match l with
		| [] -> ([], val)
		| ((LFlottant f)::q) -> simple_fois q (val*.f)
		| ((LDiv (LFlottant 0.))::q) -> raise Division_par_zero 
		| ((LDiv (LFlottant f))::q) -> simple_fois q (val /. f) 
		| (a::q) -> let res=simple_fois q val in
			((simple a)::(fst res),snd res)
	in let res=simple_fois l 1. in
	if((fst res)=[]) then [LFlottant (snd res)]
			else if((snd res)=1.) then fst res
			else (LFlottant (snd res))::(fst res)
	(* simplifie_fois l =
	let rec simple_fois l val = match l with
		| [] -> if(val=1.) then [LFlottant 1.] 
			else [LFlottant val]
		| ((LFlottant f)::q) -> simple_fois q (val*.f)
		| ((LDiv (LFlottant 0.))::q) -> raise Division_par_zero 
		| ((LDiv (LFlottant f))::q) -> simple_fois q (val /. f) 
		| (a::q) -> ((simple a)::(simple_fois q val))
	in simple_fois l 1.  *)
and simple = function
	| LPlus l -> LPlus (simplifie_plus l)
	| LFois l -> LFois (simplifie_fois l)
	| LDiv exp -> LDiv (simple exp)
	| LMoins_u exp -> LDiv (simple exp)
	| LSin exp -> LSin (simple exp)
	| LAbs exp -> LAbs (simple exp)
	| LCos exp -> LCos (simple exp)
	| LLog exp -> LLog (simple exp)
	| exp -> exp
;;

let rec regroupe_plus exp val = match (exp,val) with
        (Plus (Flotant f1,Var v),val) -> (Var v,val+.f1)
	| (Plus (Var v,Flotant f1),val) -> (Var v,(val+.f1))
	| (Moins (Var v,Flotant f1),val) -> (Var v, (val-.f1))
	| (Moins (Flotant f1,Var v),val) -> 
		(Moins_u (Var v),(val+.f1))
	
	| (Plus (Flotant f1,e),val) -> regroupe_plus e (val+.f1)
	| (Plus (e,Flotant f1),val) -> regroupe_plus e (val+.f1)
	| (Moins (e,Flotant f1),val) -> regroupe_plus e (val-.f1)
	| (Moins (Flotant f1,e),val) -> regroupe_plus (Moins_u e) (val+.f1)
	| ((Fois (e1,e2)) as e,val) -> ( e , val)
	| ((Div (e1,e2)) as e,val) -> ( e , val)
	| (Plus (e1,e2),val) -> let t1=regroupe_plus e1 val and
		t2=regroupe_plus e2 0. in
		(Plus (fst t1,fst t2), (snd t1)+.(snd t2))
	| (Moins (e1,e2),val) -> let t1=regroupe_plus e1 val and
		t2=regroupe_plus e2 0. in
		(Moins (fst t1,fst t2), (snd t1)-.(snd t2))
	| (Sin (e),val) -> (Sin e,val)
	| (Cos (e),val) -> (Cos e,val)
	| (Log (e),val) -> (Log e,val)
	| (Abs (e),val) -> (Abs e,val)
	| (Moins_u (e),val) -> let t= regroupe_plus e 0. in
		(Moins_u (fst t),val-.(snd t))
	| (Plus_u (e),val) -> let t= regroupe_plus e 0. in
		(Plus_u (fst t),val+.(snd t))
	| (Flotant f,_) -> raise (Regroupe_plus f)
	| (Var v,val) -> (Var v, val)
;;

let regroupe_plus2 exp =
	let tmp=regroupe_plus exp 0. in
	match snd tmp with
	0. -> fst tmp
	| val -> Plus ( fst tmp, Flotant (val))
;;

let rec simplifie = function 
	| Plus (Flotant 0. ,e) -> 
		(fst (simplifie (e)),true)
	| Plus (e,Flotant 0.) -> 
		(fst (simplifie (e)),true)
	| Plus (Plus(e,Flotant f1),Flotant f2) -> 
		(regroupe_plus2 
			(Plus(Flotant(f1+.f2),fst (simplifie (e)))),true)
	| Moins (e,Flotant 0.) -> 
		(fst (simplifie (e)),true)
	| Moins (Flotant 0.,e) -> (Moins_u(fst (simplifie (e))),true)
	| Fois (e,Flotant 0.) -> (Flotant(0.),true)
	| Fois (Flotant 0.,e) -> (Flotant(0.),true)
	| Fois (Flotant 1.,e) -> (fst (simplifie e),true)
	| Fois (e,Flotant 1.) -> (fst (simplifie e),true)
	| Fois (Flotant f1,Flotant f2) -> (Flotant (f1*.f2),true)
	| Div (Flotant 0.,e) -> (Flotant 0.,true)
	| Div (e,Flotant 1.) -> (fst (simplifie e),true)
	| Plus (e1,Moins_u e2) -> 
		let exp1=simplifie e1 and exp2=simplifie e2 in
		(Moins (fst exp1,fst exp2),true)
	| Plus (Moins_u e2,e1) -> 
		let exp1=simplifie e1 and exp2=simplifie e2 in
		(Moins (fst exp1,fst exp2),true)
	| Moins_u (Flotant f) -> (Flotant (-. f),true)
	| Moins_u (Moins_u e) -> 
		let exp=simplifie e in
		(fst exp,true)
	(* regles qui ne simplifie pas *)
	| Plus (e1,e2) -> let exp1=simplifie e1 and exp2=simplifie e2 in
		(Plus(fst exp1,fst exp2),(snd exp1) or (snd exp2))
	| Moins (e1,e2) -> let exp1=simplifie e1 and exp2=simplifie e2 in
		(Moins(fst exp1,fst exp2),(snd exp1) or (snd exp2))
	| Fois (e1,e2) -> let exp1=simplifie e1 and exp2=simplifie e2 in
		(Fois(fst exp1,fst exp2),(snd exp1) or (snd exp2))
	| Div (e1,e2) -> let exp1=simplifie e1 and exp2=simplifie e2 in
		(Div(fst exp1,fst exp2),(snd exp1) or (snd exp2))
	| Plus_u e -> let exp=simplifie e in
		(fst exp,snd exp)
	| Moins_u e -> let exp=simplifie e in
		(Moins_u (fst exp),snd exp)
	| Sin e -> let exp=simplifie e in
		(Sin(fst exp),snd exp)
	| Cos e -> let exp=simplifie e in
		(Cos(fst exp),snd exp)
	| Log e -> let exp=simplifie e in
		(Log(fst exp),snd exp)
	| Abs e -> let exp=simplifie e in
		(Abs(fst exp),snd exp)
	| e -> (e,false);;

let rec iter_simplifie exp n =
	let e=simplifie exp in
		if n>0 & snd e=true then iter_simplifie (fst e) (n-1)
		else fst e
;;

(* let simplifie_init exp =simplifie (simplifie (simplifie exp));; *)

let simplifie_init exp = 
	arbre_of_arbre_liste ( simple (arbre_liste_of_arbre exp ))
	(*  regroupe_plus2 (iter_simplifie exp 20)  *)
;;
