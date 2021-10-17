#ifndef _GC_BASE_H_
#define _GC_BASE_H_

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

void gc(void);
void gc_affiche(void);
void *gc_malloc(int lg);
void gc_init(int *p);
void *gc_cherche(void *pt);
int gc_bloc_valide(void *p);
void *gc_realloc(void *p,int i);
char *gc_strdup(char *p);
void gc_ajoute_global(void **p);
int nb_elt(void);

#endif

#endif