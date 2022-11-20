#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	printf("Process start to fork\n");
	printf("I'm the Parent Process, my pid = %d\n", getpid());

	/* fork a child process */
	pid_t pid = fork();

	if (pid == -1) {
		perror("fork");
	}
	/* execute test program */
	else if (pid == 0) {
		printf("I'm the Child Process, my pid = %d\n", getpid());
		char *arg[argc];
		for (int i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;
		printf("Child process start to execute test program:\n");
		execve(arg[0], arg, NULL);
	}
	/* wait for child process terminates */
	else {
		int status;
		waitpid(pid, &status, 0);
		printf("Parent process receives SIGCHLD signal\n");
		/* check child process' termination status */
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("child process get ");
			char **str_signal;
			switch (WTERMSIG(status)) {
			case SIGABRT:
				printf("SIGABRT");
				break;
			case SIGALRM:
				printf("SIGALRM");
				break;
			case SIGBUS:
				printf("SIGBUS");
				break;
			case SIGFPE:
				printf("SIGFPE");
				break;
			case SIGHUP:
				printf("SIGHUP");
				break;
			case SIGILL:
				printf("SIGILL");
				break;
			case SIGINT:
				printf("SIGINT");
				break;
			case SIGKILL:
				printf("SIGKILL");
				break;
			case SIGPIPE:
				printf("SIGPIPE");
				break;
			case SIGQUIT:
				printf("SIGQUIT");
				break;
			case SIGSEGV:
				printf("SIGSEGV");
				break;
			case SIGSTOP:
				printf("SIGSTOP");
				break;
			case SIGTERM:
				printf("SIGTERM");
				break;
			case SIGTRAP:
				printf("SIGTRAP");
				break;
			}
			printf(" signal\n");
		}
		exit(0);
	}
}
