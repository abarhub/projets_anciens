#makefiel du projet d'image

LIB = -L/usr/X11R6/lib -lX11 -lm
#OPT = -Wall
OPT = 

cube: cube.o outils2d.o outils_zbuff.o outils3d.o lgx.o peintre.o zbuffer.o 
	gcc $(OPT) -o cube cube.o outils2d.o outils3d.o lgx.o peintre.o zbuffer.o outils_zbuff.o $(LIB)

cube.o: cube.c 3d.h
	gcc $(OPT) -c cube.c

outils2d.o: outils2d.c 2d.h
	gcc $(OPT) -c outils2d.c

outils3d.o: outils3d.c 3d.h
	gcc $(OPT) -c outils3d.c

lgx.o: lgx.c
	gcc $(OPT) -c lgx.c

peintre.o: peintre.c
	gcc $(OPT) -c peintre.c

zbuffer.o: zbuffer.c 3d.h
	gcc $(OPT) -c zbuffer.c

outils_zbuff.o: outils_zbuff.c
	gcc $(OPT) -c outils_zbuff.c

clean:
	\rm -f *.o  cube

distrib:
	tar c *.c *.h *.txt avion canard crane makefile voiture | gzip -c >projet.tgz

help:
	@echo aide pour compiler le projet:
	@echo si vous tapez:
	@echo "- make : le projet se compile et apres, on peut l'executer par ./cube"
	@echo "- make clean : les fichiers objets et l'executable sont effaces"
	@echo "- make distrib : cree un fichier compresse nomme projet.tgz qui contient tous les sources, le rapport, ce makefile, et les exemples"