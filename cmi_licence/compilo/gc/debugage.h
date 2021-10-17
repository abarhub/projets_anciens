#ifndef _DEBUGAGE_H_
#define _DEBUGAGE_H_

/*
ce fichier sert a detecter les erreurs logiques
et a les expliciter
*/

#include<stdlib.h>

// si on enleve ce commentaire il n'y a plus de tests
//#define _NON_DEBUGAGE_

#ifdef _NON_DEBUGAGE_
// ne pas prendre en compte le debugage

/* verifie que la condition X est verifier
 sinon execute l'instruction Y*/
#define CHECK(X,Y)

/* verifie que la condition X est verifier
 sinon affiche le message Y */
#define CHECK_MSG(X,Y)

/* affiche X  */
#define PRINT(X)

/* arrete le programme en affichant X */
#define STOP(X)

/* execute X */
#define HELP(X)

#else
// prendre en compte le debugage

#ifdef __BORLANDC__
/* specificite de borland */
#define CHECK(X,Y) if(!(X)) { printf("Erreur CHECK dans le fichier %s dans la fonction %s a la ligne %d\n",\
				__FILE__,__FUNC__,__LINE__);\
				printf("La condition :" #X " n'est pas verifier\n"); Y; exit(1); }

#else
/* compilateur c normal */
#define CHECK(X,Y) if(!(X)) { printf("Erreur CHECK dans le fichier %s a la ligne %d\n", \
				__FILE__,__LINE__); \
				printf("La condition :" #X " n'est pas verifier\n");  Y; exit(1); }

#endif

#define CHECK_MSG(X,Y) CHECK(X,printf Y ;printf("\n");)
#define PRINT(X) { printf X ; }
#define STOP(X) { printf("Erreur STOP dans le fichier %s a la ligne %d\n", \
				__FILE__,__LINE__); printf X ;printf("\n"); exit(1); }
#define HELP(X) { X ; }

#endif


#endif