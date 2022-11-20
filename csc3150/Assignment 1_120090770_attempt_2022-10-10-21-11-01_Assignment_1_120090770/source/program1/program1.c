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

	/* execute test program */

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		// child process
		if (pid == 0) {
			int i;
			char *arg[argc];
			sleep(1);
			printf("I'm the child process, my pid = %d\n",
			       getpid());

			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("Child process start to execute test "
			       "program:\n");
			execve(arg[0], arg, NULL);

			printf("Child process run original child process!\n");
			exit(EXIT_SUCCESS);
		}

		/* wait for child process terminates */
		else {
			printf("I'm the parent process, my pid = %d\n",
			       getpid());
			waitpid(pid, &state, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");
			/* check child process'  termination status */
			if (WIFEXITED(state)) {
				printf("Normal termination with EXIT STATUS = "
				       "%d\n",
				       WEXITSTATUS(state));
			} else if (WIFSIGNALED(state)) {
				// printf("state: %d\n",state&127);
				switch (state & 127) {
				case 1:
					printf("Child process get SIGHUP signal\n");
					break;
				case 2:
					printf("Child process get SIGINT signal\n");
					break;
				case 3:
					printf("Child process get SIGQUIT signal\n");
					break;
				case 4:
					printf("Child process get SIGILL signal\n");
					break;
				case 5:
					printf("Child process get SIGTRAP signal\n");
					break;
				case 6:
					printf("Child process get SIGABRT signal\n");
					break;
				case 7:
					printf("Child process get SIGBUS signal\n");
					break;
				case 8:
					printf("Child process get SIGFPE signal\n");
					break;
				case 9:
					printf("Child process get SIGKILL signal\n");
					break;
				case 11:
					printf("Child process get SIGSEGV signal\n");
					break;
				case 13:
					printf("Child process get SIGPIPE signal\n");
					break;
				case 14:
					printf("Child process get SIGALRM signal\n");
					break;
				case 15:
					printf("Child process get SIGTERM signal\n");
					break;
				case 127:
					printf("Child process get SIGSTOP signal\n");
					break;
				default:
					printf(" Not receive signal\n");
					break;
				};
				// printf("Child Execution raise code: %d\n",
			} else if (WIFSTOPPED(state)) {
				// kill(pid,SIGKILL);
				printf("Child process get SIGSTOP signal\n");
			} else {
				printf("Child process continued\n");
			}
			printf("\n\n");
			exit(0);
		}
	}

	return 0;
}
