(* tp 4*)
(* carre des elem de liste*)
let rec l_carre = fun [] -> []
                    |  (a::l) -> (a*a)::(l_carre l)
                ;;

l_carre [2;4;3;8] ;;

let l_carre2 = map (fun x->x*x);;
let l_carre2f = map (fun x->x*.x);;

(* produit liste *)
let produitliste l1 l2 = 
        if ( (list_length l1)==(list_length l2) )
                then map2 (fun x y ->x*.y) l1 l2
                else failwith "l1 et l2 de taille differente !!!"  
        ;;
let rec mymap2 f l1 l2 = match (l1,l2) with
                | ([], []) -> [] 
                | ([], l) -> failwith "erreur"
                | (l, []) -> failwith "erreur"
                | (a::l1, b::l2) -> (f a b)::(mymap2 f l1 l2)
                ;;

(* norme de 2 vecteur *)
let  norme l = sqrt ( it_list (prefix +.) 0. ( l_carre2f l)) ;;

(* transposer une matrice *)
let rec transpose2 m =
        let toto l1 l2 l3 = match l3 with
                | [] -> (l1,l2,[])
                | (a::l) -> (l1@[a],l2,toto l1 l2 l3)
        and init = function
                | [] ->[[]]
                | (a::l) -> [a]::[init l]
        and traite l_init liste = match (l_init,liste) with
                | ([],[]) -> [[]]
                | ([],a::l) -> failwith ""
                | (l,[]) -> failwith "erreur"
                | (a::s1,b::s2) -> (a@b) :: (traite s1 s2)
        in 
        | [] -> []
        | (a::l) -> it_list traite (init a l) l
                        map2 
                        match a with
                        | [] -> []
                        | (b::l2) -> 

let rec comp f x i= if i=0 then x
                        else f( comp f x (i-1) )
                        ;;


let trans m i k = hd 
        (comp tl (
                hd  (comp tl m (k-1))
                ) 
                (i-1)) ;; 

let transpose m =
        let rec pos m i j=
                comp (comp) 