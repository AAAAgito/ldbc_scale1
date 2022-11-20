#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	/* fork a child process */

	/* execute test program */

	/* wait for child process terminates */

	/* check child process'  termination status */
	printf("Process start to fork");
	int state;
	pid_t pid = fork();
	if (pid < 0) { // fork error
		printf("Fork error\n");
	}
	if (pid == 0) { // child process
		int i;
		char *arg[argc];
		for (int i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;
		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");
		execve(arg[0], arg, NULL);
		exit(EXIT_FAILURE);
	} else { // parent process
		printf("I'm the Parent Process, my pid = %d\n", getpid());
		waitpid(pid, &state, WUNTRACED);

		// the process ends normally
		if (WIFEXITED(state)) {
			printf("Parent process receives SIGCHLD signal\n");
			printf("Normal termination with EXIT STATUS = 0\n");
		}
		if (WIFSTOPPED(state)) {
			printf("Parent process receives SIGCHLD signal\n");
			printf("child process get SIGSTOP signal\n");
		}
		// the process doesnt end normally
		if (WIFSIGNALED(state)) {
			switch (WTERMSIG(state)) {
			case SIGABRT:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGABRT signal\n");
				break;
			case SIGALRM:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGALRM signal\n");
				break;
			case SIGBUS:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGBUS signal\n");
				break;
			case SIGFPE:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGFPE signal\n");
				break;
			case SIGHUP:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGHUP signal\n");
				break;
			case SIGILL:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGILL signal\n");
				break;
			case SIGINT:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGINT signal\n");
				break;
			case SIGKILL:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGKILL signal\n");
				break;
			case SIGPIPE:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGPIPE signal\n");
				break;
			case SIGQUIT:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGQUIT signal\n");
				break;
			case SIGTERM:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGTERM signal\n");
				break;
			case SIGTRAP:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGTRAP signal\n");
				break;
			case SIGSEGV:
				printf("Parent process receives SIGCHLD signal\n");
				printf("child process get SIGSEGV signal\n");
				break;
			}
		}
		return 0;
	}
}
