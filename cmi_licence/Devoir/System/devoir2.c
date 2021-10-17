#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdarg.h>
#include<unistd.h>
#include<assert.h>

/*

BARRET Alain
SCHUDEL Edward

exercice 2:
lecture des define puis remplacement dans le fichier resultat
les macros peuvent etre sur plusieurs lignes
et il y reannalyse ce qu'il remplace en evitant les boucles infinies
ex:
  #define TOTO(X) {  \
                 X  \
                  }
  TOTO(salut)
  #define TATA TOTO TATA+1
  TATA

donne:
  {
  salut
  }

  {

  }
  TATA+1

*/

#define MAX (100)
#define MAX_NOM (30)
#define MAX_MACRO (300)
#define MAX_PARAMETRE (10)

/* Utile pour le debugage */
#define DEBUGAGE
#undef DEBUGAGE

/* retourne vrai si la line est une ligne pour le preprocesseur */
#define IS_LINE_FOR_PREPROCESS(X) (X[0]=='#')
#define SKIP_SPACE(X) {while(*(X)==' ') (X)++;}
#define VALIDE_NOM(X) (isalnum(X)||((X)=='_'))

/* liste chainee d'instructions pour le c et de parametres de la fonction*/
struct liste_def {
enum {EParametre,EInstruction} type; /* type de noeud (un parametre*/
                                     /* de la macro ou non)*/
union {
int no_var;       /* numero de la macro*/
char *instruction;  /* des instructions*/
} contenu;
struct liste_def *suivant;  /* noeud suivant*/
};

/* une macro*/
struct smacro {
char nom[MAX_NOM]; /* nom de la macro*/
struct liste_def *definition; /* corps de la macro*/
                              /*(liste chainee d'instruction et de variables)*/
int nb_param;   /* nombre de parametres de la macro*/
int deja_traite; /* si la macro a deja ete expanse une fois*/
};

/* type de retour de la fonction find_next_identifier */
struct Trouve_ident
{
 int no_identifiant; /* numero dans le tableau*/
 char *ident;        /* position dans la chaine*/
};

struct smacro table_macro[MAX_MACRO]; /* l'ensemble des macros definies*/
int no_ligne=-1; /*numero de ligne actuelle*/

void affiche();

/*
envoie des message d'erreur sur la sortie d'erreur standard
avec le numero de ligne et et un retour a la ligne
les parametres sont les memes que printf
*/
void erreur(char *s,...)
{
 va_list arg;
 va_start(arg,s);
 fprintf(stderr, "Erreur ");
 if(no_ligne>0)
   { /* on a commencer le traitement des lignes*/
    fprintf(stderr,"a la ligne %d ",no_ligne);
   }
 fprintf(stderr,": ");
 vfprintf(stderr, s, arg);
 fprintf(stderr,"\n");
 va_end(arg);
}

/*
efface l'ensemble des macros definies
doit etre apple avant la fin du programme
*/
void efface(void)
{
 struct smacro *p;
 int i;
 struct liste_def *p2,*p3;

 for(i=0,p=table_macro;i<MAX_MACRO;i++,p++)
   {
    if(p->definition!=NULL)
      {
       p2=p->definition;
       while(p2!=NULL)
         {
          if((p2->type==EInstruction)&&(p2->contenu.instruction!=NULL))
            free(p2->contenu.instruction);

          p3=p2->suivant;
          free(p2);p2=p3;
         }
      }
   }
 return;
}

char *is_command_for_preprocess(char line[],char command[])
/*
determine si la chaine line correspond a la commande du preprocesseur =
command
si c'est le cas, retourne le reste de la ligne en enlevant les 1er =
espaces
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


struct liste_def *add_struct_def(struct smacro *macro,struct liste_def *def)
/*
ajoute def a la fin de macro->definition
*/
{
 struct liste_def *d1;

 if(macro==NULL)
    {
     erreur("le parametre macro ne doit pas etre NULL");
     return NULL;
    }

 d1=macro->definition;
 if(d1==NULL)
    {
     macro->definition=def;
     def->suivant=NULL;
     return def;
    }
 else
    {
     while(d1->suivant!=NULL)
     	 d1=d1->suivant;

     d1->suivant=def;
     return d1;
    }
}

