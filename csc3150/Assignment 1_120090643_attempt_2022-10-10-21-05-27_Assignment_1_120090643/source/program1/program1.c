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
	printf("process start to fork \n");
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		if (pid == 0) {
			printf("I'm the child process,my pid = %d\n", getpid());
			printf("Children process start to execute the program:\n");
			int i;
			char *arg[argc];
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			/* execute test program */
			execve(arg[0], arg, NULL);

		} else { //parent process
			printf("I'm the parent process,my pid = %d\n",
			       getpid());
			/* wait for child process terminates */
			waitpid(-1, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");
			/* check child process'  termination status */
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				int sig = WTERMSIG(status);
				switch (sig) {
				case 6:
					printf("child process get SIGABRT signal\n");
					break;
				case 14:
					printf("child process get SIGALRM signal\n");
					break;
				case 7:
					printf("child process get SIGBUS signal\n");
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
				default:
					printf("unknown signal\n");
					break;
				}

			} else if (WIFSTOPPED(status)) {
				printf("CHILD PROCESS STOPPED\n");
			} else {
				printf("process continued\n");
			}
			exit(0);
		}
	}
	return 0;
}
