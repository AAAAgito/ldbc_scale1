#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	/* fork a child process */
	pid_t pid;
	int status;

	printf("Process start to fork \n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	}

	else {
		// child process
		if (pid == 0) {
			int i;
			char *arg[argc];

			// arg[0] == ./exece, should be removed
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("I'm the Child Process, my pid = %d \n",
			       getpid());
			printf("Children process start to execute the program: \n");
			/* execute test program */
			execve(arg[0], arg, NULL);

			perror("execve");
			exit(EXIT_FAILURE);
		}

		else { // parent process
			printf("I'm Parent Process, my pid = %d\n", getpid());

			/* wait for child process terminates */
			waitpid(-1, &status, WUNTRACED);
			// wait(&status);//need to deal with stop case
			printf("Parent process receives SIGCHLD signal \n");
			printf("child process get SIGSTOP signal. \n");

			/* check child process'  termination status */
			if (WIFEXITED(status)) { // normal
				printf("Normal termination with EXIT STATUS = %d \n",
				       WEXITSTATUS(status));
			}

			else if (WIFSTOPPED(status)) { // stop
				printf("CHILD EXECUTION STOPPED\n");
			}

			else if (WIFSIGNALED(
					 status)) { // exit because of a signal
				if (WTERMSIG(status) == 1) { // hangup
					// printf("Child process is ended by Hangup signal. \n");
					printf("child process get SIGHUP signal. \n");

				} else if (WTERMSIG(status) == 2) { // interrupt
					// printf("Child process is ended by Terminal interrupt. \n");
					printf("child process get SIGINT signal. \n");
				} else if (WTERMSIG(status) == 3) { // quit
					// printf("Child process is ended by Terminal quit. \n");
					printf("child process get SIGQUIT signal. \n");
				} else if (WTERMSIG(status) ==
					   4) { // illegal instruction
					// printf("Child process is ended by Illegal instruction. \n");
					printf("child process get SIGILL signal. \n");
				} else if (WTERMSIG(status) == 5) { // trap
					// printf("Child process is ended by Trace trap. \n");
					printf("child process get SIGTRAP signal. \n");
				} else if (WTERMSIG(status) == 6) { // abort
					// printf("Child process is aborted. \n");
					printf("child process get SIGABRT signal. \n");
				} else if (WTERMSIG(status) == 7) { // bus
					// printf("Child process is ended by Bus error. \n");
					printf("child process get SIGBUS signal. \n");
				} else if (WTERMSIG(status) == 8) { // floating
					// printf("Child process is ended by Floating point exception. \n");
					printf("child process get SIGFPE signal. \n");
				} else if (WTERMSIG(status) == 9) { // kill
					// printf("Child process is killed. \n");
					printf("child process get SIGKILL signal. \n");
				} else if (WTERMSIG(status) ==
					   11) { // segment fault
					// printf("Child process is ended by Invalid memory segment access. \n");
					printf("child process get SIGSEGV signal. \n");
				} else if (WTERMSIG(status) == 13) { // pipe
					// printf("Child process is ended by pipe error. \n");
					printf("child process get SIGPIPE signal. \n");
				} else if (WTERMSIG(status) == 14) { // alarm
					// printf("Child process is ended by alarm clock. \n");
					printf("child process get SIGALRM signal. \n");
				} else if (WTERMSIG(status) ==
					   15) { // termination
					// printf("Child process is ended by Termination. \n");
					printf("child process get SIGTERM signal. \n");
				}
			} else {
				printf("child process still continued \n");
			}
			exit(0);
		}
	}
}
