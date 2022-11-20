#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]) {

	pid_t pid;
	int status;
	char *signame[] = {
		"",
		"SIGHUP",
		"SIGINT",
		"SIGQUIT",
		"SIGILL",
		"SIGTRAP",
		"SIGABRT",
		"SIGBUS",
		"SIGFPE",
		"SIGKILL",
		"SIGUSR1",
		"SIGSEGV",
		"SIGUSR2",
		"SIGPIPE",
		"SIGALRM",
		"SIGTERM",
		"SIGSTKFLT",
		"SIGCHLD",
		"SIGCONT",
		"SIGSTOP"
	};

	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	}

	if (pid == 0) {
		int i;
		char *arg[argc];

		for (i = 0; i < argc - 1; i++) arg[i] = argv[i + 1];

		sleep(1);

		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");

		execve(arg[0], arg, NULL);
	}	
	else {
		printf("I'm the Parent Process, my pid = %d\n", getpid());

		waitpid(-1, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");

		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status)) {
			printf("child process get %s signal\n", signame[WTERMSIG(status)]);
		}
		else if (WIFSTOPPED(status)) {
			printf("child process get %s signal\n", signame[WSTOPSIG(status)]);
		}
	}
	
	return 0;
}