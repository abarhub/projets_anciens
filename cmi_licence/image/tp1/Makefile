courbe: courbe.o outils2d.o
	cc -o courbe courbe.o outils2d.o -lgx -lX11 -lm

courbe.o: courbe.c 2d.h
	cc -c courbe.c

outils2d.o: outils2d.c 2d.h
	cc -c outils2d.c