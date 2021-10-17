//#define _GC_H_
#ifndef _GC_H_
#define _GC_H_

#include "gcbase.h"

/* fonction a appele au debut du programme
le parametre est la 1ere variable locale */
#define GC_INIT(X) gc_init(&(X))

/* cree et retourne un bloc de memoire de taille X */
#define malloc(X) gc_malloc(X)
#define calloc(X,Y) gc_malloc((X)*(Y))
#define realloc(X,Y) gc_realloc((X),(Y))

#ifdef alloca
#undef alloca
#endif

#define alloca(X) gc_malloc(X)

#define strdup(X) gc_strdup(X)

/* efface un bloc pointe par X */
#define free(X) {X=NULL;gc();}

#endif