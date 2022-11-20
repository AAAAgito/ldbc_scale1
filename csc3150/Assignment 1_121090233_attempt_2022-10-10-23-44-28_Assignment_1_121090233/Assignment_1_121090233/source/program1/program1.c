#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *table[16] = { "NORMAL", "SIGHUP",  "SIGINT",  "SIGQUIT",
		    "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS",
		    "SIGFPE", "SIGKILL", "10",      "SIGSEGV",
		    "12",     "SIGPIPE", "SIGALRM", "SIGTERM" };

int main(int argc, char *argv[])
{
	sleep(3);

	pid_t pid;
	int status;

	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		if (pid == 0) {
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			int i;
			char *arg[argc];
			for (i = 0; i < argc - 1; i++)
				arg[i] = argv[i + 1];
			arg[argc - 1] = NULL;

			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);
			perror("execve");
			exit(EXIT_FAILURE);
		} else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			if (waitpid(pid, &status, WUNTRACED) < 0) {
				perror("wait");
				exit(2);
			}
			printf("Parent process receives SIGCHLD signal\n");
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS((status)));
				exit(0);
			} else {
				if (WIFSIGNALED(status)) {
					int sig = WTERMSIG(status);
					printf("child process get %s signal\n",
					       table[sig]);
				} else if (WIFSTOPPED(status)) {
					printf("child process get SIGSTOP signal\n");
				} else {
					printf("Err");
				}
			}
			exit(0);
		}
	}
}