#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>


int main(int argc, char *argv[]) 
{
	int status;
	pid_t pid = fork();

	printf("Process start to fork\n");
	
	if(pid == -1)
	{
		perror("fork failed\n");
		exit(1);
	}
	else
	{
		//child process
		if(pid == 0)
		{
			printf("I am the child process, my pid = %d\n", getpid());
			printf("Child process start to execute test program:\n");

			//意味わかんない
			char *arg[argc];

			for(int i=0;i<argc-1;i++){
				arg[i]=argv[i+1];
			}
			arg[argc-1]=NULL;

			// execute test file
			execve(arg[0],arg, NULL);

			exit(1);


		}
		else	// parent process
		{
			printf("I am the parent process, my pid = %d\n", getpid());
			waitpid(pid, &status, WUNTRACED);

			printf("Parent process receiving the SIGCHLD signal\n");

			//normal execution
			if(WIFEXITED(status))
				printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
			//encounter failure
			else if (WIFSIGNALED(status))
			{
				printf("Execution failed with signal: %d\n", WTERMSIG(status));

				switch (WTERMSIG(status))
				{
					case 1:
						printf("Execution failed with hang up signal\n");
						break;

					case 2:
						printf("Execution failed with interrupt signal\n");
						break;

					case 3:
						printf("Execution failed with quit signal\n");
						break;

					case 4:
						printf("Execution failed with illegal signal\n");
						break;

					case 5:
						printf("Execution failed with trap signal\n");
						break;

					case 6:
						printf("Execution failed with abort signal\n");
						break;

					case 7:
						printf("Execution failed with bus signal\n");
						break;

					case 8:
						printf("Execution failed with floating point exception signal\n");
						break;

					case 9:
						printf("Execution failed with kill signal\n");
						break;

					case 10:
						printf("Execution failed with bus signal\n");
						break;

					case 11:
						printf("Execution failed with invaild memory reference signal\n");
						break;

					case 12:
						printf("Execution failed with invaild system reference signal\n");
						break;

					case 13:
						printf("Execution failed with pipeline error signal\n");
						break;

					case 14:
						printf("Execution failed with alarm signal\n");
						break;

					case 15:
						printf("Execution failed with termaniation signal\n");
						break;

					default:
						printf("Invild signal received!\n");
						break;
				}
			}
			else if (WIFSTOPPED(status))
			{
				printf("CHILD PROCESS STOPPED\n");
			}
			else
			{
				printf("CHILD PROCESS CONTINUED\n");
			}
		}
	}
	


	

}