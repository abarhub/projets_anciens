#ifndef _DEBUGAGE_H_
#define _DEBUGAGE_H_

// fichier pour faciliter le debugage

// il faut mettre la ligne qui suit en commentaire pour desactiver tous les tests
#define _DEB_

#ifdef _DEB_

// stop l'execution du programme si X n'est pa verifier, et execute Y
#define STOP(X,Y) if(!(X)){printf("Erreur ligne %d du fichier %s:stop(%s)\n",\
				  __LINE__,__FILE__,#X);Y;exit(2);}
// retourne 1 ssi Y<=X<=Z
#define INTER(X,Y,Z) (((X)>=(Y))&&((X)<=(Z)))

#else

#define STOP(X,Y)
#define INTER(X,Y,Z)

#endif

#endif
