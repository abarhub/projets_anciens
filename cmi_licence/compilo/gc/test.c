#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stddef.h> // pour la macro offsetof
//#include<conio.h>

#include"debugage.h"
#include"gc.h"

/* verifie que la condition est correcte et arrete le programme sinon */
#define VERIF(X) {CHECK_MSG(X,("Le GC ne fonctionne pas ici"));/*printf("Ok\n");*/}

char *p,*p3;
struct TATA {int i,j;struct TATA *suivant;};

struct TATA *p_tata;

#define getch() //getchar()


void valide(void *p,char *str)
{
	
	if(gc_bloc_valide(p))
	{
		printf("%s est valide\n",str);
	}
	else
	{
		printf("%s est invalide\n",str);
	}
}

void test_str(void)
{
	char *s=NULL,*s2;
	
	printf("Test str:\n");
	
	s=strdup("coucou");
	valide(s,"s");
	VERIF(gc_bloc_valide(s));
	printf("s=%s\n",s);
	
	s2=s;
	
	printf("Effacement de s:\n");
	free(s);
	valide(s,"s");
	valide(s2,"s2");
	
	VERIF(!gc_bloc_valide(s));
	VERIF(gc_bloc_valide(s2));
	
	printf("s2=%s\n",s2);
	
	p=s2;
	
	free(s2);
	VERIF(s2==NULL);
	
	valide(p,"p");
	VERIF(!gc_bloc_valide(s2));
	VERIF(gc_bloc_valide(p));
	printf("p=%s\n",p);
	
	printf("Fin test str\n");
	getch();
}

void test_struct(void)
{
	struct TATA *p1,*p2;
	int i,j;
	
	free(p);free(p3);gc();
	VERIF(p==NULL);VERIF(p3==NULL);
	i=nb_elt();
	printf("Il y a %d elements\n",i);
	printf("test struct:\n");
	
	p1=NULL;
	p1=malloc(sizeof(struct TATA));
	p1->i=9;p1->j=72;p1->suivant=NULL;
	VERIF(nb_elt()-i==1);/* 1 bloc memoire ajoute depuis le debut de la fonction */
	
	p2=malloc(sizeof(struct TATA));
	p2->i=51;p2->j=999;p2->suivant=p1;
	p1->suivant=p2;
	VERIF(nb_elt()-i==2); /* 2 blocs memoires ajoutes depuis le debut de la fonction */
	
	printf("p1(%p):i=%d,j=%d,suivant=%p\n",p1,p1->i,p1->j,p1->suivant);
	printf("p2(%p):i=%d,j=%d,suivant=%p\n",p2,p2->i,p2->j,p2->suivant);
	
	VERIF(gc_bloc_valide(p1));
	VERIF(gc_bloc_valide(p2));
	VERIF(p1->suivant==p2);
	VERIF(p2->suivant==p1);
	VERIF(!gc_bloc_valide(p));
	VERIF(!gc_bloc_valide(p3));
	VERIF(!gc_bloc_valide(p_tata));
	
	gc_affiche();
	getch();
	
	//p=p1;p3=p2;
	i=nb_elt();
	gc();
	j=nb_elt();
	valide(p1,"p1");
	valide(p2,"p2");
	valide(p,"p");
	valide(p3,"p3");
	valide(p_tata,"p_tata");
	VERIF(gc_bloc_valide(p1));
	VERIF(gc_bloc_valide(p2));
	VERIF(!gc_bloc_valide(p));
	VERIF(!gc_bloc_valide(p3));
	VERIF(!gc_bloc_valide(p_tata));
	
	printf("Il y a eut %d recuperations(=0?)\n",i-j);
	VERIF(i-j==0); // pourquoi ca marche  pas avec la zone static ?
	getch();
	
	printf("Effacement de p1\n");
	i=nb_elt();
	free(p1);
	j=nb_elt();
	valide(p1,"p1");
	valide(p2,"p2");
	printf("Il y a eut %d recuperations(=0?)\n",i-j);
	VERIF(!gc_bloc_valide(p1));
	VERIF(gc_bloc_valide(p2));
	VERIF(i-j==0);
	getch();
	
	printf("traitement du GC\n");
	i=nb_elt();
	gc();
	j=nb_elt();
	printf("Il y a eut %d recuperations(=0?)\n",i-j);
	VERIF(!gc_bloc_valide(p1));
	VERIF(gc_bloc_valide(p2));
	VERIF(i-j==0);
	getch();
	
	printf("p2(%p):i=%d,j=%d,suivant=%p\n",p2,p2->i,p2->j,p2->suivant);
	//printf("p1(%p):i=%d,j=%d,suivant=%p\n",p1,p1->i,p1->j,p1->suivant);
	
	p=(char*)p2;
	VERIF(!gc_bloc_valide(p1));
	VERIF(gc_bloc_valide(p2));
	VERIF(gc_bloc_valide(p));
	VERIF(!gc_bloc_valide(p3));
	VERIF(p==(char*)p2);
	
	printf("Effacement de p2\n");
	i=nb_elt();
	free(p2);
	j=nb_elt();
	valide(p1,"p1");
	valide(p2,"p2");
	printf("Il y a eut %d recuperations(=0?)\n",i-j);
	valide(p,"p");
	VERIF(!gc_bloc_valide(p1));
	VERIF(!gc_bloc_valide(p2));
	VERIF(gc_bloc_valide(p));
	VERIF(!gc_bloc_valide(p3));
	VERIF(i-j==0);
	printf("p=%p\n",p);
	
	printf("Effacement de p\n");
	i=nb_elt();
	free(p);
	j=nb_elt();
	valide(p,"p");
	printf("Il y a eut %d recuperations(=2?)\n",i-j);
	VERIF(!gc_bloc_valide(p));
	VERIF(i-j==2);
	
	printf("fin test struct\n");
	getch();
}

