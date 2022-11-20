#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *signal_decode[] = { "SIGHUP",  "SIGINT", "SIGQUIT", "SIGILL",  "SIGTRAP",
			  "SIGABRT", "SIGBUS", "SIGFPE",  "SIGKILL", NULL,
			  "SIGSEGV", NULL,     "SIGPIPE", "SIGALRM", "SIGTERM" };

int main(int argc, char *argv[])
{
	/* fork a child process */
	pid_t pid;
	int status;
	int SIGCHILD;
	printf("Process start to fork\n");
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
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
			// printf("----------CHILD PROCESS END---------\n");
			// raise(SIGCHILD);
			// pe1ror("execve");
			// exit(1);
		} else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHILD signal\n");
			// printf("----------CHILD PROCESS START---------\n");
			// printf("Child process exited with status %d \n",status);
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d \n",
				       WEXITSTATUS(status));
			} else if (WIFSTOPPED(status)) {
				// kill(pid, SIGCONT);
				printf("child process get stop signal.\n");
			} else if (WIFSIGNALED(status)) {
				printf("child process get %s signal.\n",
				       signal_decode[WTERMSIG(status) - 1]);
			}
			// else if(WIFSTOPPED(status)){
			// printf("CHILD PROCESS STOPPED: %d \n", WSTOPSIG(status));
			// }
			else {
				printf("CHILD PROCESS CONTINUED\n");
			}
			exit(0);
		}
	}
	/* execute test program */

	/* wait for child process terminates */

	/* check child process'  termination status */

	return 0;
}
