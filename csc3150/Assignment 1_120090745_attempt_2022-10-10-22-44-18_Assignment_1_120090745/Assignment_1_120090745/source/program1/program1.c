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

	if (pid < 0) {
		printf("FORK ERROR\n");
		exit(0);
	} else {
		if (pid > 0) {
			printf("I'm the parent process, my pid = %d\n",
			       getpid());
		} else if (pid == 0) {
			printf("I'm the child process, my pid = %d\n",
			       getpid());
		}
	}

	/* execute test program */
	if (pid == 0) {
		int i;
		char *arg[argc];

		for (i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;

		printf("Child process start to execute test program:\n");
		execve(arg[0], arg, NULL);
	}

	/* wait for child process terminates */
	else if (pid > 0) {
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");

		/* check child process'  termination status */
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			if (WTERMSIG(status) == 6) {
				printf("child process get SIGABRT signal\n");
			} else if (WTERMSIG(status) == 14) {
				printf("child process get SIGALRM signal\n");
			} else if (WTERMSIG(status) == 7) {
				printf("child process get SIGBUS signal\n");
			} else if (WTERMSIG(status) == 8) {
				printf("child process get SIGFPE signal\n");
			} else if (WTERMSIG(status) == 1) {
				printf("child process get SIGHUP signal\n");
			} else if (WTERMSIG(status) == 4) {
				printf("child process get SIGILL signal\n");
			} else if (WTERMSIG(status) == 2) {
				printf("child process get SIGINT signal\n");
			} else if (WTERMSIG(status) == 9) {
				printf("child process get SIGKILL signal\n");
			} else if (WTERMSIG(status) == 13) {
				printf("child process get SIGPIPE signal\n");
			} else if (WTERMSIG(status) == 3) {
				printf("child process get SIGQUIT signal\n");
			} else if (WTERMSIG(status) == 11) {
				printf("child process get SIGSEVG signal\n");
			} else if (WTERMSIG(status) == 15) {
				printf("child process get SIGTERM signal\n");
			} else if (WTERMSIG(status) == 5) {
				printf("child process get SIGTRAP signal\n");
			} else {
				printf("child Process terminated with status = %d\n",
				       WTERMSIG(status));
			}
			/*printf("Child Process terminated with STATUS = %d\n", WTERMSIG(status));*/
		} else if (WIFSTOPPED(status)) {
			printf("child process get SIGSTOP signal\n");
			/*printf("Child Process stopped with STATUS = %d\n", WSTOPSIG(status));*/
		} else {
			printf("child Process continued\n");
		}

		exit(0);
	}

	return 0;
}
