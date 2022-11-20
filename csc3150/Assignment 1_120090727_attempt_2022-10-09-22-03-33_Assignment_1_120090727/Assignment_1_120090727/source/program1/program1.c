#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

// Retrieve POSIX signal name from signal number
char *get_signal_name(int signum)
{
	static char *signame[] = {
		"NULL",    "SIGHUP",    "SIGINT",  "SIGQUIT",  "SIGILL",
		"SIGTRAP", "SIGABRT",   "SIGBUS",  "SIGFPE",   "SIGKILL",
		"SIGUSR1", "SIGSEGV",   "SIGUSR2", "SIGPIPE",  "SIGALRM",
		"SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT",  "SIGSTOP",
		"SIGTSTP", "SIGTTIN",   "SIGTTOU", "SIGURG",   "SIGXCPU",
		"SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO",
		"SIGPWR",  "SIGSYS"
	};
	if (signum < 0)
		signum = -signum;
	if (signum >= 32)
		signum = 0;
	return signame[signum];
}

// Print out information when parent process receives SIGCHLD signal
void sigchld_handler()
{
	printf("Parent process receives SIGCHLD signal\n");
}

int main(int argc, char *argv[])
{
	/* fork a child process */
	printf("Process start to fork\n");
	pid_t pid = fork();
	signal(SIGCHLD, &sigchld_handler);

	if (pid == 0) {
		// Entry point for child process
		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");
		int i;
		char *arg[argc];
		for (i = 0; i < argc; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;
		/* execute test program */
		int c = execve(arg[0], arg, NULL);
		exit(0);
	} else if (pid == -1) {
		// Entry point for occurrance of error
		printf("Fork failed, exiting...\n");
		exit(1);
	} else {
		// Entry point for the parent process, the pid is the child process id
		printf("I'm the Parent Process, my pid = %d\n", getpid());

		/* wait for child process terminates */
		int child_status;
		waitpid(pid, &child_status, WUNTRACED);

		/* check child process' termination status */
		if (WIFEXITED(child_status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(child_status));
		} else if (WIFSIGNALED(child_status)) {
			printf("child process get %s signal\n",
			       get_signal_name(WTERMSIG(child_status)));
		} else if (WIFSTOPPED(child_status)) {
			printf("child process get %s signal\n",
			       get_signal_name(WSTOPSIG(child_status)));
		} else {
			printf("CHILD PROCESS CONTINUED\n"); // This won't happen after waitpid
		}
		exit(0);
	}
	return 0;
}