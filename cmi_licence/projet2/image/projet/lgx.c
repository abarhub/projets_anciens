#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
/* -------------------------------------------------------------------------
 *
 *                 INTERFACE X suivant la fenetre reelle
 *
 * ----------------------------------------------------------------------- */

/* Definition de la taille de la fenetre */

     /* Definition de la position initiale de la fenetre */

#define X0  200
#define Y0  300

     /* Dimensions reelles par defaut */
#define LARGEUR_INITIALE    346
#define HAUTEUR_INITIALE     225
     /* Dimensions fictives (et reelle maximale) par defaut */
#define LARGEUR_FICTIVE    6920
#define HAUTEUR_FICTIVE     4510          /*  D'ou coef=1/20.    */
     /* Dimensions reelles minimales */
#define MIN_LARG              15
#define MIN_HAUT               9

   /* infen.width et infen.height donnent la taille de la fenetre reelle.
      infen.max_width et infen.max_height celle de la fenetre virtuelle.
      Ces dernieres dimensions sont identifiees a la taille maximale de
      la fenetre reelle.
      Ici sont donnees les valeurs par defaut. */

static XSizeHints infen=
      {PPosition | PSize | PMinSize | PMaxSize ,
         X0,Y0,LARGEUR_INITIALE,HAUTEUR_INITIALE,
         MIN_LARG,MIN_HAUT,
         LARGEUR_FICTIVE,HAUTEUR_FICTIVE};

#define h_ecran (infen.height)
#define l_ecran (infen.width)

/* Ce qu'il faut pour manipuler la fenetre */

static Display *canal;
static int ecran;
static Window  fen;
static GC      cg;
static Window  racine;
static XSetWindowAttributes attr;

typedef struct position {int x,y;} PositionEcran;
struct position poscour={0,0}; /* position courante */

   /* Les couleurs */

#define NB_GRIS_MAX  256
static int nbg = NB_GRIS_MAX;
static XStandardColormap couldispo;
static Colormap palette;
static XColor couleur;
static Visual * visu;
static unsigned long assocol[NB_GRIS_MAX],noir,blanc,bord;

   /* Gestion de la fenetre graphique */

static int traceur_ouvert=0;
#define VTO { if (! traceur_ouvert) ex_openpl(); }

       /* CREATION de la FENETRE: dimensions choisies suivant infen */

