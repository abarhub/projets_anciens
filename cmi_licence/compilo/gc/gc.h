//#define _GC_H_
#ifndef _GC_H_
#define _GC_H_

// si on defini _NO_GC_ on a pas le GC

#ifdef _NO_GC_
// le gc ne sera pas actif

#include<stdlib.h>
#include<string.h>

#define GC_INIT(X)
#define gc_malloc(X) malloc(X)
#define gc_free(X) free(X)
#define gc_realloc(X,Y) realloc(X,Y)
#define gc_calloc(X),Y calloc(X,Y)
#define gc() 
#define gc_ajoute_global(X)
#define gc_strdup(X) strdup(X)

#else
// le gc sera actif

//#error "c'est une erreur"

#include "gcbase.h"

/* fonction a appele au debut du programme
le parametre est la 1ere variable locale */
#define GC_INIT(X) gc_init(&(X))

/* cree et retourne un bloc de memoire de taille X */
#define malloc(X) gc_malloc(X)
#define calloc(X,Y) gc_malloc((X)*(Y))
#define realloc(X,Y) gc_realloc((X),(Y))

#define strdup(X) gc_strdup(X)

/* efface un bloc pointe par X */
#define free(X) {X=NULL;gc();}

#endif

#endif