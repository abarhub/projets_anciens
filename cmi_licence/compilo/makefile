# makefile du projet de compilation
# les ifdef ne sont pas compatibles entre le programme make de borland et celui de gcc

ifdef NO_GC
NO_GC1 = -D_NO_GC_
else
NO_GC1 =
endif

#variables pour gcc (unix, djgpp, ...)
OPT = -Wall -ggdb3 $(NO_GC1)
CC = gcc

OBJ = .o
LIB = 
DEL = rm
OUTPUT = parse
EXEC = -o $(OUTPUT)
# macro pour tous les compilateurs
OBJET = $(SRC:.c=$(OBJ))

ifdef NO_GC
SRC = main.c  parse.tab.c lex.yy.c arbast.c verif.c erreur.c affich_rec.c 
else
SRC = main.c parse.tab.c lex.yy.c arbast.c verif.c erreur.c affich_rec.c gc.c
endif

$(OUTPUT): $(OBJET)
	$(CC) $(OPT) $(EXEC) $(OBJET) $(LIB)

parse.tab$(OBJ): parse.y
	bison -d parse.y
	$(CC) $(OPT) -c parse.tab.c

lex.yy$(OBJ): lexer.lex parse.tab$(OBJ)
	flex lexer.lex
	$(CC) $(OPT) -c lex.yy.c

arbast$(OBJ): arbast.c
	$(CC) $(OPT) -c arbast.c

verif$(OBJ): verif.c
	$(CC) $(OPT) -c verif.c

erreur$(OBJ): erreur.c
	$(CC) $(OPT) -c erreur.c

affich_rec$(OBJ): affich_rec.c
	$(CC) $(OPT) -c affich_rec.c

gc$(OBJ): gc.c gc.h
	$(CC) $(OPT) -c gc.c

main$(OBJ): main.c
	$(CC) $(OPT) -c main.c

# sous capucine il faut utiliser cette regle
all2:
	bison -d parse.y
	flex lexer.lex
	gcc  -o parse lexyy.c parse.tab.c arbast.c verif.c 

test:
	parse <test1.kiss

testall:
	parse *.kiss

test2:
	parse *.kiss | grep "correct|e fich"

clean:
	$(DEL) *$(OBJ)
	$(DEL) parse.tab.*
	$(DEL) lex.yy.* 

touch:
	touch makefile $(OUTPUT)

emacs:
	emacs type.h parse.y verif.c arbast.c lexer.lex verif.h arbast.h &