static int ex_openpl()
{ if (traceur_ouvert)
    { fprintf(stderr,"Le traceur est deja ouvert\n"); return 0;}
       /* Connection au serveur */
  { char *dp;
    dp=getenv("DISPLAY");
    if (!dp)
      { fprintf(stderr,"'faudrait p'tre penser a affecter $DISPLAY !\n");
        exit(1);
      }
    canal= XOpenDisplay("");
    if (! canal)
      { fprintf(stderr,"Impossible de contacter %s\n",dp);exit(1);}
    else { traceur_ouvert = 1; }
  }
  ecran= DefaultScreen(canal);
  palette=DefaultColormap(canal,ecran);
  noir= BlackPixel(canal,ecran);
  blanc= WhitePixel(canal,ecran);

  /* Definition de la taille reelle de la fenetre (a faire) */
                /* a faire */

  /* Creation de la fenetre */
  racine=XDefaultRootWindow(canal);
  fen= XCreateSimpleWindow(canal, racine,
                           infen.x, infen.y, infen.width, infen.height,
                           2,noir,blanc);

  /* La fenetre est cree , on initialise les parametres d'utilisation */
         /* Couleur du fond  */
  couleur.red= couleur.green=0; couleur.blue= 15000;
  XAllocColor(canal,palette, &couleur);
  attr.background_pixel=couleur.pixel;
         /* Couleur du remplissage lorsque la fenetre n'a pas les
            memes proportions qu'initialement */
  couleur.red= 0; couleur.green=0; couleur.blue= 15000;
  XAllocColor(canal,palette, &couleur);
  bord=couleur.pixel;
            /* On se charge de rafraichir la fenetre */
  attr.backing_store=  NotUseful ;
  XChangeWindowAttributes(canal,fen,CWBackingStore | CWBackPixel,&attr);

  /* Initialisation du contexte graphique */
  cg=XCreateGC(canal,fen,0,0);
  XSetForeground(canal,cg,blanc);
  XSetBackground(canal,cg,attr.background_pixel);

  /* Info pour le gestionnaire de fenetre */
               /* On donne un nom a la fenetre (pour le WM) */
  XStoreName(canal,fen,"Infographie");
               /* On donne un nom a l'icone */
  XSetIconName(canal,fen,"Li6");
               /* Specification de la taille et de l'emplacement */
  /* XSetNormalHints(canal,fen,&infen); */

  /* Visualisation de la fenetre */
  XMapRaised(canal,fen);

  /* Definition des nuances de gris */

  { int i,prof; unsigned short vblanc, vnoir; /* val RGB blanc et noir */
    double facteur; double g0,gi,gnb;

    assocol[0]= blanc;

           /* nombre de gris a determiner en fonction de la
              profondeur de l'ecran et de la disponibilite sur la
              palette */

    visu= DefaultVisual(canal,ecran);
    nbg= visu->map_entries;  /* nbg: le nombre maximal de nuances pouvant
                                     etre obtenues. Par la suite
            on ajustera nbg en fonction du nombre de cellules qu'il
            sera possible d'allouer */

           /* association des valeurs de gris */

    if (nbg == 2)
      { fprintf(stderr,"Cette application n'est pas forcement prevue pour \n");
        fprintf(stderr,"etre executee sur un terminal noir et blanc. Vous\n");
        fprintf(stderr,"risquez de perdre des informations graphiques !\n");
      }
    else
      { couleur.pixel=blanc;
        XQueryColor(canal,palette,&couleur); /* Recuperation de RGB blanc */
        vblanc=couleur.red;
        couleur.pixel=noir; XQueryColor(canal,palette,&couleur);
        vnoir=couleur.red;
              /* On essaye d'avoir un maximum de nuances. On commence
                 par parier sur autant de nuances que la palette peut
                 en contenir */
        { int gagne=0;
          while (!gagne && (nbg > 16))
            { unsigned short ngcour, /* niveau de gris courant */
                             valprec; /* niveau de gris precedent pour voir
                                         si l'on en obtient un nouveau */
              facteur=(vblanc-vnoir)/(nbg-1);
              ngcour=vblanc;
                            /* A partir du blanc on calcul tous les gris */
                                 /* en prenant des intervalles constants */
              i=1; valprec=vblanc;
              while (i< nbg-1)   /* le blanc et le noir sont a part */
                { ngcour-=facteur;
                  couleur.red=couleur.green=couleur.blue=ngcour;
                  if (! XAllocColor(canal,palette, &couleur)) break;
     /* Ici on espere qu'alors la repartion des gris sera uniforme. C'est
        le cas pour les pseudocolors */
                  if (valprec == couleur.red) { nbg--; }
                  else { assocol[i++]= couleur.pixel; valprec=couleur.red; }
                }
              gagne = !(i < nbg-1);
              if (!gagne)
                { if ( i > 100 && nbg > i+2) nbg=i+2;
                       /* bidouille pour aller plus vite lorsque c'est satisfaisant */
                  XFreeColors(canal,palette,assocol+1,i-1,0);
                  nbg--;
                }
            }
          if (!gagne)
            { fprintf(stderr,"Il n'y a pas assez de nuances de gris pour executer cette application\n");
              fprintf(stderr,"Mille excuses ! je suis vraiment desole\n");
              //exit(1);
            }
        }
        if (assocol[nbg-2]==noir) nbg--;
        else assocol[nbg-1]=noir;

/*    for (i=0; i<nbg; i++)
      { couleur.pixel=assocol[i];
        XQueryColor(canal,palette,&couleur);
        printf("%d %d R%d G%d B%d\n",i, assocol[i],
               couleur.red,couleur.green,couleur.blue);
      } */
      }
  }
}

