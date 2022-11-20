#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	char *sig[20] = {
		"SIGHUP",    "SIGINT",  "SIGQUIT", "SIGILL",  "SIGTRAP",
		"SIGABRT",   "SIGBUS",  "SIGFPE",  "SIGKILL", "SIGUSR1",
		"SIGSEGV",   "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM",
		"SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP",
	};
	/* fork a child process */
	pid_t pid;

	int status;

	printf("Process start to fork\n");

	pid = fork();

	/* execute test program */
	//child process
	if (pid == 0) {
		printf("I'm the Child Progress, My pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");
		char *arg[argc];

		for (int i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;

		execvp(arg[0], arg);

		exit(0);
	}

	//father process
	else if (pid > 0) {
		printf("I'm the Parents Progress, My pid = %d\n", getpid());
		waitpid(pid, &status, WUNTRACED);

		if (WIFEXITED(status)) {
			printf("Parent process receives SIGCHLD signal\n");
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));

		} else if (WIFSIGNALED(status)) {
			printf("Parent process receives SIGCHLD signal\n");

			printf("Child process get %s signal\n",
			       sig[WTERMSIG(status) - 1]);
		}

	} else if (WIFSTOPPED(status)) {
		printf("Parent process receives SIGCHLD signal\n");
		// printf("child process get %s signal\n",
		//    sig[WSTOPSIG(status) - 1]);
	}

	exit(0);

	/* code */
}
