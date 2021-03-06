#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"arbast.h"
#include"erreur.h"
#include"gc.h"

//#ifndef _NO_GC_
//#error "il faut no gc"
//#endif

int annalyse(char *nom_fichier);

void Syntaxe(char *nom)
{
  printf("Syntaxe: %s [-h] [-err] [noms_de_fichiers]\n",nom);
  printf("-h : affiche la syntaxe des parametres\n");
  printf("-err : fait une pause apres chaque erreur detectee\n");
}

// retourne 1 ssi le fichier existe
int fichier_existe(char *nom)
{
  FILE *f;
  
  if(nom==NULL)
    return 0;
  
  f=fopen(nom,"r");
	
  if(f==NULL)
    return 0;
	
  if(fclose(f))
    return 0;
  
  return 1;
}

int main(int argc, char *argv[])
{
  int i,res;
  
  //initialisation()
  GC_INIT(i);
  gc_ajoute_global((void**)&programme);
  gc_ajoute_global((void**)&liste);
  gc_ajoute_global((void**)&liste_dernier);
  gc_ajoute_global((void**)&dernier_marque);
  /*gc_ajoute_global((void**)&liste_dernier);
    gc_ajoute_global((void**)&liste_dernier);*/
  
  if(argc>1)
    {// annalyse des parametres et traitements
      if(strcmp(argv[1],"-h")==0)
	{
	  Syntaxe(argv[0]);
	  return 0;
	}
      if(strcmp(argv[1],"-err")==0)
	{
	  pause_touch=1;
	}
      for(i=1;i<argc;i++)
	{
	  if(fichier_existe(argv[i]))
	    {
	      printf("Annalyse du fichier %s\n",argv[i]);
	      res=annalyse(argv[i]);
	      switch(res)
		{
		case 0:
                  printf("Le fichier %s a une grammaire correcte\n",
			 argv[i]);
                  affiche(programme);
                  
                  break;
		case -1:
                  printf("Erreur: Impossible d'ouvrire le fichier %s\n",
			 argv[i]);
		default:
               case -2:
		 printf("Le fichier %s a eut %d erreurs\n",
                        argv[i],nb_erreurs);
		 break;
		}
	      
	      if(res!=-2)
             	efface_ls();
	    }
	  else
	    {
	      printf("Erreur: Le fichier %s n'existe pas\n",argv[i]);
	    }
        }
      
    }
  else
    {// annalyse a partir du fichier standart d'entree
      switch(annalyse(NULL))
	{
	case 0:
	  printf("Grammaire correcte\n");
	  affiche(programme);
	  efface_ls();
	  return 0;
	case -2:
	default:
	  printf("Il y a eut %d erreurs\n",nb_erreurs);
	  if(res!=-2)
	    efface_ls();
	  return 1;
	}
    }
  
  return 0;
}
