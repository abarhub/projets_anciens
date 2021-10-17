#ifndef _ERREUR_H_
#define _ERREUR_H_

/* ce fichier exporte les fonctions d'erreurs de "erreur.c" */

void affiche_erreur(char *s, ...);
void affiche_erreur_ligne(int no,char *s, ...);

extern int nb_erreurs;
extern int pause_touch;

#endif