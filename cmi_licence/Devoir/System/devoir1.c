#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<unistd.h>
#include<malloc.h>

/*
BARRET Alain
SCHUDEL Edward

exercice 1:
le programme lit les 'define' du fichier donne en parametre
et cree un fichier en /tmp avec ifdef en moins
*/

#define MAX (200)
#define MAX_NOM (30)
#define MAX_CONTENU (500)
#define MAX_SYMBOL (1000)

/* retourne vrai si la line est une ligne pour le preprocesseur */
#define IS_LINE_FOR_PREPROCESS(X) (X[0]=='#')

struct Symbol
{
 char *nom;
 char *valeur;
} table_symbol[MAX_SYMBOL]={{NULL,NULL}};

/* =1 si la fonction recherche a trouve la macro, =0 sinon */
int symbol_trouve=0;

char *is_command_for_preprocess(char line[],char command[])
/*
determine si la chaine line correspond a la commande du preprocesseur command
si c'est le cas, retourne le reste de la line en enlevant les 1er espaces
sinon retourne NULL
*/
{
 char *p;

 if(IS_LINE_FOR_PREPROCESS(line))
   {
    if(strncmp(&line[1],command,strlen(command))!=0)
      return NULL;

    p=line+1+strlen(command);
    while((*p==' '))
      p++;

    return p;
   }
 return NULL;
}

void init_table_symbol(void)
/*
remplit la table des symbols a 0
*/
{
 int i;

 for(i=0;i<MAX_SYMBOL;i++)
    {
     table_symbol[i].nom=NULL;
     table_symbol[i].valeur=NULL;
    }
}

struct Symbol *place_libre(void)
/*
determine le premier emplacement de libre dans le tableau
retourne l'adresse de l'emplacement ou NULL si non trouve
*/
{
 int i;

 for(i=0;i<MAX_SYMBOL;i++)
    {
     if(table_symbol[i].nom==NULL)
       return &(table_symbol[i]);

    }

 return NULL;
}

void affiche_symbol(void)
/*
affiche les symbols:
position_dans_le_tableau : nom_de_macro = valeur_de_la_macro
*/
{
 int i;
 struct Symbol *s1;

 s1=table_symbol;
 for(i=0;i<MAX_SYMBOL;i++)
    {
     if(s1->nom!=NULL)
       {
        printf("%d : %s = ",i,s1->nom);

        if(s1->valeur!=NULL)
          printf("%s",s1->valeur);

        printf("\n");
       }
     s1++;
    }
}

void efface_symbol(void)
/*
efface les symbols du tableau
*/
{
 struct Symbol *s1;

 s1=table_symbol;
 while(s1 != &(table_symbol[MAX_SYMBOL-1]))
   {
    if(s1->nom!=NULL)
      free(s1->nom);

    if(s1->valeur!=NULL)
      free(s1->valeur);

    s1++;
   }
}

char *recherche(char s[])
/*
cherche si la macro s est definie
si elle le trouve, la variable globale symbol_trouve=1 et
   retourne la valeur
sinon retourne NULL et symbol_trouve=0
*/
{
 int i;
 struct Symbol *s1;

 s1=table_symbol;symbol_trouve=0;
 for(i=0;i<MAX_SYMBOL;i++)
   {
    s1=&(table_symbol[i]);
    if((s1->nom!=NULL)&&(strcmp(s,s1->nom)==0))
      {
       symbol_trouve=1;
       return s1->valeur;
      }
    s1++;
   }

 return NULL;
}

