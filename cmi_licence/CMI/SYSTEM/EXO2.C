#include<stdio.h>
#include <unistd.h>

int main()
{
	int pid;
	pid=fork();
	switch(pid)
	{
		case -1: printf("Erreur");break;
		case 0:/* fils1 */
			printf("fils1 pid=%d pere pid=%d\n",getpid(),getppid());
			switch(fork())
			{
				case -1:/* Erreur */ 
					printf("Erreur");break;
				case 0: /* petit fils 1 */
					printf("petit fils1 pid=%d pere pid=%d\n",getpid(),getppid());
					break;
				default:/* fils1 */
					switch(fork())
					{
						case -1:/* Erreur */ 
							printf("Erreur");break;
						case 0: /* petit fils 2 */
							printf("petit fils2 pid=%d pere pid=%d\n",
								getpid(),getppid());
							break;
						default:/* fils1 */
						break;
					}
					break;
			}
			break;
		default:/* pere */
		switch(fork())
		{
			case -1: 
				/* erreur */
				printf("Erreur");break;
			case 0:/* fils2 */
				printf("fils2 pid=%d pere pid=%d\n",getpid(),getppid());
				break;
			default:/* pere */
				switch(fork())
				{
					case -1: printf("Erreur");break;
					case 0:/* fils3 */
						printf("fils3 pid=%d pere pid=%d\n",
							getpid(),getppid());
						break;
					default:/* pere */
						printf("pere pid=%d pere pid=%d\n",
							getpid(),getppid());
						break;
				}
				break;
		}
	}
	
	return 0;
}