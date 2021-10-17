#open "excep";;
#open "arbre";;
#open "deriv";;
#open "affiche";;
#open "simpl";;

(* Module principal *)

exception Fichier_non_ouvert of string;;

(* let expr = function Arbre(e,_) ->e;; *)


(* annalyse a partir du fichier standard d'entree *)
let parse_stdin () =
	let lexbuf =
		lexing__create_lexer_channel std_in 
	in
		try
			let result=parser__Main lexer__Token lexbuf
			in
				hd result
		with Eof -> Vide
				
;;

(* annalyse a partir du fichier nom *)
let parse_fichier nom =
	try 
		let lexbuf = lexing__create_lexer_channel 
			(open_in nom)
		in
			try
				let result=parser__Main lexer__Token lexbuf
				in
					print_string "salut";flush std_out;
					hd result
			with Eof -> Vide (* fin du fichier *)
	with sys__Sys_error s -> 
		raise (Fichier_non_ouvert(nom))
;;

let expression_of_arbre = function
	| Vide -> failwith "va te faire"
	| Arbre (exp,var) -> exp
;;

let main ()=
	print_string "hello";flush std_out;
	try
		let arbre_syntaxique =
			if not sys__interactive && 
				(vect_length sys__command_line) == 2 then
				parse_fichier
				(vect_item sys__command_line 1)
			else
				parse_stdin ()
		in 
			if arbre_syntaxique=Vide then 
				print_string "arbre non	vide"
				else print_string "arbre vide";
			affiche (simplifie_init 
			( (* derivation *)
				expression_of_arbre(arbre_syntaxique)));
		(* interactive *)
		print_string "coucou\n";
		flush std_out;
		(* affiche (parse_fichier "test1.txt") *)
	with
		Failure str -> 
			print_string ("Exception dans le main:"^str)
		| Fichier_non_ouvert nom 
			-> print_string ("Erreur:impossible d'ouvrir le"^
					" fichier "^nom)
		| Division_par_zero -> 
			print_string "division par zero";
			flush std_out;
		| LPlus_vide -> 
			print_string "LPlus_vide";
			flush std_out;
		| LFois_vide -> 
			print_string "LFois_vide";
			flush std_out;
;;

print_string "exec de main\n";;
main();;
