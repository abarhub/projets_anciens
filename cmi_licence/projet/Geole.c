#include<stdio.h>
#include<math.h>

int main(int argc, char ** argv)
{
  int i;
  double nb;
  double a=1,b=3,c=2,d=18;
  double theta=M_PI/3,phi;

  if(argc!=2)
    {
      fprintf(stderr,"il faut donner le nb de pales en parametre\n");    
      return 0;
    }

  nb=(double)(atoi(argv[1]));

  printf("#eolienne\n");
  printf("%d %d %d\n",(int)(nb*4.0),(int)(nb*4.0),(int)nb);

  printf("#sommets\n");
  for(i=0;i<nb;i++)
    {
      phi=2*i*M_PI/nb;
      printf("%d %.4f %.4f %.4f 1\n",4*i+1,
	     c*cos(phi) - a*cos(theta)*sin(phi),
	     c*sin(phi) + a*cos(theta)*cos(phi),
	                  a*sin(theta));
      printf("%d %.4f %.4f %.4f 1\n",4*i+2,
	     c*cos(phi) + a*cos(theta)*sin(phi),
	     c*sin(phi) - a*cos(theta)*cos(phi),
	                - a*sin(theta));
      printf("%d %.4f %.4f %.4f 1\n",4*i+3,  
	     d*cos(phi) + b*cos(theta)*sin(phi),
	     d*sin(phi) - b*cos(theta)*cos(phi),
	                - b*sin(theta));
      printf("%d %.4f %.4f %.4f 1\n",4*i+4,
	     d*cos(phi) - b*cos(theta)*sin(phi),
	     d*sin(phi) + b*cos(theta)*cos(phi),
	                  b*sin(theta));
    }
  
  printf("#aretes\n");
  for(i=0;i<nb;i++)
    {
      printf("%d %d %d 1\n",4*i+1,
	     4*i+1,4*i+2);
      printf("%d %d %d 1\n",4*i+2,
	     4*i+2,4*i+3);
      printf("%d %d %d 1\n",4*i+3,
	     4*i+3,4*i+4);
      printf("%d %d %d 1\n",4*i+4,
	     4*i+4,4*i+1);
    }
      
  printf("#faces\n");
  for(i=0;i<nb;i++)
    {
      printf("%d %d %d %d %d %d\n",i+1,
	     4*i+1,4*i+2,
	     4*i+3,4*i+4,
	     4*i+1);
    }
}
