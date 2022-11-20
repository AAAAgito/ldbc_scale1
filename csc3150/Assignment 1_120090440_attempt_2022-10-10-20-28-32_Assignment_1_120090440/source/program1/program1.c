#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	// 6=abort, 14=alarm, 7=bus, 8=floating, 1=hangup, 4=illegal_instr,
	// 2=interrupt, 9=kill
	// 13=pipe, 3=quit, 11=segment_fault, 15=terminate, 5=trap
	char *TermSig[] = { NULL,     "SIGHUP",  "SIGINT",  "SIGQUIT",
			    "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS",
			    "SIGFPE", "SIGKILL", NULL,      "SIGSEGV",
			    NULL,     "SIGPIPE", "SIGALRM", "SIGTERM" };

	char *TermReason[] = {
		NULL,
		"child process is hung up\n",
		"child process gets interrupt from input keyboard\n",
		"child process has terminal quit\n",
		"child process has illegal instruction\n",
		"child process reach a breakpoint\n",
		"child process is aborted\n",
		"child process has bus error\n",
		"child process has floating point exception\n",
		"child process is killed\n",
		NULL,
		"child process has invalid memory segment access\n",
		NULL,
		"child process writes on a pipe with no reader\n",
		"child process releases alarm signal\n",
		"child process terminates\n"
	};

	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		// Child process
		if (pid == 0) {
			int i;
			char *arg[argc];

			printf("I'm the Child Process, my pid = %d\n",
			       getpid());

			for (int i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			/* execute test program */
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);

			perror("execve");
			exit(EXIT_FAILURE);
		}

		// Parent process
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());

			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");

			/* check child process'  termination status */

			// normal
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			}

			// failure
			else if (WIFSIGNALED(status)) {
				int sig = WTERMSIG(status);
				if (sig >= 1 && sig <= 15 &&
				    TermSig[sig] != NULL) {
					// printf("%s", TermReason[sig]);
					printf("child process get %s signal\n",
					       TermSig[sig]);
				} else {
					printf("child process get unknown signal\n");
				}
				printf("CHILD EXECUTION FAILED: %d\n", sig);
			}

			// stopped
			else if (WIFSTOPPED(status)) {
				if (WSTOPSIG(status) == SIGSTOP) {
					printf("child process get SIGSTOP signal\n");
				} else {
					printf("child process get unknown signal\n");
				}
				printf("CHILD PROCESS STOPPED: %d\n",
				       WSTOPSIG(status));
			} else {
				printf("CHILD PROCESS CONTINUED\n");
			}
			exit(0);
		}
	}
}