int is_define(char s[])
/*
verifie si s commence par #define
si c'est vrai, la macro est ajoutee au tableau et retourne 1
sinon retourne 0
*/
{
 int i,j;
 char nom[MAX_NOM],contenu[MAX_CONTENU],*p;
 struct Symbol *s1;

 if((p=is_command_for_preprocess(s,"define")))
   {
    i=(int)(p-s);

    /* copie le nom de la macro dans nom */
    j=0;
    while(isalpha(s[i]))
      {/* isalpha(0)==0 */
       nom[j]=s[i];
       j++;i++;
      }
    nom[j]=0;

    /* saute les espaces */
    while((i<(int)strlen(s))&&(s[i]==' ')) i++;

    /* copie la valeur de la macro dans contenu (s'il y en a une) */
    j=0;
    while(i<(int)strlen(s)-1)/* longueur-1 car il y a \n a la fin */
      {
       contenu[j]=s[i];
       j++;
       i++;
      }
    contenu[j]=0;

    /* met le nom et la valeur dans le tableau */
    s1=place_libre();
    if(s1==NULL)
      {
       fprintf(stderr,"Plus de place pour ajouter des symbols\n");
       return 1;
      }

    s1->nom=(char *)malloc(strlen(nom)+1);
    strcpy(s1->nom,nom);

    s1->valeur=(char *)malloc(strlen(contenu)+1);
    strcpy(s1->valeur,contenu);

    return 1;
   }

 return 0;
}

char *is_ifdef(char s[])
/*
cherche si la chaine s commence par #ifdef
si c'est le cas retourne ce qu'il y a apres
sinon retourne NULL
*/
{
 char *p;

 if((p=(char*)is_command_for_preprocess(s,"ifdef")))
   {
    return p;
   }

 return NULL;
}

/* programme principal */
int main(int argc, char *argv[])
{
 int i;
 FILE *fd_in,*fd_out;
 char nomfichier[200],s[MAX+1],*s2,*s3;

 if(argc<2)
   {
    printf("Erreur: Pas assez d'argument\n");
    return 1;
   }

 fd_in=fopen(argv[1],"r");

 if(fd_in==NULL)
   {
    printf("Erreur: Impossible d'ouvrir le fichier %s\n",argv[1]);
    return 2;
   }

 sprintf(nomfichier,"/tmp/%s%d",argv[1],getpid());
 fd_out=fopen(nomfichier,"w");

 if(fd_out==NULL)
   {
    printf("Erreur: Impossible de creer le fichier resultat = %s\n",nomfichier);
    fclose(fd_in);
    return 3;
   }

 /* remplit la table des macro de 0 */
 init_table_symbol();

 while((fgets(s,MAX,fd_in))!=NULL)
   { /* parcour les lignes du fichiers une apres l'autre et les annalyse */
    if(is_define(s)) /* un define */;
    else if((s2=is_ifdef(s)))
      { /* un ifdef */
       /* ne garde que le symbol (enleve les espaces a la fin) */
       for(i=0;i<(int)strlen(s2);i++)
          if(!isalpha(s2[i]))
             break;

       s2[i]=0;

       s3=recherche(s2);
       if(s3==NULL)
         { /* symbol non defini */
          while((fgets(s,MAX,fd_in))!=NULL)
            { /* saute le ifdef */
             if((is_command_for_preprocess(s,"endif"))||
                (is_command_for_preprocess(s,"else")))
               break;
            }

          if(is_command_for_preprocess(s,"else"))
            { /* traite le else */
             while((fgets(s,MAX,fd_in))!=NULL)
               {
                if(is_command_for_preprocess(s,"endif"))
                  break;
                else
                  fputs(s,fd_out);
               }
            }

         }
       else
         { /* symbol defini */
          while((fgets(s,MAX,fd_in))!=NULL)
            { /* retourne le if */
             if((is_command_for_preprocess(s,"endif"))||
                (is_command_for_preprocess(s,"else")))
               break;
             else
               fputs(s,fd_out);
            }
          if(is_command_for_preprocess(s,"else"))
            { /* saute le else */
             while((fgets(s,MAX,fd_in))!=NULL)
               {
                if(is_command_for_preprocess(s,"endif"))
                  break;
               }
            }
         }

      }
    else
      { /* ce n'est pas une directive pour le preprocesseur */
       fputs(s,fd_out);
      }

   }

 /*affiche_symbol();*/
 efface_symbol();

 fclose(fd_in);
 fclose(fd_out);

 return 0;
}




