#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;

	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork();
	if (pid < 0) {
		printf("Fork error\n");
	} else if (pid == 0) {
		int i;
		char *arg[argc];
		sleep(0.5);
		printf("I'm the Child Process, my pid = %d\n", getpid());
		for (i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;
		printf("Child process start to execute test program:\n");

		/* execute test program */
		execve(arg[0], arg, NULL);
		printf("Continue to run original Child process!\n");
		perror("execve");
		exit(EXIT_FAILURE);
	} else {
		printf("I'm the Parent Process, my pid = %d\n", getpid());

		/* wait for child process terminates */
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");

		/* check child process'  termination status */
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			if (WTERMSIG(status) == 1) {
				printf("Child process get SIGHUP signal\n");
			} else if (WTERMSIG(status) == 2) {
				printf("Child process get SIGINT signal\n");
			} else if (WTERMSIG(status) == 3) {
				printf("Child process get SIGQUIT signal\n");
			} else if (WTERMSIG(status) == 4) {
				printf("Child process get SIGILL signal\n");
			} else if (WTERMSIG(status) == 5) {
				printf("Child process get SIGTRAP signal\n");
			} else if (WTERMSIG(status) == 6) {
				printf("Child process get SIGABRT signal\n");
			} else if (WTERMSIG(status) == 7) {
				printf("Child process get SIGBUS signal\n");
			} else if (WTERMSIG(status) == 8) {
				printf("Child process get SIGFPE signal\n");
			} else if (WTERMSIG(status) == 9) {
				printf("Child process get SIGKILL signal\n");
			} else if (WTERMSIG(status) == 11) {
				printf("Child process get SIGSEGV signal\n");
			} else if (WTERMSIG(status) == 13) {
				printf("Child process get SIGPIPE signal\n");
			} else if (WTERMSIG(status) == 14) {
				printf("Child process get SIGALRM signal\n");
			} else if (WTERMSIG(status) == 15) {
				printf("Child process grt SIGTERM signal\n");
			}
		} else if (WIFSTOPPED(status)) {
			printf("Child process get SIGSTOP signal\n");
		} else {
			printf("CHILD PROCESS CONTINUED\n");
		}
		exit(0);
	}
	return 0;
}
