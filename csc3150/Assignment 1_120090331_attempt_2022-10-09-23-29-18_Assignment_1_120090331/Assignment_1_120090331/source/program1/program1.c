#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

char *Signal_list[] = { "SIGHUP",  "SIGINT",  "SIGQUIT", "SIGILL",  "SIGTRAP",
			"SIGABRT", "SIGBUS",  "SIGFPE",  "SIGKILL", "SIGSR1",
			"SIGEGV",  "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM" };

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork();
	/* execute test program */
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		/*Child Process*/
		if (pid == 0) {
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			int i;
			char *arg[argc];
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);
			exit(EXIT_FAILURE);
		} /*Parent Process*/
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			/* wait for child process terminates */
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives the SIGCHLD signal\n");
			/* check child process'  termination status */
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSTOPPED(status)) {
				printf("child process get SIGSTOP signal\n");
			} else if (WIFSIGNALED(status)) {
				int signal = WTERMSIG(status);
				printf("child process get %s signal\n",
				       Signal_list[signal - 1]);
			}
			exit(0);
		}
	}
}
