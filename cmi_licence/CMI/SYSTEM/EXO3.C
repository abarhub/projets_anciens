#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	pid=fork();
	switch(pid)
	{
		case -1: printf("Erreur");break;
		case 0:/* fils1 */
			printf("fils1 pid=%d pere pid=%d\n",(int)getpid(),(int)getppid());
			sleep(5);
			switch(fork())
			{
				case -1:/* Erreur */ 
					printf("Erreur");break;
				case 0: /* petit fils 1 */
					printf("petit fils1 pid=%d pere pid=%d\n",
						(int)getpid(),(int)getppid());
					break;
				default:/* fils1 */
					switch(fork())
					{
						case -1:/* Erreur */ 
							printf("Erreur");break;
						case 0: /* petit fils 2 */
							printf("petit fils2 pid=%d pere pid=%d\n",
								(int)getpid(),(int)getppid());
							break;
						default:/* fils1 */
							wait(NULL);wait(NULL);
						break;
					}
					break;
			}
			break;
		default:/* pere */
		//wait(NULL);
		switch(fork())
		{
			case -1: 
				/* erreur */
				printf("Erreur");break;
			case 0:/* fils2 */
				printf("fils2 pid=%d pere pid=%d\n",(int)getpid(),(int)getppid());
				break;
			default:/* pere */
				switch(fork())
				{
					case -1: printf("Erreur");break;
					case 0:/* fils3 */
						printf("fils3 pid=%d pere pid=%d\n",
							(int)getpid(),(int)getppid());
						break;
					default:/* pere */
						printf("pere pid=%d pere pid=%d\n",
							(int)getpid(),(int)getppid());
						wait(NULL);wait(NULL);wait(NULL);
						break;
				}
				break;
		}
	}
	
	return 0;
}