#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

int main(int argc, char *argv[])
{
	/* fork a child process */

	/* execute test program */

	/* wait for child process terminates */

	/* check child process's termination status */
	pid_t pid;
	int status;

	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		if (pid == 0) {
			int i;
			char *arg[argc];
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			printf("Child Pocess start to execute test program:\n");

			execve(arg[0], arg, NULL);
			exit(0);
		} else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives the SIGCHLD signal\n");
			if (status == -1) {
				perror("wait");
				exit(EXIT_FAILURE);
			} else if (WIFEXITED(status)) {
				printf("Normal temination with EXIT STATUS = %d\n",
				       status);
			} else if (WIFSTOPPED(status) != 0) {
				printf("Child process is stopped by stop signal\n");
				printf("Child process gets SIGSTOP signal\n");
			}

			else if (WIFSIGNALED(status)) {
				if (WTERMSIG(status) == 6) {
					printf("Child process is terminated by abort signal\n");
					printf("Child process gets SIGABRT signal\n");
				} else if (WTERMSIG(status) == 14) {
					printf("Child process is terminated by alarm signal\n");
					printf("Child process gets SIGALRM signal\n");
				} else if (WTERMSIG(status) == 7) {
					printf("Child process is terminated by bus signal\n");
					printf("Child process gets SIGBUS signal\n");
				} else if (WTERMSIG(status) == 8) {
					printf("Child process is terminated by floating signal\n");
					printf("Child process gets SIGFPE signal\n");
				} else if (WTERMSIG(status) == 1) {
					printf("Child process is terminated by hangup signal\n");
					printf("Child process gets SIGUP signal\n");
				} else if (WTERMSIG(status) == 4) {
					printf("Child process is terminated by illegal_instr signal\n");
					printf("Child process gets SIGILL signal\n");
				} else if (WTERMSIG(status) == 2) {
					printf("Child process is terminated by interrupt signal\n");
					printf("Child process gets SIGINT signal\n");
				} else if (WTERMSIG(status) == 9) {
					printf("Child process is terminated by kill signal\n");
					printf("Child process gets SIGKILL signal\n");
				} else if (WTERMSIG(status) == 13) {
					printf("Child process is terminated by pipe signal\n");
					printf("Child process gets SIGPIPE signal\n");
				} else if (WTERMSIG(status) == 3) {
					printf("Child process is terminated by quit signal\n");
					printf("Child process gets SIGQUIT signal\n");
				} else if (WTERMSIG(status) == 11) {
					printf("Child process is terminated by segment_fault signal\n");
					printf("Child process gets SIGSEGV signal\n");
				} else if (WTERMSIG(status) == 15) {
					printf("Child process is terminated by terminate signal\n");
					printf("Child process gets SIGTERM signal\n");
				} else if (WTERMSIG(status) == 5) {
					printf("Child process is terminated by trap signal\n");
					printf("Child process gets SIGTRAP signal\n");
				}
			}
			exit(0);
		}
	}

	return 0;
}
