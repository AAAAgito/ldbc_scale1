#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

void print_status(int s)
{
	printf("Parent process receives SIGCHLD signal\n");
	if (WIFEXITED(s)) {
		printf("Normal termination with EXIT STATUS = %d\n", s);
	} else if (WIFSTOPPED(s)) {
		printf("child process get SIGSTOP signal\n");
	} else {
		// printf("%d\n",WTERMSIG(s));
		if (WTERMSIG(s) == 14)
			printf("child process get SIGALRM signal\n");
		else if (WTERMSIG(s) == 6)
			printf("child process get SIGABRT signal\n");
		else if (WTERMSIG(s) == 7)
			printf("child process get SIGBUS signal\n");
		else if (WTERMSIG(s) == 8)
			printf("child process get SIGFPE signal\n");
		else if (WTERMSIG(s) == 1)
			printf("child process get SIGHUP signal\n");
		else if (WTERMSIG(s) == 4)
			printf("child process get SIGILL signal\n");
		else if (WTERMSIG(s) == 2)
			printf("child process get SIGINT signal\n");
		else if (WTERMSIG(s) == 9)
			printf("child process get SIGKILL signal\n");
		else if (WTERMSIG(s) == 13)
			printf("child process get SIGPIPE signal\n");
		else if (WTERMSIG(s) == 3)
			printf("child process get SIGQUIT signal\n");
		// else if (WTERMSIG(s) == 127)
		// 	printf("child process get SIGSTOP signal\n");
		else if (WTERMSIG(s) == 11)
			printf("child process get SIGSEGV signal\n");
		else if (WTERMSIG(s) == 15)
			printf("child process get SIGTERM signal\n");
		else if (WTERMSIG(s) == 5)
			printf("child process get SIGTRAP signal\n");
	}
}

int main(int argc, char *argv[])
{
	int status;
	printf("Process start to fork\n");
	pid_t pid = fork();

	if (pid < 0) {
		printf("Fork error!\n");
	} else {
		if (pid == 0) {
			int i;
			char *arg[argc];

			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("I'm the Parent Process, my pid = %d\n",
			       getppid());
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());

			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);

		} else {
			waitpid(pid, &status, WUNTRACED);
			// printf("%d\n",status);
			print_status(status);
			exit(1);
		}
	}
	return 0;
}
