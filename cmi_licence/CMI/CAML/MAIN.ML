#open "excep";;
#open "arbre";;
#open "deriv";;
#open "affiche";;
#open "simpl";;

(* Module principal *)

exception Fichier_non_ouvert of string;;

(* annalyse a partir du fichier standard d'entree *)
let parse_stdin () =
	let lexbuf =lexing__create_lexer_channel std_in 
		(* prepare le lexeur pour lecture sur l'entree standard *)
	in
		try
			let result=parser__Main lexer__Token lexbuf
				(* parse le fichier *)
			in
				result
		with (*Eof -> Liste_expression([]) *)
		| parsing__Parse_error -> raise Erreur_syntaxique
;;

(* annalyse a partir du fichier nom *)
let parse_fichier nom =
	try 
		let lexbuf = lexing__create_lexer_channel 
			(open_in nom)
		(* prepare le lexeur pour lecture du fichier nom *)
		in
			(* try *)
				let result=parser__Main lexer__Token lexbuf
					(* parse le fichier *)
				in
					print_string "salut";flush std_out;
					result
			(*with (* Eof -> Liste_expression([]) (* fin du fichier *) *)
			| parsing__Parse_error -> raise Erreur_syntaxique *)
	with sys__Sys_error s -> (* impossible d'ouvrir le fichier *)
			raise (Fichier_non_ouvert(nom))
		| parsing__Parse_error -> (* erreur de syntaxe *)
			raise Erreur_syntaxique
;;

let expression_of_arbre = function
	| Liste_expression [] -> failwith "va te faire"
	| Liste_expression (a::q) -> a
;;

let exp_of_arbre = function
	(*| Vide -> failwith "va te faire"*)
	| Arbre (exp,var) -> exp
;;

(* traite la liste des expressions a deriver *)
let annalyse_liste_exp = function
	Liste_expression (l) -> 
		let rec annalyse = function
			| [] -> ()
			| [a] -> affiche (simplifie_init (
					(*derivation*) (exp_of_arbre a)));
			| (a::q) -> affiche (simplifie_init (
					(*derivation*) (exp_of_arbre a)));
				annalyse q;
		in
			annalyse l
;;

let main ()=
	print_string "hello\n";flush std_out;
	try
		let arbre_syntaxique =
			if sys__interactive then
				parse_fichier "test1.txt"
			else if (vect_length sys__command_line) == 2 then
				parse_fichier (vect_item sys__command_line 1)
			else
				parse_stdin ()
		in 
		begin
		annalyse_liste_exp arbre_syntaxique;
		(*match expression_of_arbre arbre_syntaxique with
		| Vide -> print_string "arbre vide\n";flush std_out
		| Arbre(exp,var) -> print_string "arbre non vide***\n";
				flush std_out;
			affiche (simplifie_init 
			( (* derivation *)
				(* expression_of_arbre(arbre_syntaxique) *)
				exp));*)
		end;
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
		| Erreur_syntaxique -> 
			print_string "Erreur syntaxique";
			flush std_out;
;;

print_string "exec de main\n";;
main();;
