#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Function as a dictionary to look the name of the signal
const char *LinuxSingal(int i)
{
	if (i == 1) {
		char *Singal = "SIGHUP";
		return Singal;
	} else if (i == 2) {
		char *Singal = "SIGINT";
		return Singal;
	} else if (i == 3) {
		char *Singal = "SIGQUIT";
		return Singal;
	} else if (i == 4) {
		char *Singal = "SIGILL";
		return Singal;
	} else if (i == 5) {
		char *Singal = "SIGTRAP";
		return Singal;
	} else if (i == 6) {
		char *Singal = "SIGABRT";
		return Singal;
	} else if (i == 7) {
		char *Singal = "SIGBUS";
		return Singal;
	} else if (i == 8) {
		char *Singal = "SIGFPE";
		return Singal;
	} else if (i == 9) {
		char *Singal = "SIGKILL";
		return Singal;
	} else if (i == 11) {
		char *Singal = "SIGSEGV";
		return Singal;
	} else if (i == 13) {
		char *Singal = "SIGPIPE";
		return Singal;
	} else if (i == 14) {
		char *Singal = "SIGALRM";
		return Singal;
	} else if (i == 15) {
		char *Singal = "SIGTERM";
		return Singal;
	} else if (i == 16) {
		char *Singal = "SIGSTKFLT";
		return Singal;
	} else if (i == 17) {
		char *Singal = "SIGCHLD";
		return Singal;
	} else if (i == 18) {
		char *Singal = "SIGCONT";
		return Singal;
	} else if (i == 19) {
		char *Singal = "SIGSTOP";
		return Singal;
	}
	return "";
}

// The following code is quoted and modified from Tutorial2's PPT
int main(int argc, char *argv[])
{
	pid_t pid;
	int status;

	printf("Process start to fork\n");

	/* fork a child process */
	pid = fork();
	// Parent and child process runs concurrently after forking.

	// error situation
	if (pid == -1) {
		perror("fork");
		exit(1);
	}

	// Child process
	else {
		if (pid == 0) {
			int i;
			char *arg[argc];

			// getpid() returns PID of calling system.
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());

			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("Child process start to execute test program:\n");
			/* execute test program */
			execve(arg[0], arg, NULL);

			// Check if the child process is replaced by new program
			printf("Run the original child process(err)");
			perror("execve");
			exit(EXIT_FAILURE);
		}

		// Parent process
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());

			// To take all 15 cases into consideration, we choose WUNTRACED
			// If we choose 0, Some of the signals will be lost for some cases
			// WUNTRACED reports on stopped child processes as well as terminated ones
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");

			/* check child process'  termination status */
			// Check if child process exits normally
			if (WIFEXITED(status)) {
				// WEXITSTATUS(status) Get status value of child process
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			}

			// Check if child process received a terminating signal
			else if (WIFSIGNALED(status)) {
				// WTERMSIG(status) Get status value for child progress’ terminating
				// signal
				printf("child process get %s signal\n",
				       LinuxSingal(WTERMSIG(status)));
			}

			else if (WIFSTOPPED(status)) {
				printf("child process get %s signal\n",
				       LinuxSingal(WSTOPSIG(status)));
			}

			else {
				printf("child process continued\n");
			}

			exit(0);
		}
	}

	return 0;
}
