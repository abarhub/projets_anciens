#include<pthread.h>

#define M (2)
#define N1 (1)
#define N2 (2)

#define CONDENSE(X,Y)  ( ((X)<<8) + (Y))
#define SEPARE(X,Y,Z)  { (X)=(Z)>>8; (Y)=(Z)&(0xFF);}

// matrice de type (no_ligne)*(no_col)
int mat1[N1][M],mat2[M][N2],mat_res[N1][N2];

void affiche_mat(int *mat,int max_ligne,int max_col)
{
  int i,j;
  
  for(i=0;i<max_ligne;i++)
    {
      for(j=0;j<max_col;j++)
	{
	  printf(" %d ",*(mat+max_ligne*j+i));
	}
      printf("\n");
    }
}

void rempli(int *mat,int max_ligne,int max_col)
{
  int i,j;
  
  for(i=0;i<max_ligne;i++)
    {
      for(j=0;j<max_col;j++)
	{
	  *(mat+max_ligne*j+i)=rand()%10;
	}
    }
}

pthread_mutex_t mutex;

void *f(void *arg)
{
  int i,j,x,y;
  
  pthread_mutex_lock(&mutex);
  
  SEPARE(x,y,(int)arg);
  printf("thread cree (%d %d %d)\n",x,y,(int)arg);
  
  printf("mat_res[%d][%d]=",x,y);
  mat_res[x][y]=0;
  for(i=0;i<M;i++)
    //for(j=0;j<M;j++)
      {
	if(x<0||x>=N1)
	  printf("Erreur: x incorrecte %d\n",x);
	if(y<0||y>=N2)
	  printf("Erreur: y incorrecte %d\n",y);
	if(i<0||i>=M)
	  printf("Erreur: i incorrecte %d\n",i);
	mat_res[x][y]+=mat1[x][i]*mat2[y][i];
	if(i>0)
	  printf("+");
	printf("(%d,%d)%d*(%d,%d)%d",x,i,mat1[x][i],i,y,mat2[i][y]);
      }
  printf("=%d\n",mat_res[x][y]);
  pthread_mutex_unlock(&mutex);
}

int main()
{
  int i,j,k;
  pthread_t *pth;
  
  i=3;j=4;
  printf("avant i=%d j=%d\n",i,j);
  k=CONDENSE(i,j);
  i=0;j=0;
  printf("pendant i=%d j=%d\n",i,j);
  SEPARE(i,j,k);
  printf("apres i=%d j=%d\n",i,j);
  
  pthread_mutex_init(&mutex,NULL);
  srand(time(NULL));
  
  rempli((int*)mat1,N1,M);
  rempli((int*)mat2,M,N2);
  
  pth=(pthread_t*)calloc(sizeof(pthread_t),N2*N1);
  for(i=0;i<N2;i++)
    {
      for(j=0;j<N1;j++)
	{
	  k=CONDENSE(i,j);
	  pthread_create(pth+i*N2+j,NULL,f,(void*)CONDENSE(j,i));
	}
    }
  
  for(i=0;i<N2;i++)
    {
      for(j=0;j<N1;j++)
	{
	  pthread_join(*(pth+i*N2+j),NULL);
	}
    }
  pthread_mutex_destroy(&mutex);
  free(pth);
  
  printf("mat1:\n");
  affiche_mat((int*)mat1,N1,M);
  printf("mat2:\n");
  affiche_mat((int*)mat2,M,N2);
  printf("mat_res (mat2*mat1):\n");
  affiche_mat((int*)mat_res,N1,N2);
}
