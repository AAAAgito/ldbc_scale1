#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	pid_t pid, childpid;
	int state;

	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	}

	else {
		if (pid == 0) {
			sleep(1);
			char *arg[argc];
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());

			for (int i = 0; i < argc - 1; i++)
				arg[i] = argv[i + 1];
			arg[argc - 1] = NULL;
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);
		}

		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			waitpid(pid, &state, WUNTRACED);
			//wait(&state);
			printf("Parent process receives SIGCHLD signal\n");

			if (WIFEXITED(state)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(state));
			} else if (WIFSIGNALED(state)) {
				switch (WTERMSIG(state)) {
				case 6:
					printf("child process get SIGABRT signal\n");
					break;
				case 7:
					printf("child process get SIGBUS signal\n");
					break;
				case 14:
					printf("child process get SIGALRM signal\n");
					break;
				case 8:
					printf("child process get SIGFPE signal\n");
					break;
				case 1:
					printf("child process get SIGHUP signal\n");
					break;
				case 4:
					printf("child process get SIGILL signal\n");
					break;
				case 2:
					printf("child process get SIGINT signal\n");
					break;
				case 9:
					printf("child process get SIGKILL signal\n");
					break;
				case 13:
					printf("child process get SIGPIPE signal\n");
					break;
				case 3:
					printf("child process get SIGQUIT signal\n");
					break;
				case 11:
					printf("child process get SIGSEGV signal\n");
					break;
				case 15:
					printf("child process get SIGTERM signal\n");
					break;
				case 5:
					printf("child process get SIGTRAP signal\n");
					break;
				}
			} else if (WIFSTOPPED(state)) {
				printf("child process get SIGSTOP signal\n");
			} else
				printf("CHILD PROCESS CONTINUED\n");
			exit(0);
		}
	}

	/* fork a child process */

	/* execute test program */

	/* wait for child process terminates */

	/* check child process'  termination status */
}
