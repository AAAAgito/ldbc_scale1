#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

// function prototype, implemented below
char *formatSigName(int sig);

int main(int argc, char *argv[])
{
	/* fork a child process */
	int status;
	printf("Process start to fork\n");
	pid_t pid = fork();

	if (pid == 0) {
		// child goes here
		printf("I'm the Child Process, my pid = %d \n", getpid());

		/* execute test program */
		printf("Child process start to execute test program:\n");
		// special test use
		// execve("./abort", argv+1, NULL);
		execve(argv[1], argv + 1, NULL);

	} else {
		// parent goes here
		printf("I'm the Parent Process, my pid = %d \n", getpid());

		/* wait for child process terminates */
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");

		/* check child process' termination status */
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("child process get %s signal\n",
			       formatSigName(WTERMSIG(status)));
		} else if (WIFSTOPPED(status)) {
			printf("child process get %s signal\n",
			       formatSigName(WSTOPSIG(status)));
		} else {
			printf("continue!\n");
		}
		return 0;
	}
}

char *formatSigName(int status)
{
	switch (status) {
	case 1:
		return "SIGHUP";
	case 2:
		return "SIGINT";
	case 3:
		return "SIGQUIT";
	case 4:
		return "SIGILL";
	case 5:
		return "SIGTRAP";
	case 6:
		return "SIGABRT";
	case 7:
		return "SIGBUS";
	case 8:
		return "SIGFPE";
	case 9:
		return "SIGKILL";
	case 10:
		return "SIGUSR1";
	case 11:
		return "SIGSEGV";
	case 12:
		return "SIGUSR2";
	case 13:
		return "SIGPIPE";
	case 14:
		return "SIGALRM";
	case 15:
		return "SIGTERM";
	case 16:
		return "SIGSTKFLT";
	case 17:
		return "SIGCHLD";
	case 18:
		return "SIGCONT";
	case 19:
		return "SIGSTOP";
	case 20:
		return "SIGTSTP";
	case 21:
		return "SIGTTIN";
	case 22:
		return "SIGTTOU";
	case 23:
		return "SIGURG";
	case 24:
		return "SIGXCPU";
	case 25:
		return "SIGXFSZ";
	case 26:
		return "SIGVTALRM";
	case 27:
		return "SIGPROF";
	case 28:
		return "SIGWINCH";
	case 29:
		return "SIGIO";
	default:
		return "UNKNOWN CODE!";
	}
}
