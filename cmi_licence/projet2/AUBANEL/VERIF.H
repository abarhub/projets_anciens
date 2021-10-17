#ifndef VERIF_H
#define VERIF_H

#include "type.h"

/* ce fichier exporte les fonctions de "verif.c" */

int verification(ST *program);
enum VF table_symbol_vide(void);
void efface_table_symbol(void);

#endif
