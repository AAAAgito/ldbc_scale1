#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	/* fork a child process */
	printf("Process start to fork\n");
	int status;
	pid_t pid = fork();
	/* execute test program */
	if (-1 == pid) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (0 == pid) {
		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");
		char *arg[argc];
		for (int i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;
		char *const envp[] = {
			NULL,
		};
		execve(arg[0], arg, envp);
		// execve failure
		printf("Continue to run original child process!\n");
		perror("execve");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		printf("I'm the Parent Process, my pid = %d\n", getpid());
		/* wait for child process terminates */
		waitpid(-1, &status, WUNTRACED);
		printf("Parent process receives SIGCHILD signal\n");
		/* check child process'  termination status */
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			switch (WTERMSIG(status)) {
			case SIGABRT:
				printf("child process get SIGABRT signal\n");
				break;
			case SIGALRM:
				printf("child process get SIGALRM signal\n");
				break;
			case SIGTERM:
				printf("child process get SIGTERM signal\n");
				break;
			case SIGQUIT:
				printf("child process get SIGQUIT signal\n");
				break;
			case SIGKILL:
				printf("child process get SIGKILL signal\n");
				break;
			case SIGBUS:
				printf("child process get SIGBUS signal\n");
				break;
			case SIGFPE:
				printf("child process get SIGFPE signal\n");
				break;
			case SIGHUP:
				printf("child process get SIGHUP signal\n");
				break;
			case SIGILL:
				printf("child process get SIGILL signal\n");
				break;
			case SIGINT:
				printf("Child process get SIGINT signal\n");
				break;
			case SIGPIPE:
				printf("child process get SIGPIPE signal\n");
				break;
			case SIGSEGV:
				printf("child process get SIGSEGV signal\n");
				break;
			case SIGTRAP:
				printf("child process get SIGTRAP signal\n");
				break;
			default:
				break;
			}
		} else if (WIFSTOPPED(status)) {
			printf("child process get SIGSTOP signal\n");
		} else {
			printf("CHILD PROCESS CONTINUED\n");
		}
		exit(EXIT_SUCCESS);
	}
}
