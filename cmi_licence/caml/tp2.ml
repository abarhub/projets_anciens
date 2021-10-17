(*  tp2  *)

(* 1.1 *)
let div (x,y) = let d=ref 0 and tmp=ref x in
while !tmp>=y do tmp:=(!tmp)-y;d:=(!d)+1 done; !d;;

let div (x,y) = 
	let rec div_tmp x y nb_soustr = 
		if x<y then nb_soustr 
		else div_tmp (x-y) y (nb_soustr+1) in
			div_tmp x y 0;;
	

let div4 (x) = div(x,4);;

let div37 (x) = div(37,x);;


(* 1.2 *)

let divc x y = div (x,y);;

let divc4 (x) = divc x 4;;

let divc37 (x) = divc 37 x;;


(* 1.3 *)

let divc x y = 
	let rec div_tmp x y nb_soustr = 
		if x<y*nb_soustr then nb_soustr 
		else div_tmp x y (nb_soustr+1) in
			(div_tmp x y 0)-1 ;;

(* 1.4 *)

let rec pgcd x y = 
	if x==0 then y
	else 
		begin 
			if x<=y then pgcd x (y-x)
			else pgcd y x 
		end;;

let rec pgcd_euclide x y = 
	if x==0 then y
	else 
		begin 
			if x<=y then pgcd x (y mod x)
			else pgcd y x 
		end;;

(* 2.1 *)

let rec fact = fun 0 -> 1
		| 1-> 1;
		|n -> n*fact (n-1);;

let rec fact = fun n when n=0 or n=1 -> 1;
		|n -> n*fact (n-1);;
		
(* 2.2 *)

let rec fact n = match n with 
		0|1 -> 1;
		|p -> p*fact (p-1);;

(* 2.3 *)

let rec mult x y = 
	match y with
	0 -> 0
	| _ -> (mult x (y-1))+x;;
	
let rec mult x  = 
	fun 0 -> 0
	| n -> (mult x (n-1))+x;;
		