#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	/* fork a child process */

	/* execute test program */

	/* wait for child process terminates */

	/* check child process'  termination status */

	pid_t pid;
	int status;

	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		//Child process
		if (pid == 0) {
			char *arg[argc];

			printf("I'm the Child Process, my pid = %d\n",
			       getpid());

			for (int i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("Child process starts to execute test program:\n");
			execve(arg[0], arg, NULL);
		}

		//Parent process
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());

			waitpid((pid_t)-1, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");

			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				printf("child process get ");

				switch (WTERMSIG(status)) {
				case 6:
					printf("SIGABRT");
					break;
				case 14:
					printf("SIGALRM");
					break;
				case 7:
					printf("SIGBUS");
					break;
				case 8:
					printf("SIGFPE");
					break;
				case 1:
					printf("SIGHUP");
					break;
				case 4:
					printf("SIGILL");
					break;
				case 2:
					printf("SIGINT");
					break;
				case 9:
					printf("SIGKILL");
					break;
				case 13:
					printf("SIGPIPE");
					break;
				case 3:
					printf("SIGQUIT");
					break;
				case 11:
					printf("SIGSEGV");
					break;
				case 15:
					printf("SIGTERM");
					break;
				case 5:
					printf("SIGTRAP");
					break;
				default:
					break;
				}
				printf(" signal\n");
			} else if (WIFSTOPPED(status)) {
				printf("child process get SIGSTOP signal\n");

			} else {
				printf("child process continue\n");
			}

			exit(0);
		}
	}

	return 0;
}
