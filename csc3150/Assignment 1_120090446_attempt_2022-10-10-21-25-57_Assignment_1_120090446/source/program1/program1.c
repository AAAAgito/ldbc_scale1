#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		// Child process
		if (pid == 0) {
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());

			/* execute test program */
			int i;
			char *arg[argc];

			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("Child process start to execute test program:\n");

			// raise(SIGCHLD);

			execve(arg[0], arg, NULL);

			printf("Continue to run original child process!\n");

			perror("execve");
			exit(EXIT_FAILURE);

		} else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());

			/* wait for child process terminates */
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");

			/* check child process'  termination status */
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				switch (WTERMSIG(status)) {
				case 6:
					printf("Child process was aborted by abort signal\n");
					printf("SIGABRT signal was raised in child process\n");
					break;
				case 1:
					printf("Child process was hung up\n");
					printf("SIGHUP signal was raised in child process\n");
					break;
				case 2:
					printf("Child process was interrupted\n");
					printf("SIGINT signal was raised in child process\n");
					break;
				case 3:
					printf("Child process quited\n");
					printf("SIGQUIT signal was raised in child process\n");
					break;
				case 4:
					printf("Child process had illegal instruction\n");
					printf("SIGILL signal was raised in child process\n");
					break;
				case 5:
					printf("Child process was trapped\n");
					printf("SIGTRAP signal was raised in child process\n");
					break;
				case 7:
					printf("Child process had bus error\n");
					printf("SIGBUS signal was raised in child process\n");
					break;
				case 8:
					printf("Child process had floating point exception\n");
					printf("SIGFPE signal was raised in child process\n");
					break;
				case 9:
					printf("Child process was killed\n");
					printf("SIGKILL signal was raised in child process\n");
					break;
				case 11:
					printf("Child process had segment fault\n");
					printf("SIGSEGV signal was raised in child process\n");
					break;
				case 13:
					printf("Child process had broken pipe\n");
					printf("SIGPIPE signal was raised in child process\n");
					break;
				case 14:
					printf("Child process was alarmed\n");
					printf("SIGALRM signal was raised in child process\n");
					break;
				case 15:
					printf("Child process terminated\n");
					printf("SIGTERM signal was raised in child process\n");
					break;
				}
			} else if (WIFSTOPPED(status)) {
				printf("child process get SIGSTOP signal\n");
			} else {
				printf("Child process continued\n");
				printf("SIGCONT signal was raised in child process\n");
			}
			exit(0);
		}
	}

	return 0;
}
