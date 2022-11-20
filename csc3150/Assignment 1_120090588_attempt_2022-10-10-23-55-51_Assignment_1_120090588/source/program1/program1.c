#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	/* fork a child process */

	/* execute test program */

	/* wait for child process terminates */

	/* check child process'  termination status */
	printf("Process start to fork\n");
	int status;
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		sleep(1);
		int index;
		char *arg[argc];
		for (index = 0; index < argc - 1; index++) {
			arg[index] = argv[index + 1];
		}
		arg[argc - 1] = NULL;
		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");
		execve(arg[0], arg, NULL);
		perror("execve");
		exit(EXIT_FAILURE);
	} else {
		printf("I'm the Parent Process, my pid = %d\n", getpid());
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = 0\n");
		} else if (WIFSIGNALED(status)) {
			int signal = WTERMSIG(status);
			if (signal == SIGABRT) {
				printf("Child process get SIGABRT signal\n");
			} else if (signal == SIGALRM) {
				printf("Child process get SIGALRM signal\n");
			} else if (signal == SIGBUS) {
				printf("Child process get SIGBUS signal\n");
			} else if (signal == SIGFPE) {
				printf("Child process get SIGFPE signal\n");
			} else if (signal == SIGHUP) {
				printf("Child process get SIGHUP signal\n");
			} else if (signal == SIGILL) {
				printf("Child process get SIGILL signal\n");
			} else if (signal == SIGINT) {
				printf("Child process get SIGINT signal\n");
			} else if (signal == SIGPIPE) {
				printf("Child process get SIGPIPE signal\n");
			} else if (signal == SIGQUIT) {
				printf("Child process get SIGQUIT signal\n");
			} else if (signal == SIGSEGV) {
				printf("Child process get SIGSEGV signal\n");
			} else if (signal == SIGTERM) {
				printf("Child process get SIGTERM signal\n");
			} else if (signal == SIGTRAP) {
				printf("Child process get SIGTRAP signal\n");
			} else if (signal = SIGKILL) {
				printf("Child process get SIGKILL signal\n");
			} else {
				printf("The signal is not correct\n");
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