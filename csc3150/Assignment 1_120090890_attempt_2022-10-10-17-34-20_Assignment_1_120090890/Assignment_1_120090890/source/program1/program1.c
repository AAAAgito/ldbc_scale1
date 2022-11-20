#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	/* fork a child process */

	int sta; //status value
	pid_t p; //pid value
	printf("Process start to fork\n");

	/* fork a child process */
	p = fork();

	if (p == -1) {
		perror("Fork error!\n");
		exit(1);
	} else {
		// child process
		if (p == 0) {
			char *arg[argc];
			int i;
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("I'm the child process, my pid = %d\n",
			       getpid());
			printf("Child process start to execute test "
			       "program:\n");

			/* execute test program */
			execve(arg[0], arg, NULL);
		}

		// parent process
		else {
			printf("I'm the parent process, my pid = %d\n",
			       getpid());

			/* wait for child process terminates */
			//wait(&sta);

			waitpid(p, &sta, WUNTRACED);

			printf("Parent process receives the SIGCHLD signals\n");
			/* check child process'  termination state */
			if (WIFEXITED(sta)) {
				printf("Normal termination with EXIT STATUS = "
				       "%d\n",
				       WEXITSTATUS(sta));
			} else if (WIFSIGNALED(sta)) {
				switch (sta & 0x7f) {
				case 4: {
					printf("child process get SIGILL "
					       "signal\n");
					break;
				}

				case 2: {
					printf("child process get SIGINT "
					       "signal\n");

					break;
				}

				case 1: {
					printf("child process get SIGHUP "
					       "signal\n");

					break;
				}
				case 5: {
					printf("child process get SIGTRAP "
					       "signal\n");
					break;
				}
				case 6: {
					printf("child process get SIGABRT "
					       "signal\n");
					break;
				}
				case 3: {
					printf("child process get SIGQUIT "
					       "signal\n");

					break;
				}

				case 9: {
					printf("child process get SIGKILL "
					       "signal\n");

					break;
				}
				case 8: {
					printf("child process get SIGFPE "
					       "signal\n");

					break;
				}

				case 13: {
					printf("child process get SIGPIPE "
					       "signal\n");
					break;
				}
				case 7: {
					printf("child process get SIGBUS "
					       "signal\n");
					break;
				}
				case 11: {
					printf("child process get SIGSEGV "
					       "signal\n");

					break;
				}

				case 15: {
					printf("child process get SIGTERM "
					       "signal\n");
					break;
				}
				case 14: {
					printf("child process get SIGALRM "
					       "signal\n");
					break;
				}
				}

			} else if (WIFSTOPPED(sta)) {
				printf("Child process get SIGSTOP signal\n");
			} else {
				printf("Child process continued\n");
			}
			exit(0);
		}
	}
	return 0;
}