/*
ajoute la chaine s a la chaine p (adresse du pointeur sur la chaine p)
en recadrant la zone alouee
si s=NULL ou *s=0 ne fait rien
si longueur_de_s est >=0 , ne prend que les
                 longueur_de_s 1er caractere
                 mais ne verifi pas si s est plus petite que longueur_de_s
*/
int add_string(char **p,char *s,int longueur_de_s)
{
 int i,j;

 if((s==NULL)||(s[0]==0)||(longueur_de_s==0))
   return 0;

 if(longueur_de_s<0)
   i=strlen(s);
 else
   i=longueur_de_s;

 if(*p==NULL)
   {
    *p=(char *)malloc(i+1);
    strncpy(*p,s,i);
    (*p)[i]=0;
   }
 else
   {
    j=strlen(*p);
    *p=(char *)realloc(*p,j+i+1);

    strncat(*p,s,i);
    (*p)[j+i]=0;

   }
 return 0;
}

/*
ajoute un noeud de type parametre de macro a la fin de m->definition
*/
int add_macro_param(struct smacro *m,int var)
{
 struct liste_def *def1,*def2;

 def1=(struct liste_def *)malloc(sizeof(struct liste_def));
 if(def1==NULL) return 1;
 def1->type=EParametre;
 def1->contenu.no_var=var;
 def1->suivant=NULL;
 if(m->definition==NULL)
   m->definition=def1;
 else
   {
    def2=m->definition;
    while(def2->suivant!=NULL)
      def2=def2->suivant;

    def2->suivant=def1;
   }

 return 0;
}

/*
ajoute un noeud de type instruction de macro a la fin de m->definition
*/
int add_macro_instr(struct smacro *m,char *instr)
{
 struct liste_def *def1,*def2;

 def1=(struct liste_def *)malloc(sizeof(struct liste_def));
 if(def1==NULL) return 1;
 def1->type=EInstruction;
 def1->contenu.instruction=instr;
 def1->suivant=NULL;
 if(m->definition==NULL)
   m->definition=def1;
 else
   {
    def2=m->definition;
    while(def2->suivant!=NULL)
      def2=def2->suivant;

    def2->suivant=def1;
   }

 return 0;
}

/*
determine si dans la chaine ligne, un des identifier est present
et retourne son adresse dans ligne et son numero dans le tableau
retourne une chaine null et un numero negatif sinon
*/
struct Trouve_ident find_next_identifier(char *ligne/*ligne a annalyser*/,
                                         char *identifier/*tableau d'identifier*/,
                                         int max_tableau/*taille du tableau*/,
                                         int taille_1_elt/*taille d'un element du tableau*/)
{
 char *p;
 int i,j;
 struct Trouve_ident ident={-1,NULL};

 p=ligne;
 while(*p!=0)
   {
    while((*p!=0)&&(!VALIDE_NOM(*p)))
      p++;

    if(*p==0)
      return ident;

    i=0;
    while(VALIDE_NOM(p[i]))
      i++;

    for(j=0;j<max_tableau;j++)
       {
        if((strlen(identifier+taille_1_elt*j)==i)&&
           (strncmp(identifier+taille_1_elt*j,p,i)==0))
          {
           ident.ident=p;
           ident.no_identifiant=j;
           return ident;
          }
       }

    p+=i;
   }

 return ident;
}

/*
determine si la fin de la ligne se termine par un '\\' suivi d'un retour a la ligne
fait la diferrence entre les retours a la ligne \r\n et \n
*/
int suite_sur_ligne_suivante(char *ligne)
{
 int longueur=strlen(ligne);

 if((ligne==NULL)||(longueur<2))
   return 0;

 if((longueur>=2)&&(ligne[longueur-1]=='\n')&&(ligne[longueur-2]=='\\'))
     return 1;

 if((longueur>=3)&&(ligne[longueur-1]=='\n')&&(ligne[longueur-2]=='\r')
     &&(ligne[longueur-3]=='\\'))
     return 1;

 return 0;
}

