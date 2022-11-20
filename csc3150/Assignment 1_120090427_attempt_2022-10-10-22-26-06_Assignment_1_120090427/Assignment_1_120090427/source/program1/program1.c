#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	// Set the pointer status
	int status;
	// Fork the process
	printf("Process start to fork\n");
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork error");
		exit(1);
	} else if (pid == 0) {
		printf("I'm the Child Process, my pid = %d\n", getpid());
	} else {
		printf("I'm the Parent Process, my pid = %d\n", getpid());
	}
	// Execute test program
	if (pid == 0) {
		int i;
		char *arg[argc];
		printf("Child process start to execute test program:\n");
		for (i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;
		execve(arg[0], arg, NULL);
		raise(SIGCHLD);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	// Wait for child process terminates and check child process's termination
	if (pid > 0) {
		waitpid(-1, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			switch (WTERMSIG(status)) {
			case 1:
				printf("child process get SIGHUP signal\n");
				break;
			case 2:
				printf("child process get SIGINT signal\n");
				break;
			case 3:
				printf("child process get SIGQUIT signal\n");
				break;
			case 4:
				printf("child process get SIGILL signal\n");
				break;
			case 5:
				printf("child process get SIGTRAP signal\n");
				break;
			case 6:
				printf("child process get SIGABRT signal\n");
				break;
			case 7:
				printf("child process get SIGBUS signal\n");
				break;
			case 8:
				printf("child process get SIGFPE signal\n");
				break;
			case 9:
				printf("child process get SIGKILL signal\n");
				break;
			case 10:
				printf("child process get SIGUSR1 signal\n");
				break;
			case 11:
				printf("child process get SIGSEGV signal\n");
				break;
			case 12:
				printf("child process get SIGUSR2 signal\n");
				break;
			case 13:
				printf("child process get SIGPIPE signal\n");
				break;
			case 14:
				printf("child process get SIGALRM signal\n");
				break;
			case 15:
				printf("child process get SIGTERM signal\n");
				break;
			case 16:
				printf("child process get SIGSTKFLT signal\n");
				break;
			case 17:
				printf("child process get SIGCHLD signal\n");
				break;
			case 18:
				printf("child process get SIGCONT signal\n");
				break;
			case 20:
				printf("child process get SIGTSTP signal\n");
				break;
			case 21:
				printf("child process get SIGTTIN signal\n");
				break;
			case 22:
				printf("child process get SIGTTOU signal\n");
				break;
			case 23:
				printf("child process get SIGURG signal\n");
				break;
			case 24:
				printf("child process get SIGXCPU signal\n");
				break;
			case 25:
				printf("child process get SIGXFSZ signal\n");
				break;
			case 26:
				printf("child process get SIGVTALRM signal\n");
				break;
			case 27:
				printf("child process get SIGPROF signal\n");
				break;
			case 28:
				printf("child process get SIGWINCH signal\n");
				break;
			case 29:
				printf("child process get SIGIO signal\n");
				break;
			case 30:
				printf("child process get SIGPWR signal\n");
				break;
			case 31:
				printf("child process get SIGSYS signal\n");
				break;
			default:
				printf("child process get signal\n");
				break;
			}
			printf("CHILD EXECUTION FAILED: %d\n",
			       WTERMSIG(status));
		} else if (WIFSTOPPED(status)) {
			printf("child process get SIGSTOP signal\n");
			printf("CHILD PROCESS STOPPED: %d\n", WSTOPSIG(status));
		} else {
			printf("CHILD PROCESS CONTINUED\n");
		}
		exit(0);
	}
	return 0;
}
