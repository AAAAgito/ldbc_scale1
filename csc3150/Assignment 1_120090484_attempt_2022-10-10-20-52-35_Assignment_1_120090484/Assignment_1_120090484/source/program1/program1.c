#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	/* fork a child process */
	pid_t pid, wpid;
	int status, signum;
	printf("process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		/* execute test program */
		if (pid == 0) {
			int i;
			char *arg[argc];

			//printf("this is child process\n");

			// raise(SIGCHLD);

			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("I'm child process, my pid = %d\n", getpid());
			printf("child process start to execute test file\n");

			execve(arg[0], arg, NULL);

			printf("continue to run the original child process\n");

			perror("execve");
			exit(EXIT_FAILURE);

		} else {
			printf("I'm parent process, my pid = %d\n", getpid());

			/* wait for child process terminates */

			wpid = waitpid(pid, &status, WUNTRACED);
			printf("parent process receives SIGCHILD signal\n");

			/* check child process'  termination status */

			if (WIFEXITED(status)) {
				printf("child process normally exited with status %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSTOPPED(status)) {
				signum = WSTOPSIG(status);
				printf("CHILD PROCESS STOPPED: %d\n", signum);
				printf("signal stop\n");
			} else if (WIFSIGNALED(status)) {
				signum = WTERMSIG(status);
				printf("child execution failed: %d\n", signum);

				if (signum == 2) {
					printf("signal intrrupted\n");
				} else if (signum == 3) {
					printf("signal quit\n");
				} else if (signum == 6) {
					printf("signal abort\n");
				} else if (signum == 14) {
					printf("signal alarm\n");
				} else if (signum == 7) {
					printf("signal bus\n");
				} else if (signum == 8) {
					printf("signal floating\n");
				} else if (signum == 1) {
					printf("signal hang up\n");
				} else if (signum == 4) {
					printf("signal illegal instruction\n");
				} else if (signum == 9) {
					printf("signal kill\n");
				} else if (signum == 13) {
					printf("signal pipe\n");
				} else if (signum == 11) {
					printf("signal segment fault\n");
				} else if (signum == 15) {
					printf("signal terminate\n");
				} else if (signum == 5) {
					printf("signal trap\n");
				}
			}

			exit(0);
		}
	}

	return 0;
}