void test_global(void)
{
	struct TATA *p1,*p2;
	int i,j;
	
	printf("Test des variables globales:\n");
	
	free(p);free(p3);
	VERIF(!gc_bloc_valide(p));VERIF(!gc_bloc_valide(p3));
	i=nb_elt();
	printf("Il y a %d elements\n",i);
	
	printf("Creation d'un cycle ( p1->p2 et p2->p1 )\n");
	p1=malloc(sizeof(struct TATA));
	p1->i=9;p1->j=72;p1->suivant=NULL;
	VERIF(nb_elt()-i==1); /* 1 bloc cree */
	
	p2=malloc(sizeof(struct TATA));
	p2->i=51;p2->j=999;p2->suivant=p1;
	VERIF(nb_elt()-i==2); /* 2 blocs crees */
	p1->suivant=p2;
	VERIF(p1->suivant==p2);
	VERIF(p2->suivant==p1);
	
	printf("Une var globale(p_tata) pointe sur le cycle et"
			" les var locales(p1 et p2) sont effacees\n");
	p_tata=p2;
	VERIF(p_tata==p2);
	i=nb_elt();
	free(p1);free(p2);
	j=nb_elt();
	printf("Il y a eut %d recuperations(=0?)\n",i-j);
	VERIF(i-nb_elt()==0);
	
	valide(p1,"p1");valide(p2,"p2");
	valide(p_tata,"p_tata");
	VERIF(!gc_bloc_valide(p1));
	VERIF(!gc_bloc_valide(p2));
	VERIF(gc_bloc_valide(p_tata));
	
	printf("Nettoyage de la memoire\n");
	i=nb_elt();
	gc();
	j=nb_elt();
	printf("Il y a eut %d recuperations(=0?)\n",i-j);
	VERIF(i-j==0);
	
	valide(p1,"p1");valide(p2,"p2");
	valide(p_tata,"p_tata");
	VERIF(!gc_bloc_valide(p1));
	VERIF(!gc_bloc_valide(p2));
	VERIF(gc_bloc_valide(p_tata));
	
	printf("p_tata(%p):i=%d j=%d suivant=%p et suivant(%p):i=%d j=%d suivant=%p\n",
			p_tata,
			p_tata->i,p_tata->j,p_tata->suivant,p_tata->suivant,p_tata->suivant->i,
			p_tata->suivant->j,p_tata->suivant->suivant);
	
	i=nb_elt();
	free(p_tata);
	j=nb_elt();
	printf("Il y a eut %d recuperations(=2?)\n",i-j);
	VERIF(i-j==2);
	
	printf("fin test var globale\n");
	getch();
}

/* test si l'implementation du compilateur/lieur
correspond a ce qu'il faut */
void test_correct_compile(int *i)
{
	struct TATA
	{
		int i;
		char c;
		int j;
	} b,c;
#ifdef __BORLANDC__
	register int k=9483,l=27618;
#endif
	int j,*p1,*p2;
	
	/* test si la pile est bien gere */
	if(&j>=i)
	{
	    printf("Erreur: la pile n'est pas gere comme il le faudrait\n"
			"quand la pile augmente, l'adresse du sommet de la pile diminue\n");
	    exit(1);
	}
	else
	{
	    printf("Ok: le pile est gere comme il le faut\n");
	}
	
	/* test si l'alignement est correcte */
	j=offsetof(struct TATA,j);
      printf("j=%d\n",j);
	if(j!=sizeof(int)*2)
	{
	    printf("Erreur: l'alignement n'est pas sur la taille d'un int\n");
	    printf("l'alignement devrait etre sur %d octect\n",sizeof(int));
	    exit(1);
	}
	else
	{
	    printf("Ok: l'alignement est de la taille d'un int\n");
	}
	
#ifdef __BORLANDC__
	/* test si les registres sont desactives */
	p1=(&j)+1;
	printf("j(%p)=%d,k(%p)=%d,l(%p)=%d\n",&j,j,k,k,l,l);
	if((*p1!=27618)||(*(p1+1)!=9483))
	{
		printf("Erreur: Il faut desactiver l'utilisation des registres\n");
		exit(1);
	}
	else
	{
	    printf("Ok: Les registres sont desactives\n");
	}
#endif	

	if((int)(&j)%2!=0)
	{
		printf("Erreur: Il n'y a pas d'alignement sur un multiple de 2\n");
		exit(1);
	}
	else
	{
	    printf("Ok: l'alignement est un multiple de 2\n");
	}

	printf("La compilation est correcte\n");
	getch();
}