/*
regarde si ligne est une ligne pour le define,
et c'est le cas, traite la macro et retourne 1
sionon retourne 0
*/
int is_define(char *ligne,FILE *fd)
{
 char *p,*p2,*p3,*p4,buf[500],*begin;
 int i,no_param,nb_param;
 struct smacro *t;
 char param[MAX_PARAMETRE][MAX_NOM];
 struct Trouve_ident id_found;

 if((p=is_command_for_preprocess(ligne,"define"))!=NULL)
   { /* c'est une ligne qui commence par un define */

    begin=NULL;
    add_string(&begin,p,-1);
    if(suite_sur_ligne_suivante(ligne))
      {/* un 'define' sur plusieurs lignes */
       while(suite_sur_ligne_suivante(ligne))
         {
          i=strlen(begin);
          if((begin[i-2]=='\r'))
              begin[i-3]='\r';
          begin[i-2]='\n';begin[i-1]=0;

          if(((fgets(ligne,MAX,fd))!=NULL))
             {
              add_string(&begin,ligne,-1);
              no_ligne++;
             }
          else
             break;
         }
      }
    p=begin;

    /* recherche de la 1ere place libre dans le tableau */
    t=table_macro;
    while((t->nom[0]!=0)&&(t<=&table_macro[MAX_MACRO-1]))
       t++;

    if(t<=&table_macro[MAX_MACRO-1])
      { /* il y a une place libre */

       /* copie dans nom le nom de la macro */
       p2=p;i=0;
       while((p2!=0)&&(isalpha(*p2))&&(i<30))
        {
         t->nom[i]=*p2;
         i++;p2++;
        }
       t->nom[i]=0;

       /* m4 ne veux pas d'espace entre le nom et les parametres
       pour le changer, enlever le commentaire qui suit */
       /*SKIP_SPACE(p2);*/

       /* recherche s'il y a des parametres */
       nb_param=0;
       if(*p2=='(')
         { /* Il y a des parametres */
          memset(&param,0,sizeof(param));

          no_param=0;
          p2++;
          while((*p2!=0)&&(*p2!='\n')&&(*p2!=')'))
            { /* parcourt des parametres */
             SKIP_SPACE(p2);

             p3=p2;
             while(VALIDE_NOM(*p2)) p2++;

             if(p2<=p3)
               { /* caracteres interdits en parametre de la mecro */
                param[no_param][0]=0;
                erreur("Il y a une erreur dans le format du fichier"
                       " pour la macro %s",t->nom);
                if(begin!=NULL)
                   free(begin);
                return 1;
               }
             else
               {
                strncpy(param[no_param],p3,p2-p3);
               }

             SKIP_SPACE(p2);
             if(*p2==',') p2++;
             no_param++;
            }
          nb_param=no_param;
          if(*p2==')') p2++;
          else
            erreur("les parametres doive se terminer par une )");
#ifdef DEBUGAGE
          printf("Parametre de la macro %s",t->nom);
          for(i=0;i<MAX_PARAMETRE;i++)
          	if(param[i][0]!=0) printf("%d=%s\n",i,param[i]);
#endif
         }/* fin de l'annalyse des parametres */
       p3=p2;
       t->nb_param=nb_param;

       buf[0]=0;
       t->definition=NULL;
       p4=NULL;

       /* parcourt le reste de la ligne*/
       while((id_found=
                find_next_identifier(p3,(char *)param,nb_param,MAX_NOM))
                .ident!=NULL)
            { /* tant que le reste de la ligne a au moins un parametre*/
             p4=(char *)malloc(id_found.ident-p3+1);
             strncpy(p4,p3,id_found.ident-p3);
             p4[id_found.ident-p3]=0;
             if(p4[strlen(p4)-1]=='\n')
               p4[strlen(p4)-1]=0;
             if(p4[strlen(p4)-1]=='\r')
               p4[strlen(p4)-1]=0;
             add_macro_instr(t,p4);
             add_macro_param(t,id_found.no_identifiant);
             p3=id_found.ident+strlen(param[id_found.no_identifiant]);
            }

       if(id_found.ident==NULL)
         { /* plus de parametres sur la ligne*/
          p4=(char *)malloc(strlen(p3)+1);
          strcpy(p4,p3);
          if(p4[strlen(p4)-1]=='\n')
            p4[strlen(p4)-1]=0;
          if(p4[strlen(p4)-1]=='\r')
            p4[strlen(p4)-1]=0;
          add_macro_instr(t,p4);
         }
       if(begin!=NULL)
          free(begin);
       return 1;
      }
    else
      { /* il n'y a pas de place libre */
       erreur("plus de place pour ajouter la macro:\n%s",p);
       if(begin!=NULL)
         free(begin);
       return 1;
      }
   }
 else return 0;
}