/* Coefficients et transformation de la visualisation courante */

static double coef= ((double) LARGEUR_INITIALE)/LARGEUR_FICTIVE;
static int dx,dy;

#define trans(x,y) ((int) (coef*x+dx)),((int) (coef*y+dy))
#define afftrans(a,x,b,y) {(a)=((short) (coef*x+dx));(b)=((short) (coef*y+dy));}

static void ex_erase ()
{ VTO XClearWindow(canal,fen);
     /* A changer si l'on utilise ces parametres pour une visualisation
        partielle */
  XSetForeground(canal,cg,bord);
  if (dx) { XFillRectangle(canal,fen,cg,0,0,dx,infen.height);
            XFillRectangle(canal,fen,cg,infen.width-dx,0,dx,infen.height);
          }

  if (dy) { XFillRectangle(canal,fen,cg,0,0,infen.width,dy);
            XFillRectangle(canal,fen,cg,0,infen.height-dy,infen.width,dy);
          }
  XSetForeground(canal,cg,blanc);
}

static void ex_opensizedpl(h,l)
int h,l;
{ double c1,c2;
  infen.height=h; infen.width=l;
  c1= h / ((double) HAUTEUR_FICTIVE);
  c2= l / ((double) LARGEUR_FICTIVE);
  coef = ((c1>c2) ? c1 : c2);
  infen.max_height=h/coef; infen.max_width=l/coef;
  ex_openpl(); }

/* Voir ex_erase() apres les parametres de visualisation */

static void ex_line(x1,y1,x2,y2)
short x1,y1,x2,y2;
{ VTO XDrawLine(canal,fen,cg,x1,h_ecran-y1,x2,h_ecran-y2); }

static void ex_point(x,y)
short x,y;
{ VTO XDrawPoint(canal,fen,cg,x,h_ecran-y); }

/* Ce que l'on rajoute a plot */

static void ex_color (int c)
{ if (c < nbg && c >= 0) XSetForeground(canal,cg,assocol[c]);
  else fprintf(stderr,"Couleur no %d inexistante !\n",c);
}

static void ex_polygon (p,n)
int n;
XPoint p[];
{ int i;
  VTO
  for (i=0; i<n; i++) p[i].y= h_ecran- p[i].y;
  XFillPolygon(canal,fen,cg,p,n,Complex,CoordModeOrigin);
  for (i=0; i<n; i++) p[i].y= h_ecran- p[i].y;
}

static void ex_flushpl() {  XFlush(canal);}

int hauteur() { return (int) infen.max_height; }
int largeur() { return (int) infen.max_width; }

int nb_gris() { return nbg; }

  /*   On pourrait rajouter label, arc , ... */

static int ex_label (char *s)
{ VTO
  XDrawImageString(canal, fen, cg, poscour.x, h_ecran - poscour.y,
                   s, strlen(s)); }

static int ex_circle (x,y,r)
short x,y,r;
{ unsigned haut= 2* ((unsigned) r);
  VTO XDrawArc(canal,fen,cg, x-r, (h_ecran-y)-r, haut,haut, 0,23040); }

/* static int ex_closepl()
{ if (! traceur_ouvert)
    { fprintf(stderr,"closepl: Il n'y a pas de traceur a fermer!\n");}
  else {traceur_ouvert=0; XDestroyWindow(canal,fen); XFlush(canal);}
}
*/

/* -------------------------------------------------------------------------
 *
 *                 Le demon et la traduction fenetre virtuelle-reelle
 *
 * ----------------------------------------------------------------------- */

/* Definition des variables communes au demon et au prg utilisateur */

