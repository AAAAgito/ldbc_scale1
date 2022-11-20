#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char* argv[]) {

	pid_t pid;
	int status;

	/* fork a child process */

	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else
	{
		if (pid == 0) //child
		{
			printf("I'm the Child Process, my pid = %d\n", getpid());
			printf("Child process start to execute test program:\n");
			printf("----------CHILD PROCESS START------------\n");

			//code copied from tut ppt
			int i;
			char* arg[argc];
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			/* execute test program */
			execve(arg[0], arg, NULL);

			perror("execve");
			exit(EXIT_FAILURE);
		}

		else // parent process
		{
			printf("I'm the Parent Process, my pid = %d\n", getpid());

			/* wait for child process terminates */
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives the SIGCHLD signal\n");

			if (WIFEXITED(status))
			{
				printf("This is the normal program\n");
				printf("----------CHILD PROCESS END------------\n")
				printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
			}

			else if (WIFSIGNALED(status))
			{
				else if (WTERMSIG(status) == 1)
				{
					printf("This is the SIGHUP program\n")
						printf("child process get SIGHUP signal\n");
				}
				else if (WTERMSIG(status) == 2)
				{
					printf("This is the SIGINT program\n")
						printf("child process get SIGINT signal\n");
				}
				else if (WTERMSIG(status) == 3)
				{
					printf("This is the SIGQUIT program\n")
					printf("child process get SIGQUIT signal\n");
				}
				else if (WTERMSIG(status) == 4)
				{
						printf("This is the SIGILL program\n")
						printf("child process get SIGILL signal\n");
				}
				else if (WTERMSIG(status) == 5)
				{
					printf("This is the SIGTRAP program\n")
					printf("child process get SIGTRAP signal\n");
				}
				if (WTERMSIG(status) == 6)
				{
					printf("This is the SIGABRT program\n");
					printf("child process get SIGABRT signal\n");
				}
				else if (WTERMSIG(status) == 7)
				{
					printf("This is the SIGBUS program\n")
						printf("child process get SIGBUS signal\n");
				}
				else if (WTERMSIG(status) == 8)
				{
					printf("This is the SIGFPE program\n")
						printf("child process get SIGFPE signal\n");
				}
				else if (WTERMSIG(status) == 9)
				{
					printf("This is the SIGKILL program\n")
						printf("child process get SIGKILL signal\n");
				}
				else if (WTERMSIG(status) == 11)
				{
					printf("This is the SIGSEGV program\n")
						printf("child process get SIGSEGV signal\n");
				}
				else if (WTERMSIG(status) == 13)
				{
					printf("This is the SIGPIPE program\n")
						printf("child process get SIGPIPE signal\n");
				}
				else if (WTERMSIG(status) == 14)
				{
					printf("This is the SIGALRM program\n");
					printf("child process get SIGALRM signal\n");
				}
				else if (WTERMSIG(status) == 15)
				{
					printf("This is the SIGTERM program\n")
					printf("child process get SIGTERM signal\n");;
				}
			}
			else if (WIFSTOPPED(status))
			{
				printf("This is the SIGSTOP program\n")
				printf("child process get SIGSTOP signal\n");
			}
			else
			{
				printf("try again.\n");
			}
			exit(1);
		}

	}


}