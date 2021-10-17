(* tp3 *)

(* 1.1 *)

let rec last =fun [x] ->x 
	| (l::suite) -> last suite 
	| [] -> failwith "erreur" ;;

last [2;4;5;8];;

(* 1.2 *)

let rec nieme liste no = 
	if no<0 then failwith "Erreur: no est negatif"
	else match liste with
		[] -> failwith "Erreur"
		| [x] -> if no=0 then x 
			else failwith ("Erreur: le "^" n'existe pas")
		| x::suite -> if no=0 then x
			else nieme suite (no-1);;

nieme [2;6;5;9;1] 3;;

(* 1.3 *)

let loc liste valeur = 
	let rec position liste val no = 
		match liste with
		[] -> failwith "il n'est pas dans la liste"
		| [x] -> if x=val then no 
			else failwith "il n'est pas dans la liste"
		| x::suite -> if x=val then no 
			else position suite val (no+1)
		in
			position liste valeur 1;;

loc [4;6;7;8] 7;;

(* 1.4 *)

let rec append l1 l2 =
	match (l1,l2) with
	([],l) -> l
	| (x::suite,l) -> x:: append suite l2;;

append [3;5;8] [2;4];;

(* 1.5 *)

let rec renv = fun [] -> []
	| [x] -> [x]
	| (x::suite) -> append (renv suite) [x] ;;

renv [1;2;3];;

(* 2.1 *)

let rec appartient s =
	fun [] -> false
	| (x::suite) -> (x=s) or appartient s suite;;

appartient 6 [1;2;3;4];;
appartient 6 [1;2;3;4;5;6;7];;

(* 2.2 *)

let rec est_un =
	fun [] -> true
	| (x::suite) -> (not (appartient x suite))
			& est_un suite;;

est_un [1;2;3;4;5];;
est_un [1;2;3;7;3;7];;

(* 2.3 *)

let rec creer =
	fun [] -> []
	| (x::suite) -> if appartient x suite then creer suite
			else x::(creer suite);;

creer [1;2;3;4;5];;
creer [1;2;3;7;3;7];;

(* 2.4 *)

let rec uni l1 l2 =
	match (l1,l2) with
	([],l2) -> l2
	| (x::suite,l2) -> if appartient x l2 then uni suite l2
				else x::(uni suite l2);;
uni [1;2;3;4] [4;5;6;7];;
uni [1;6;4;8] [];;
uni [1] [1];;

let rec inter l1 l2 = 
	match (l1,l2) with
	([],l2) ->[]
	| (x::suite,l2) ->if appartient x l2 then x::(inter suite l2)
				else inter suite l2;;

inter [1;2;3;4;5] [3;5;6;4];;
inter [6;3;4] [];;
inter [1;2] [2;1];;

(* 2.5 *)

let rec inclus l1 l2 =
	match (l1,l2) with
	([],l) -> true
	| (x::suite,l) ->(appartient x l) & inclus suite l;;

inclus [1;2;3] [0;3;1;2;4;6];;
inclus [] [6;4;3];;
inclus ["a";"b"] ["a";"c"];;

let egal l1 l2 = inclus l1 l2 & inclus l2 l1;;

egal [1;2;3] [5];;
egal ["a";"d";"z";"c"] ["z";"d";"a";"c"];;
egal [[];[[]]] [[[]];[]];; (* ensemble d'ensemble *)

(* 3.1 *)

let rec liste_entiers =
	fun 0 -> []
	| n -> (liste_entiers (n-1))@[n];;

liste_entiers 3;;
liste_entiers 90;;

(* 3.2 *)

let rec produitliste l1 l2 =
	match (l1,l2) with
	([],[]) -> []
	|([],l1) ->failwith "les 2 liste n'ont pas la meme dimension"
	| (l2,[]) -> failwith "les 2 liste n'ont pas la meme dimension"
	| (x::suite,y::suite2) -> 
		(x*.y)::(produitliste suite suite2);;

produitliste [1.;2.5] [3.2;2.];;
(* produitliste [2.;3.;5.] [4.;6.];; *)

(* 3.3 *)

let prodscal v1 v2 = 
	let rec somme =
		fun [] -> 0.
		| (x::suite) -> x +. (somme suite)
		in
			somme (produitliste v1 v2) ;;

prodscal [1.;2.;3.] [2.;3.;4.];;
prodscal [1.;1.;1.;1.;1.] [2.;2.;2.;2.;2.];;