static int p[2];  /* Le pipe */
#define LIRE   0
#define ECRIRE 1

         /* Typage des messages entre demon et application */
static Atom TRAIT,POINt,POLYGAUNE,TRIANGLE,CARRE,
            COULEUR,ERASE,CHASSE;

static pid_t ChildId, ParentId;

/* Memorisation des requetes */

       /* Typage */

#define LINE     1
#define POINT    2
#define POLYGON  5
#define COLOR    6

typedef
  struct memcell { short typage, *vals; struct memcell *suiv; }
  *ListRequete,Requete;

static ListRequete LaMem=NULL, *DerMem= &LaMem;

static void memorise(t,p)
short t,*p;
{ *DerMem= (ListRequete) malloc (sizeof(Requete));
  (*DerMem)->typage=t;
  (*DerMem)->vals  =p;
  (*DerMem)->suiv  =NULL;    /* on ajoute en queue */
  DerMem= &((*DerMem)->suiv);
}

static void oublier()
{ ListRequete l;
  while (LaMem) {
    l=LaMem; LaMem=l->suiv;
    free(l->vals); free(l);
  }
  DerMem= &LaMem;
}

/*  Traduction PostScript de l'image memorisee */

void PsDump()
{ ListRequete l; FILE *f;
  short shaut,slarg,zero;

  f=fopen("image.ps","w");
  if (!f) {
    fprintf(stderr,"Impossible de creer image.ps dans le repertoire courant\n");
    return;
  }

#define LargDessin  ((double) ( 72*6.2677 ))
/* on veut une image de 6.2677 inches de large */
#define pixou(x,y) (int) ((((double) *(x))/ infen.max_width) * LargDessin),\
(int) ((((double) *(y))/ infen.max_width) * LargDessin)

  fprintf(f, "%%!PS\n");        /* en-tete postcript  */

  fprintf(f,"90 rotate\n");
  fprintf(f,"72 -523 translate\n");

  fprintf(f,"1 setlinewidth\n");    /* Epaisseur des traits */

  fprintf(f,"1.53 1.53 scale\n");
  shaut= (short) infen.max_height ;
  slarg= ((short) infen.max_width);
  zero=0;   /* original non ? */
  fprintf(f,"newpath\n");           /* Le fond de la fenetre */
  fprintf(f,"0 0 moveto\n");
  fprintf(f,"%d %d lineto\n",pixou(&zero,&shaut));
  fprintf(f,"%d %d lineto\n",pixou(&slarg,&shaut));
  fprintf(f,"%d %d lineto\n",pixou(&slarg,&zero));
  fprintf(f,"0.9 setgray\n");
  fprintf(f,"closepath\n");
  fprintf(f,"fill\n");
  for (l=LaMem; l; l=l->suiv) {
    short *v; v=l->vals;
    switch (l->typage) {
    case LINE:
      fprintf(f,"newpath\n");
      fprintf(f,"%d %d moveto\n",pixou(v,v+1));
      fprintf(f,"%d %d lineto\n",pixou(v+2,v+3));
      fprintf(f,"stroke\n");
      break;
    case POINT:
      fprintf(f,"newpath\n");
      fprintf(f,"%d %d moveto\n",pixou(v,v+1));
      fprintf(f,"0 0 rlineto\n");
      fprintf(f,"stroke\n");
      break;
    case POLYGON:
      fprintf(f,"newpath\n");
      { short int i;
        for ( i= *(v++)-2, fprintf(f,"%d %d moveto\n",pixou(v,v+1)), v+=2;
             i;
             v+=2, i-=2) {
          fprintf(f,"%d %d lineto\n",pixou(v,v+1));
        }
      }
      fprintf(f,"closepath\n");
      fprintf(f,"fill\n");
      break;
    case COLOR:
      fprintf(f,"%4.2lf setgray\n", 1-(((double) *(l->vals))/(nb_gris()-1)));
    }
  }
  fprintf(f,"showpage\n");
  fclose(f);
}

