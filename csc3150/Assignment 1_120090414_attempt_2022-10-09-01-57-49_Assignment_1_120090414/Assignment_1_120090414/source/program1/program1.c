#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
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

			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			/* execute test program */
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);

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
			/* check child process'  termination status */
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSTOPPED(status)) {
				printf("child process get SIGSTOP signal\n");
			} else if (WIFSIGNALED(status)) {
				if (WTERMSIG(status) == SIGABRT) {
					printf("child process get SIGABRT signal\n");
				} else if (WTERMSIG(status) == SIGALRM) {
					printf("child process get SIGALRM signal\n");
				} else if (WTERMSIG(status) == SIGBUS) {
					printf("child process get SIGBUS signal\n");
				} else if (WTERMSIG(status) == SIGFPE) {
					printf("child process get SIGFPE signal\n");
				} else if (WTERMSIG(status) == SIGHUP) {
					printf("child process get SIGHUP signal\n");
				} else if (WTERMSIG(status) == SIGILL) {
					printf("child process get SIGILL signal\n");
				} else if (WTERMSIG(status) == SIGINT) {
					printf("child process get SIGINT signal\n");
				} else if (WTERMSIG(status) == SIGKILL) {
					printf("child process get SIGKILL signal\n");
				} else if (WTERMSIG(status) == SIGPIPE) {
					printf("child process get SIGPIPE signal\n");
				} else if (WTERMSIG(status) == SIGQUIT) {
					printf("child process get SIGQUIT signal\n");
				} else if (WTERMSIG(status) == SIGSEGV) {
					printf("child process get SIGSEGV signal\n");
				} else if (WTERMSIG(status) == SIGTERM) {
					printf("child process get SIGTERM signal\n");
				} else if (WTERMSIG(status) == SIGTRAP) {
					printf("child process get SIGTRAP signal\n");
				}
			}
			exit(0);
		}
	}
	return 0;
}