void test2(void)
{
	int i=92,j=91,k=90;
	
	//setjmp(env);
	
	printf("test2:i=%d j=%d k=%d\n",i,j,k);
	
	/*fin=&k;
	printf("test2: fin=%p et debut=%p et env=%p\n",fin,debut,env);
	/*if(fin==env)
	{
		printf("")
	}*/
	//fin=(char*)&env;
	gc();
	
}

void test1(void)
{
	int i=50,j=51;
	
	printf("test1: i=%d j=%d\n",i,j);
	test2();
}

int main()
{
	int i,*k=(int*)47,j;
	struct TOTO { int i,j;char *nom;} *p1,*p2;
	int a=43;

	/*t.ch1=8;t.ch2=(int)-1;
	printf("t.ch1=%d, t.ch2=%d\n",t.ch1,t.ch2);
	m2=&m1;
	m2->suivant=9658;
	printf("m2=%d,%d,%d\n",m2->suivant,GET_MARK(m2),GET_SUIVANT(m2));
	SET_MARK(m2,1);
	m2->suivant = (0==1)? (((int)m2->suivant)&(~1))|1 : 
	    (((int)m2->suivant)&(~1));
	printf("m2=%d,%d,%d\n",m2->suivant,GET_MARK(m2),GET_SUIVANT(m2));
	SET_MARK(m2,1);
	printf("m2=%d,%d,%d\n",m2->suivant,GET_MARK(m2),GET_SUIVANT(m2));
	SET_SUIVANT(m2,900);
	printf("m2=%d,%d,%d\n",m2->suivant,GET_MARK(m2),GET_SUIVANT(m2));

	return 0;*/
		
	/*p1=(struct TOTO*)0x6625f8;
	p2=(char*)p1+sizeof(char)*12;
	printf("p1=%p p2=%p p2-p1=%d\n",p1,p2,p2-p1);
	
	return 0;*/
	
	GC_INIT(i);

	/* determine si la compilation est comme il faut,
	et si il n'y a pas des pb d'architecture */
	test_correct_compile(&i);

	gc_ajoute_global((void**)&p);
	gc_ajoute_global((void**)&p3);
	gc_ajoute_global((void**)&p_tata);
	printf("p=%x\n",&p);
	gc_affiche();
	getch();
	
	//return 0;
	
	/*p=strdup("salut");
	
	valide(p,"p");
	printf("p=%s\n",p);
	
	gc();
	
	valide(p,"p");
	
	return 0;*/
	i=0;j=0;
	while(i<100)
	{
		j+=i+9*i;
		printf("j=%d= et i=%d\n",j,i);
		i++;
	}
	
	//p1=7;p2=31;
	
	test1();
	
	p1=malloc(sizeof(struct TOTO));
	p2=malloc(sizeof(struct TOTO));
	
	//gc();
	
	//printf("p1.i=%d p2.j=%d\n",p1->i,p2->j);
	printf("p1=%p  p2=%p rech1=%p rech2=%p\n",
			p1,p2,gc_cherche((p1)),gc_cherche((p2)));
	getch();
	
	/*test1();
	
	getch();*/
	
	gc_affiche();
	
	gc();
	
	printf("p1=%p(p2->i=%d)  p2=%p(p2->i=%d) rech=%p rech=%p (%d)\n",
			p1,p1->i,p2,p2->i,gc_cherche(p1),gc_cherche(p2),a);
	
	printf("effacement de p1\n");
	free(p1);
	
	if(gc_bloc_valide(p1))
	{
		printf("p1 est valide\n");
		printf("p1=%p p1->i=%d\n",p1,p1->i);
	}
	else
	{
		printf("p1 est invalide\n");
	}
	
	if(gc_bloc_valide(p2))
	{
		printf("p2 est valide\n");
	}
	else
	{
		printf("p2 est invalide\n");
	}
	
	p=strdup("salut");
	printf("p=%s(0)\n",p);
	p=malloc(50);
	strcpy(p,"coucou");
	printf("p=%s %p\n",p,p);
	
	printf("salut(1)\n");
	p3=strdup(p);
	printf("salut(2)\n");
	free(p);
	printf("salut(3)\n");
	printf("p3=%d %p %p\n",p3,p3,p);
	
	test_str();
	
	test_struct();
	
	test_global();
	
	printf("Bravo: Le gc fonctionne sur cet ordinateur  !!!\n");

	return 0;
}