/* L'evenement en cours de traitement */

static XEvent evenem;
static XWindowChanges modifs;

static void tracer_polygone(p)
short *p;
{ int i,j; XPoint *t,*tt;
  j= i= *(p++);
  tt = t = (XPoint *) malloc ((i/2) * sizeof(XPoint));
  while (i) {
    afftrans(tt->x,(*p),tt->y,(*(p+1)));
    i-=2; p+=2; tt++;
  }
  ex_polygon(t,j/2);
  free(t);
}

static void rafraichir()
{
  ListRequete l;
  short *p,i;
  XPoint t[4];

  ex_erase();

  l=LaMem;
  while(l)
    { p=l->vals;
      switch(l->typage) {
      case LINE :
        ex_line(trans(*p,*(p+1)),trans(*(p+2),*(p+3)));
        break;
      case POINT:
        ex_point(trans(*p,*(p+1)));
        break;
      case POLYGON:
        tracer_polygone(p);
        break;
      case COLOR:
        ex_color((int) *p);
        break;
      }
      l=l->suiv;
    }
    ex_flushpl();
}

/*  Calcul des coefficients de visualisation   */

static int retaille(int larg,int haut) /* nouvelles largeur et hauteur */
{                             /* renvoie 1 ssi la fenetre est redimensionnee */
  int ancien_larg=infen.width;
  int ancien_haut=infen.height;

  double c1=((double) larg)/ infen.max_width,
         c2=((double) haut)/ infen.max_height;

  if ((ancien_larg==larg) && (ancien_haut==haut)) return 0;

  coef = ( (c1>c2) ? c2 : c1 );
  dx   = ( (c1>c2) ? (larg-coef*infen.max_width)/2 : 0 );
  dy   = ( (c1>c2) ? 0 : (haut-coef*infen.max_height)/2 );
  infen.width=larg;
  infen.height=haut;
  return 1;
}

#include <sys/wait.h>

 static void application_terminee()
{ /* union wait st; */
  exit(0);
}

