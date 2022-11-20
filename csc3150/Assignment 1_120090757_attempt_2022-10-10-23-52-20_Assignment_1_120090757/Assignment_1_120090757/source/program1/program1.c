#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork();

	if (pid < 0) {
		perror("fork error.");
		exit(1);
	} else {
		//child process
		if (pid == 0) {
			/* execute test program */
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			int i;
			char *arg[argc];
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			execve(arg[0], arg, NULL);
			perror("execve");
			exit(EXIT_FAILURE);
		}
		// parent process
		else {
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			/* wait for child process terminates */
			waitpid(-1, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD	signal\n");
			/* check child process' termination status */
			//Normal termination
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = 0\n");
			}
			//signal raise(except for SIGSTOP)
			else if (WIFSIGNALED(status)) {
				// refered from signal.h
				char *siganls[32] = {
					"zero",    "SIGHUP",    "SIGINT",
					"SIGQUIT", "SIGILL",    "SIGTRAP",
					"SIGABRT", "SIGBUS",    "SIGFPE",
					"SIGKILL", "SIGUSR1",   "SIGSEGV",
					"SIGUSR2", "SIGPIPE",   "SIGALRM",
					"SIGTERM", "SIGSTKFLT", "SIGCHLD",
					"SIGCONT", "SIGSTOP",   "SIGTSTP",
					"SIGTTIN", "SIGTTOU",   "SIGURG",
					"SIGXCPU", "SIGXFSZ",   "SIGVTALRM",
					"SIGPROF", "SIGWINCH",  "SIGIO",
					"SIGPWR",  "SIGSYS"
				};
				printf("Child Process get %s Signal.\n",
				       siganls[WTERMSIG(status)]);
			}
			//SIHSTOP raise
			else if (WIFSTOPPED(status)) {
				printf("Child Process Was Stoppped By SIGSTOP.\n");
			} else {
				printf("Child Process Continued.\n");
			}
			exit(0);
		}
		return 0;
	}
}