/*
affiche toutes les macros
util pour le debogage
*/
void affiche(void)
{
 struct liste_def *p1;
 int i;

 printf("Affichage des macro:\n");
 for(i=0; i<MAX_MACRO;i++)
    {
     if(table_macro[i].nom[0]!=0)
       {
        printf("macro %d = %s(%d parametre%s)",i,
               table_macro[i].nom,table_macro[i].nb_param,
               table_macro[i].nb_param<2 ? "":"s");

        p1=table_macro[i].definition;
        if(p1!=NULL)
          {
           printf("{");
           while(p1!=NULL)
             {
              if(p1->type==EInstruction)
                  printf("%s",p1->contenu.instruction);
              else if(p1->type==EParametre)
                printf("var%d",p1->contenu.no_var);
              else
                erreur("pb de type(=%d) pour l'affichage",p1->type);

              p1=p1->suivant;
              if(p1!=NULL) printf(",");
             }
           printf("}");
          }

        printf("\n");
       }
    }
}

/*
met au plus les n premiers caracters de s dans le fichier f
arrete le programme s'il y a un pb
*/
void fputsn(char *s,int n,FILE *f)
{
 int i;

 for(i=0;(i<n)&&(s[i]!=0);i++)
    if(fputc(s[i],f)==EOF)
      {
       erreur("Ne peut plus ajouter de caracter "
              "au fichier resultat (manque de place ?)");
       efface();
       exit(1);
      }

 return ;
}

/*
annalyse la chaine p pour une longueur maximum de longueur
et retourne la chaine avec les transformations de macro
la fonction est recursive:
apres la 1ere expansion, elle essai d'y faire une nouvelle expansion
en evitant toujours les boucles infinies
ex:
 #define A(X) 1+A(X)
 A(3)
donne:
 1+A(3)
*/
char *traitement(char *p,int longueur)
{
 char param[MAX_PARAMETRE][MAX_NOM];
 int i,k;
 char *result=NULL,*p3,*p2,*debut=p,*result2=NULL;
 struct liste_def *def1;
 struct Trouve_ident t1;

 if((p==NULL)||(*p==0))
   return NULL;

 while((*p!=0)&&(p-debut<longueur))
   {
    /* determine la prochaine macro a expanser*/
    i=0;
    while(((t1=find_next_identifier(p+i,(char *)table_macro,
          MAX_MACRO,sizeof(struct smacro))).ident!=NULL)&&
          (table_macro[t1.no_identifiant].deja_traite!=0))
      {
       i+=t1.ident-p+1;
      }

    if((t1.ident==NULL)||(t1.ident>p+longueur)||
       (table_macro[t1.no_identifiant].deja_traite!=0))
      { /* il n'y a plus de macro dans le reste de la ligne*/
       add_string(&result,p,-1);
       break;
      }
    else
      { /* il y a une macro a expanser*/
       add_string(&result,p,t1.ident-p);

       /* pour eviter de retraite la macro plusieur fois */
       table_macro[t1.no_identifiant].deja_traite=1;

       p2=p;
       for(i=0;VALIDE_NOM(t1.ident[i]);i++) ;
       /*assert(i==strlen(table_macro[t1.no_identifiant].nom));*/

       p=t1.ident+i;

       memset(&param,0,sizeof(param));

       if(table_macro[t1.no_identifiant].nb_param>0)
         { /* enleve les espaces avant la parenthese ouvrante */
          for(i=0;(p[i]==' ')||(p[i]=='\t');i++) ;
          if(p[i]=='(')
            SKIP_SPACE(p);
         }

       if(*p=='(')
         { /* il y a des parametres*/
          p++;k=0;
          while((*p!=')')&&(*p!=0)&&(k<MAX_PARAMETRE)&&(p-debut<longueur))
            {
             p3=p;
             while((*p3!=0)&&(*p3!=',')&&(*p3!=')')&&(p3-debut<longueur))
               p3++;

             /* ajout de la chaine entre p et p3 dans param[k] */
             strncpy(param[k],p,p3-p);
             param[k][p3-p]=0;
             k++;
             p=p3;
             if((*p!=0)&&(*p!=')')&&(p-debut<longueur))
               p++;
            }

          if(k==MAX_PARAMETRE)
            erreur("Trop de parametre");

#ifdef DEBUGAGE
          for(i=0;i<k;i++)
             printf("Parametre %d= %s\n",i,param[i]);
#endif

          if(*p==')') /* parentese fermante */
            p++;
         }

       def1=table_macro[t1.no_identifiant].definition;
       while(def1!=0)
         {
          if(def1->type==EInstruction)
            add_string(&result,def1->contenu.instruction,-1);
          else
            add_string(&result,param[def1->contenu.no_var],-1);

          def1=def1->suivant;
         }
      }
   }

 if((result!=NULL)&&(strncmp(result,debut,strlen(result))!=0))
   { /* si la fonction a fait une expansion de macro */
    result2=traitement(result,strlen(result));
    if(strcmp(result,result2)==0)
      { /* pas de changement dans result2 */
       free(result2);
      }
    else
      { /* il y a eu expansion dans result2 */
       free(result);
       result=result2;
      }
   }

 return result;
}

