#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	/* fork a child process */
	pid_t pid;
	int status;
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		//Child process
		if (pid == 0) {
			int i;
			char *arg[argc];

			/* execute test program */
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("I'm the Child Process: %d\n", getpid());
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);

			perror("execve");
			exit(EXIT_FAILURE);
		}

		//Parent process
		else {
			printf("Process start to fork\n");
			printf("I'm the Parent Process: %d\n", getpid());
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");

			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				int signal = WTERMSIG(status);
				switch (signal) {
				/* print out specified signal raised in child process and how it terminated */
				case 1:
					printf("Parent process receives SIGHUB signal\n");
					break;
				case 2:
					printf("Parent process receives SIGINT signal\n");
					break;
				case 3:
					printf("Parent process receives SIGQUIT signal\n");
					break;
				case 4:
					printf("Parent process receives SIGILL signal\n");
					break;
				case 5:
					printf("Parent process receives SIGTRAP signal\n");
					break;
				case 6:
					printf("Parent process receives SIGABRT signal\n");
					break;
				case 7:
					printf("Parent process receives SIGBUS signal\n");
					break;
				case 8:
					printf("Parent process receives SIGFPE signal\n");
					break;
				case 9:
					printf("Parent process receives SIGKILL signal\n");
					break;
				case 11:
					printf("Parent process receives SIGSEGV signal\n");
					break;
				case 13:
					printf("Parent process receives SIGPIPE signal\n");
					break;
				case 14:
					printf("Parent process receives SIGALRM signal\n");
					break;
				case 15:
					printf("Parent process receives SIGTERM signal\n");
					break;
				}
			} else if (WIFSTOPPED(status)) {
				printf("Child process gets SIGCHLD signal.\n");
				printf("CHILD PROCESS STOPPED\n");
			} else {
				printf("CHILD PROCESS CONTINUED.\n");
			}

			exit(0);
		}
	}

	return 0;
}
