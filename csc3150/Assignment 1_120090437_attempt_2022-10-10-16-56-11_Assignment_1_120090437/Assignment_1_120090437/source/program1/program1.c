#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;

	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		// Child process
		if (pid == 0) {
			int i;
			char *arg[argc];

			printf("I'm the Child Process, my pid = %d\n",
			       getpid());

			// argv -> arg
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			/* execute test program */
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);

			// exception if child process is not replaced
			printf("Continue to run original child process!\n");
			perror("execve");
			exit(EXIT_FAILURE);
		}

		// Parent process
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());

			/* wait for child process terminates */
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");

			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				switch (WTERMSIG(status)) {
				case SIGABRT:
					printf("Child process gets SIGABRT signal\n");
					break;
				case SIGALRM:
					printf("Child process gets SIGALRM signal\n");
					break;
				case SIGBUS:
					printf("Child process gets SIGBUS signal\n");
					break;
				case SIGFPE:
					printf("Child process gets SIGFPE signal\n");
					break;
				case SIGHUP:
					printf("Child process gets SIGHUP signal\n");
					break;
				case SIGILL:
					printf("Child process gets SIGILL signal\n");
					break;
				case SIGINT:
					printf("Child process gets SIGINT signal\n");
					break;
				case SIGKILL:
					printf("Child process gets SIGKILL signal\n");
					break;
				case SIGPIPE:
					printf("Child process gets SIGPIPE signal\n");
					break;
				case SIGQUIT:
					printf("Child process gets SIGQUIT signal\n");
					break;
				case SIGSEGV:
					printf("Child process gets SIGSEGV signal\n");
					break;
				case SIGTERM:
					printf("Child process gets SIGTERM signal\n");
					break;
				case SIGTRAP:
					printf("Child process gets SIGTRAP signal\n");
					break;
				default:
					printf("Child process gets unknown signal\n");
					break;
				}
			} else if (WIFSTOPPED(status)) {
				printf("Child process gets SIGSTOP signal\n");
			} else {
				printf("CHILD PROCESS CONTINUED\n");
			}

			exit(0);
		}
	}

	/* check child process'  termination status */
}