/*
cherche dans la ligne une des macros, les remplace si il y en a,
et envoi la ligne dans le fichier de sortie
*/
void annalyse_ligne(char *s,FILE *fd)
{
 char *p2,*p,*p3;
 int i;
 struct Trouve_ident t1;

 if((s==NULL)||(*s==0))
   return;

#ifdef DEBUGAGE
 printf("pour l'entree : %s$",s);
#endif

 p=s;
 while(*p!=0)
   { /* annalyse jusqu'a la fin de la ligne */
    /* recherche de la prochaine macro a expanser */
    t1=find_next_identifier(p,(char *)table_macro,MAX_MACRO,
                            sizeof(struct smacro));
    if(t1.ident==NULL)
      { /* le reste de la ligne ne doit pas etre expanse */
       fputs(p,fd);
       return;
      }
    else
      {
       i=0;p2=p;
       while(p!=t1.ident)
         {
          fputc(*p,fd);
          p++;
         }
       p2=p;
       p+=strlen(table_macro[t1.no_identifiant].nom);
       if(*p=='(')
         { /* il y a des parametres */
          p++;
          while((*p!=0)&&(*p!=')'))
            p++;

          if(*p==')')
            p++;
         }

       for(i=0;i<MAX_MACRO;i++)
          table_macro[i].deja_traite=0;

       /* expansion de la macro entre p2 et p et resultat dans p3 */
       p3=traitement(p2,p-p2);
       if(p3!=NULL)
         {
#ifdef DEBUGAGE
          printf("result=%s\n",p3);
#endif
          fputs(p3,fd);
          free(p3);
         }
      }
   }

 for(i=0;i<MAX_MACRO;i++)
    table_macro[i].deja_traite=0;

 return;
}

/* programme principal*/
int main(int argc,char *argv[])
{
 FILE *fd_in,*fd_out;
 char sortie[FILENAME_MAX+1]="testout.txt",s[MAX+1];

#ifdef MSS
 MSS_STARTUP;
#endif

 if(argc!=2)
   { /* le nombre d'argument ne convient pas */
    erreur("Il faut un argument\nex: %s fichier_a_lire.c",argv[0]);
    exit(1);
   }

 fd_in=fopen(argv[1],"r");
 if(fd_in==NULL)
   { /* erreur d'ouverture du fichier en lecture */
    erreur("Impossible d'ouvrir le fichier en lecture = %s",argv[1]);
    exit(1);
   }

 sprintf(sortie,"/tmp/%s%d",argv[1],getpid());
 fd_out=fopen(sortie,"w");
 if(fd_out==NULL)
   { /* erreur d'ouverture du fichier en ecriture */
    erreur("Impossible d'ouvrir le fichier en ecriture = %s",sortie);
    fclose(fd_in);
    exit(1);
   }

 /* remplit le tableau de 0 */
 memset(&table_macro,0,sizeof(table_macro));

 no_ligne=1;

 while((fgets(s,MAX,fd_in))!=NULL)
   { /* lit une ligne apres l'autre*/
    if(is_define(s,fd_in)) ;
    else annalyse_ligne(s,fd_out);

    no_ligne++;
   }

#ifdef DEBUGAGE
 affiche();
#endif

 /* efface l'ensemble des macros*/
 efface();

#ifdef MSS
 MSS_LOG_BLOCK_LIST;
#endif

 /* ferme les fichiers*/
 fclose(fd_in);fclose(fd_out);

 return 0;
}
