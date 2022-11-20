#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	int state;
	printf("Process start to fork\n");
	pid_t pid = fork();

	char *failedSignal[] = { NULL,     "SIGHUP",  "SIGINT",  "SIGQUIT",
				 "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS",
				 "SIGFPE", "SIGKILL", NULL,      "SIGSEGV",
				 NULL,     "SIGPIPE", "SIGALRM", "SIGTERM" };
	/* fork a child process */
	if (pid < 0) {
		printf("fork error!\n");
	} else {
		//Child Process
		if (pid == 0) {
			int i;
			char *arg[argc];

			printf("I'm the child process, my pid = %d\n",
			       getpid());

			/* execute test program */
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);

			exit(0);
		}
		//Parent Process
		else {
			printf("I'm the parent process, my pid = %d\n",
			       getpid());

			/* wait for child process terminates */
			waitpid(-1, &state, WUNTRACED);

			printf("Parent process receives SIGCHLD signal\n");

			/* check child process'  termination status */
			if (WIFEXITED(state)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(state));
			} else if (WIFSIGNALED(state)) {
				printf("CHILD EXECUTION FAILED: %d\n",
				       WTERMSIG(state));
				printf("child process get %s signal\n",
				       failedSignal[WTERMSIG(state)]);
			} else if (WIFSTOPPED(state)) {
				printf("CHILD PROCESS STOPPED: %d\n",
				       WSTOPSIG(state));
				printf("child process get SIGSTOP signal\n");
			}
			exit(0);
		}
	}
	return 0;
}