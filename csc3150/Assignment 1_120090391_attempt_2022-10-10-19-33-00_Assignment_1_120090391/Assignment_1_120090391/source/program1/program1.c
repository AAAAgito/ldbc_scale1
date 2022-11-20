#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	/* fork a child process */
	int state;
	pid_t pid;

	printf("Process start to fork\n");
	pid = fork();

	if (pid < 0) {
		printf("Fork error!\n");
		exit(1);
	} else {
		// Child process
		if (pid == 0) {
			int i;
			char *arg[argc];

			printf("I'm the Child Process, my pid = %d\n",
			       getpid());

			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);

			raise(SIGCHLD);

			perror("execve");
			exit(EXIT_FAILURE);
		}
		// Parent process
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			waitpid(-1, &state, WUNTRACED);

			printf("Parent process receives SIGCHLD signal\n");
			if (WIFEXITED(state)) {
				printf("Normal termination with EXIT STATUS = %d \n",
				       WEXITSTATUS(state));
			} else if (WIFSIGNALED(state)) {
				int signal = WTERMSIG(state);
				switch (signal) {
				case 1:
					printf("child process raise SIGHUP signal\n");
					break;
				case 2:
					printf("child process raise SIGINT signal\n");
					break;
				case 3:
					printf("child process raise SIGQUIT signal\n");
					break;
				case 4:
					printf("child process raise SIGILL signal\n");
					break;
				case 5:
					printf("child process raise SIGTRAP signal\n");
					break;
				case 6:
					printf("child process raise SIGABRT signal\n");
					break;
				case 7:
					printf("child process raise SIGBUS signal\n");
					break;
				case 8:
					printf("child process raise SIGFPE signal\n");
					break;
				case 9:
					printf("child process raise SIGKILL signal\n");
					break;
				case 11:
					printf("child process raise SIGSEGV signal\n");
					break;
				case 13:
					printf("child process raise SIGPIPE signal\nn");
					break;
				case 14:
					printf("child process raise SIGALRM signal\n");
					break;
				case 15:
					printf("child process raise SIGTERM signal\n");
					break;
				}
			} else if (WIFSTOPPED(state)) {
				printf("child process raise SIGSTOP signal\n");
			} else {
				printf("CHILD PROCESS CONTINUED\n");
			}

			exit(0);
		}
	}
	return 0;
	/* execute test program */

	/* wait for child process terminates */

	/* check child process'  termination status */
}
