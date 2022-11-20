#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	/* fork a child process */
	int status;
	printf("Process start to fork.\n");
	pid_t pid = fork();

	// child process
	if (pid == 0) {
		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program.\n");

		char *arg[argc];
		int i;
		for (i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;
		/* execute test program */
		execve(arg[0], arg, NULL);
	}

	// parent process
	else {
		printf("I'm the Parent Process, my pid = %d\n", getpid());

		/* wait for child process terminates */
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal.\n");

		/* check child process'  termination status */
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			switch (WTERMSIG(status)) {
			case 1:
				printf("Child process get the SIGHUP signal.\n");
				// printf("Hangup is detected in the terminal or death of controlling "
				//    "process.\n");
				break;
			case 2:
				printf("Child process get SIGINT signal.\n");
				// printf("The child process is interrupted from the key borad.\n");
				break;
			case 3:
				printf("Child process get SIGQUIT signal.\n");
				// printf("The child process is quitted from keyborad.\n");
				break;
			case 4:
				printf("Child process get SIGILL signal.\n");
				// printf("The child process is terminated because of illegal "
				//    "instructions.\n");
				break;
			case 5:
				printf("Child process get SIGTRAP signal.\n");
				// printf("The child process is terminated because of breakpoints or "
				//    "other traps.\n");
				break;
			case 6:
				printf("Child process get SIGABRT signal.\n");
				// printf("Error is found in the program and the child process is "
				//    "terminated by abort().\n");
				break;
			case 7:
				printf("Child process get SIGBUS signal.\n");
				// printf("The child process is terminated out of bus error (or bad "
				//    "memory access).\n");
				break;
			case 8:
				printf("Child process get SIGFPE signal.\n");
				// printf("termination caused by floating-point exception\n");
				break;
			case 9:
				printf("Child process get SIGKILL signal.\n");
				// printf("The child process is killed by SIGKILL signal.\n");
				break;
			case 11:
				printf("Child process get SIGSEGV signal.\n");
				// printf("termination caused by invalid memory reference\n");
				break;
			case 13:
				printf("Child process get SIGPIPE signal.\n");
				// printf("The child process teminated because of broken pipe.\n");
				break;
			case 14:
				printf("Child process get SIGALRM signal.\n");
				// printf("The child process is terminated by time signal.\n");
				printf("\n");
				break;
			case 15:
				printf("Child process get SIGTERM signal.\n");
				// printf("The child process is terminated by termination signal.\n");
				break;
			default:
				printf("Child process raise other signals.\n");
				// printf("The child process is terminated.\n");
				break;
			}
		} else if (WIFSTOPPED(status)) {
			switch (WSTOPSIG(status)) {
			case 19:
				printf("Child process get SIGSTOP signal.\n");
				// printf("The child process stopped.\n");
				break;
			default:
				printf("Parent process receives signals that stop the child process\n");
				printf("The child process stopped.\n");
				break;
			}
		} else {
			printf("Child process continues.\n");
		}
		exit(0);
	}
}
