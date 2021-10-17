#ifndef _GC_BASE_H_
#define _GC_BASE_H_

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