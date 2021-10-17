type expression = 
	Flotant of float
	| Var of string
	| Plus of expression*expression
	| Fois of expression*expression
	| Div of expression*expression
	| Moins of expression*expression
	| Moins_u of expression
	| Plus_u of expression
	| Sin of expression
	| Log of expression
	| Abs of expression
	| Cos of expression	
;;

type arbre = Arbre of expression*string
	| Vide;;	