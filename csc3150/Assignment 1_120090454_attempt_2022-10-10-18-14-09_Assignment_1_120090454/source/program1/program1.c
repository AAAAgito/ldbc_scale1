#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // unix std, header for POSIX OS API.
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

char *signals[] = { NULL,     "SIGHUP",  "SIGINT",  "SIGQUIT",
		    "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS",
		    "SIGFPE", "SIGKILL", NULL,      "SIGSEGV",
		    NULL,     "SIGPIPE", "SIGALRM", "SIGTERM" };

int main(int argc, char *argv[])
{
	pid_t pid, w;
	int status;

	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork(); // fork the process, for the child process return 0, for the parent process, return child pid, for failure, return -1.
	// Both parent and child process will start at the following line.
	if (pid == -1) {
		perror("fork error"); // Print a message describing the meaning of the value of errno.
		exit(1); // Exit the program abnormally.
	} else {
		// child process
		if (pid == 0) {
			char *arg[argc];
			for (int i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("I'm the Child Process, my pid = %d\n",
			       getpid()); // getppid: get parent pid.
			printf("Child Process start to execute a test program:\n");
			execve(arg[0], arg, NULL);

			printf("Continue to run original child process\n"); // If the execve fails, it will continue to run on original child process.
			perror("execve");
			exit(EXIT_FAILURE); // Exit the program abnormally.
		}
		// parent process
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			w = waitpid(pid, &status, WUNTRACED);
			if (w == -1) {
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
			printf("Parent process receives SIGCHLD signal\n");

			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
				exit(EXIT_SUCCESS);
			} else if (WIFSIGNALED(status)) {
				printf("status: %d", status);
				printf("Child process is killed by SIGNAL: %s\n",
				       signals[WTERMSIG(status)]);
				printf("Child process is killed by SIGNAL: %d\n",
				       WTERMSIG(status));
				exit(EXIT_SUCCESS);
			} else if (WIFSTOPPED(status)) {
				printf("Child process is stopped with SIGNAL: SIGSTOP\n");
				printf("CHILD PROCESS STOPPED\n");
			} else if (WIFCONTINUED(status)) {
				printf("Child process continues to run\n");
			}
		}
	}
	return 0;
}