static int pere()
{
  int i,nb ;
  KeySym touche;
  char frappe[10];
  extern GC cg;
  int flags;

  close (p[ECRIRE]); /*fermeture du pipe en ecriture */

 signal(SIGCHLD,application_terminee);

  /* Selection des evenements en entree */
  XSelectInput(canal,fen,
                ButtonPressMask   /* pour controler les zooms, et ? presser bouton */
               |KeyPressMask      /* debug */
               |ExposureMask      /* rafraichissement */
               |ButtonReleaseMask /* pour controler les zooms, et ? presser bouton */
               |StructureNotifyMask)  /* redimensionnement de la fenetre */ ;


  while (1)
    {
      XNextEvent(canal,&evenem);

      switch(evenem.type)
        {
        case Expose :

          if (evenem.xexpose.count == 0)
            {
              rafraichir();
            }
          break;

          /* Prise en compte des modifications du clavier  */
        case MappingNotify :   /* QU'EST-CE QUE C'EST ? */
          printf("\n Evenement MappingNotify recu !\n\n");
          XRefreshKeyboardMapping(&(/* (XMappingEvent)*/  evenem));
          break;
        case KeyPress :
          i=XLookupString(&evenem,frappe,10,&touche,0);
          if(i==1 && frappe[0]=='q')
            { kill(ChildId,SIGKILL); exit(0); }
          if(i==1 && frappe[0]=='m')
            { static ListRequete requete; int i,j;
              printf("\n");
              for (requete=LaMem,i=j=0; requete;
                   (++i==10 ? (printf("\n"),i=0): printf("\t")),
                   (++j==100 ? (printf("\n"),j=0): 0),
                   (requete=requete->suiv))
                printf("%d",requete->typage);
              printf("\n");
            }
          if(i==1 && frappe[0]=='P')
                { PsDump();
                  printf("image.ps crii dans le repertoire courant\n");}
          break;

        case ClientMessage:
          { Atom mt;
            mt= evenem.xclient.message_type;
            if ( mt == TRAIT )
              { short *p; p=evenem.xclient.data.s;
                     /*   realisation du trait  */
                ex_line(trans(*p,*(p+1)),trans(*(p+2),*(p+3)));
                     /*   memorisation du trait */
                { static short *param; int i;
                  param = (short *)malloc(4*sizeof(short));
                  for (i=0; i<4 ; *(param+i)= *(p+i),i++);
                  memorise(LINE,param);
                }
              }
            else {
              if (mt == COULEUR) {
                           /*  realisation du choix de couleur  */
                ex_color((int) evenem.xclient.data.s[0]);
                       /*  memorisation du changement de couleur */
                { static short int *LaCouleur;
                  LaCouleur =  malloc(sizeof(short));
                  *LaCouleur = evenem.xclient.data.s[0];
                  memorise(COLOR, LaCouleur);
                }}
              else {
                if (mt == ERASE )
                  {
                          /* effacement */
                    ex_erase();
                         /* memorisation de l'effacement */
                    oublier();
                  }
                else {
                  if (mt == CHASSE) ex_flushpl();
                  else {
                    if (mt == POINt) {
                             /*  realisation du tracage du point  */
                      ex_point(trans(evenem.xclient.data.s[0],
                                     evenem.xclient.data.s[1]));
                             /*  memorisation du point */
                      { static short *LePoint;
                        LePoint = (short *) malloc(2*sizeof(short));
                        *LePoint = evenem.xclient.data.s[0];
                        *(LePoint+1) = evenem.xclient.data.s[1];
                        memorise(POINT,LePoint);
                      }}
                    else {
                      if (mt == TRIANGLE) {
                        int n,xy[2];
                        short *p,*ps,*pp;
                        pp= p = (short *) malloc(7*sizeof(short));
                        *(p++)=6;
                        for (n=6, ps=evenem.xclient.data.s; n; n-=2)
                          { xy[0]= *(ps++); xy[1]= *(ps++);
                            *(p++)=xy[0]; *(p++)=xy[1];
                          }
                        tracer_polygone(pp);
                        memorise(POLYGON,pp); }
                      else {
                        if (mt == CARRE) {
                          int n,xy[2];
                          short *p,*ps,*pp;
                          pp= p = (short *) malloc(9*sizeof(short));
                          *(p++)=8;
                          for (n=8,ps=evenem.xclient.data.s; n; n-=2)
                            { xy[0]= *(ps++); xy[1]= *(ps++);
                              *(p++)=xy[0]; *(p++)=xy[1];
                            }
                          memorise(POLYGON,pp);
                          tracer_polygone(pp);
                        }
                        else {
                          if (mt == POLYGAUNE) {
                            int n,xy[2];
                            short *po,*pp;
                            read(p[LIRE],&n,sizeof(int));
                            pp=po = (short *) malloc((n+1)*sizeof(short));
                            *po=n;
                            for (po++; n; n-=2) {
                              read(p[LIRE],xy,2*sizeof(int));
                              *(po++)=xy[0]; *(po++)=xy[1];
                            }
                            memorise(POLYGON,pp);
                            tracer_polygone(pp);
                          }
                         /* else printf("Message bizarre : %d\n",evenem.xclient.message_type); enleve le 27.11.97 */
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          break;

        case ConfigureNotify :
          if (retaille( evenem.xconfigure.width, evenem.xconfigure.height))
            { ex_erase(); }
          break;

        default: /* fprintf(stderr,"Evenement inconnu : %d\n",evenem.type)*/ ;
        }
      fflush(stdout);  /* Pour DEBUG */
    }
}

/* -------------------------------------------------------------------------
 *
 *                 L'extension de Plot
 *
 * ----------------------------------------------------------------------- */

static Display *lanac;    /* canal perso */
static DejaOpen=0;

void openpl()
{ int pid ;

  if (DejaOpen) { XMapRaised(lanac,fen); return; }
  DejaOpen=1;

  ex_openpl();

        TRAIT       = XInternAtom(canal,"__TRAIT",False);
        POINt       = XInternAtom(canal,"__POINt",False);
        POLYGAUNE   = XInternAtom(canal,"__POLYGAUNE",False);
        TRIANGLE    = XInternAtom(canal,"__TRIANGLE",False);
        CARRE       = XInternAtom(canal,"__CARRE",False);
        COULEUR     = XInternAtom(canal,"__COULEUR",False);
        ERASE       = XInternAtom(canal,"__ERASE",False);
        CHASSE      = XInternAtom(canal,"__CHASSE",False);

        pipe(p);      /* Creation du pipe */
        ParentId = getpid();
        pid = ChildId = fork();
        if (pid != 0) pere();   /* boucle de gestion des evenements
                                   sur la fenetre de visualisation */

        ChildId=getpid();
        close(p[LIRE]);
        lanac=XOpenDisplay("");
        if (!lanac) { printf("Open display impossible \n"); exit(1); }

        XSelectInput(lanac,fen,0ul);

        /* Puis on continue apres l'appel d'openpl() */
      }

/* Envoie du message courant */

static XEvent ev;  /* C'est lui le courant ! */

static void envoicour()
{ int s; static nb_envoi=0;

  ev.type               = ClientMessage;
  ev.xany.send_event    = True;           /* Comme ce doit etre */
  ev.xclient.format     = 16;
  s = XSendEvent(lanac,fen,True,0, &ev);
}

static int AbscisseCourante,OrdonneeCourante;

void move(x,y)
 int x,y;
{ AbscisseCourante=x; OrdonneeCourante=y; }

void line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
  /* Envoie d'un message de type ClientMessage avec
     - type: c'est une ligne
     - les coordonnees du trait
     */

  ev.xclient.message_type = TRAIT;
  ev.xclient.data.s[0]  = x1;
  ev.xclient.data.s[1]  = y1;
  ev.xclient.data.s[2]  = x2;
  ev.xclient.data.s[3]  = y2;

  envoicour();
  move(x2,y2);
}

void cont(x,y)
int x,y;
{ line(AbscisseCourante,OrdonneeCourante,x,y); }

void point(x,y)
int x,y;
{
  ev.xclient.message_type = POINt;
  ev.xclient.data.s[0]  = ((short) x);
  ev.xclient.data.s[1]  = ((short) y);

  envoicour();
}

void dot()
{ point(AbscisseCourante,OrdonneeCourante); }

void polygon(posit,n)
int posit[];
int n;
{
  switch (n) {
  case 6:
    ev.xclient.message_type = TRIANGLE;
    for (; n-- ; ) ev.xclient.data.s[n]  = ((short) posit[n]);
    break;
  case 8:
    ev.xclient.message_type = CARRE;
    for (; n-- ; ) ev.xclient.data.s[n]  = ((short) posit[n]);
    break;
  default:
    ev.xclient.message_type = POLYGAUNE;
    envoicour();
    XFlush(lanac);
    write(p[ECRIRE],&n,sizeof(int));
    write(p[ECRIRE],posit,n*sizeof(int));
    return;
  }

  envoicour();
}

void color(couleur)
int couleur;
{
  ev.xclient.message_type = COULEUR;
  ev.xclient.data.s[0]  = ((short) couleur);

  envoicour();
}

void erase()
{
  ev.xclient.message_type = ERASE;

  envoicour();
}

void  flushpl()
{
  ev.xclient.message_type = CHASSE;

  envoicour();

  XFlush(lanac); /* on suppose que les requetes sont realisees des reception */
}

void closepl()
{ XUnmapWindow(lanac,fen); XFlush(lanac); }

