#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	int status;

	/* fork a child process */
	printf("Process start to fork\n");
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	}

	/* execute test program */
	else {
		char *arg[argc];
		if (pid == 0) {
			for (int i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);
			perror("execve");
			exit(EXIT_FAILURE);
		}
		/* wait for child process terminates */
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			waitpid(-1, &status, WUNTRACED);
			/* check child process'  termination status */
			printf("Parent process receives SIGCHLD signal\n");
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				int sigNo = WTERMSIG(status);
				switch (sigNo) {
				case 1:
					printf("child process get SIGHUP signal\nChild process is hung up by hangup signal\n");
					break;
				case 2:
					printf("child process get SIGINT signal\nChild process is interrupted by interrupt signal\n");
					break;
				case 3:
					printf("child process get SIGQUIT signal\nChild process is quitted by quit signal\n");
					break;
				case 4:
					printf("child process get SIGILL signal\nChild process is terminated because it has illegal instruction\n");
					break;
				case 5:
					printf("child process get SIGTRAP signal\nChild process is terminated by trap signal\n");
					break;
				case 6:
					printf("child process get SIGABRT signal\nChild process is aborted by abort signal\n");
					break;
				case 7:
					printf("child process get SIGBUS signal\nChild process is terminated because it has bus error\n");
					break;
				case 8:
					printf("child process get SIGFPE signal\nChild process is terminated because it has floating-point exception\n");
					break;
				case 9:
					printf("child process get SIGKILL signal\nChild process is killed by kill signal\n");
					break;
				case 10:
					printf("child process get SIGUSR1 signal\nChild process is terminated by user defined 1 signal\n");
					break;
				case 11:
					printf("child process get SIGASEGV signal\nChild process is terminated because it has segment fault\n");
					break;
				case 12:
					printf("child process get SIGUSER2 signal\nChild process is terminated by user defined 2 signal\n");
					break;
				case 13:
					printf("child process get SIGPIPE signal\nChild process is terminated by pipe signal\n");
					break;
				case 14:
					printf("child process get SIGALRM signal\nChild process is terminated by alarm signal (wake up call)\n");
					break;
				case 15:
					printf("child process get SIGTERM signal\nChild process is terminated by termination signal\n");
					break;
				default:
					printf("child process get signal %d\nChild process is terminated by signal %d\n",
					       sigNo, sigNo);
					break;
				}
			} else if (WIFSTOPPED(status)) {
				printf("child process get SIGSTOP signal\nChild process is stopped by stop singal\n");
			} else {
				printf("Child process continued\n");
			}
			exit(0);
		}
	}
}
