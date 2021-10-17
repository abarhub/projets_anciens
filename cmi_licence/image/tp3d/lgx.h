/******************** declarations de libg.c **********************/
/*                    Auteur : Pascal Bernard                     */
/******************************************************************/

/* Dimensions de la fenetre */

extern int hauteur(),largeur();

/* Les ton de gris ... */

extern int nb_gris();    /* ... leur nombre */
#define BLANC 0          /* du plus claire (le blanc) */
#define NOIR (nb_gris()-1) /* en allant vers le plus fonce (le noir) */

/* Les fonction des plot */

extern void openpl(), erase(), line(),
           move(), cont(), point(), closepl();

/* Les fonctions ajoutees a plot */

void color(), polygon(), flushpl();
