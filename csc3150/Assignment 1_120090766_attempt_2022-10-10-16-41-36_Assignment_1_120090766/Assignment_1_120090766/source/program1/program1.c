#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char *sig[15] = {
		"SIGHUP",  "SIGINT",  "SIGQUIT", "SIGILL",  "SIGTRAP",
		"SIGABRT", "SIGBUS",  "SIGFPE",  "SIGKILL", "SIGUSR1",
		"SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM"
	};
	/* fork a child process */
	int state;
	printf("Process start to fork\n");
	pid_t pid = fork();
	if (pid < 0) {
		printf("Fork error!\n");
	} else {
		// child process
		if (pid == 0) {
			int i;
			char *arg[argc];
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);
			printf("Continue to run original child process!\n");
			perror("execve");
			exit(EXIT_FAILURE);
		} else {
			// father process
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			// wait(&state);
			waitpid(-1, &state, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");
			if (WIFEXITED(state)) {
				printf("Normal termination with Exit STATUS = %d\n",
				       WEXITSTATUS(state));
			} else if (WIFSIGNALED(state)) {
				printf("Parent process get %s signal\n",
				       sig[WTERMSIG(state) - 1]);
			} else if (WIFSTOPPED(state)) {
				printf("child process get SIGSTOP signal\n");
			}
		}
	}
	return 0;
	/* execute test program */

	/* wait for child process terminates */

	/* check child process'  termination status */
}
