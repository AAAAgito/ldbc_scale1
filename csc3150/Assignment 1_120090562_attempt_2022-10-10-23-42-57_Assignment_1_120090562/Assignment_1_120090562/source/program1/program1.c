#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

void sigprint(char *buffer, int signal)
{
	if (signal == SIGHUP)
		snprintf(buffer, 10, "SIGHUP");
	if (signal == SIGINT)
		snprintf(buffer, 10, "SIGINT");
	if (signal == SIGQUIT)
		snprintf(buffer, 10, "SIGQUIT");
	if (signal == SIGILL)
		snprintf(buffer, 10, "SIGILL");
	if (signal == SIGTRAP)
		snprintf(buffer, 10, "SIGTRAP");
	if (signal == SIGABRT)
		snprintf(buffer, 10, "SIGABRT");
	if (signal == SIGBUS)
		snprintf(buffer, 10, "SIGBUS");
	if (signal == SIGFPE)
		snprintf(buffer, 10, "SIGFPE");
	if (signal == SIGKILL)
		snprintf(buffer, 10, "SIGKILL");
	if (signal == SIGUSR1)
		snprintf(buffer, 10, "SIGUSR1");
	if (signal == SIGSEGV)
		snprintf(buffer, 10, "SIGSEGV");
	if (signal == SIGUSR2)
		snprintf(buffer, 10, "SIGUSR2");
	if (signal == SIGPIPE)
		snprintf(buffer, 10, "SIGPIPE");
	if (signal == SIGALRM)
		snprintf(buffer, 10, "SIGALRM");
	if (signal == SIGTERM)
		snprintf(buffer, 10, "SIGTERM");
	if (signal == SIGCHLD)
		snprintf(buffer, 10, "SIGCHLD");
	if (signal == SIGCONT)
		snprintf(buffer, 10, "SIGCONT");
	if (signal == SIGSTOP)
		snprintf(buffer, 10, "SIGSTOP");
	if (signal == SIGTSTP)
		snprintf(buffer, 10, "SIGTSTP");
	if (signal == SIGTTIN)
		snprintf(buffer, 10, "SIGTTIN");
	if (signal == SIGTTOU)
		snprintf(buffer, 10, "SIGTTOU");
	if (signal == SIGURG)
		snprintf(buffer, 10, "SIGURG");
	if (signal == SIGXCPU)
		snprintf(buffer, 10, "SIGXCPU");
	if (signal == SIGXFSZ)
		snprintf(buffer, 10, "SIGXFSZ");
	if (signal == SIGVTALRM)
		snprintf(buffer, 10, "SIGVTALRM");
	if (signal == SIGPROF)
		snprintf(buffer, 10, "SIGPROF");
	if (signal == SIGWINCH)
		snprintf(buffer, 10, "SIGWINCH");
	if (signal == SIGIO)
		snprintf(buffer, 10, "SIGIO");
	if (signal == SIGPWR)
		snprintf(buffer, 10, "SIGPWR");
	if (signal == SIGSYS)
		snprintf(buffer, 10, "SIGSYS");
}

int main(int argc, char *argv[])
{
	pid_t pid;
	char buffer[20];
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file-to-exec>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* fork a child process */
	fprintf(stdout, "Process start to fork\n");
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Fork Failed");
		exit(EXIT_FAILURE);
	}

	/* execute test program */
	if (pid == 0) {
		fprintf(stdout, "I'm the Child Process, my pid = %d\n",
			getpid());
		fprintf(stdout,
			"Child process start to execute test program:\n");
		char *new_env[] = { NULL };
		char *new_argv[] = { argv[1], NULL };
		execve(argv[1], new_argv, new_env);
		perror("execve");
		exit(EXIT_FAILURE);
	}

	/* wait for child process terminates */
	fprintf(stdout, "I'm the Parent Process, my pid = %d\n", getpid());

	int status;
	if (waitpid(-1, &status, WUNTRACED) < 0) {
		perror("wait");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Parent process receives SIGCHLD signal\n");

	/* check child process'  termination status */
	if (WIFSIGNALED(status)) {
		sigprint(buffer, WTERMSIG(status));
		fprintf(stdout, "child process get %s signal\n", buffer);
	}

	if (WIFSTOPPED(status)) {
		sigprint(buffer, WSTOPSIG(status));
		fprintf(stdout, "child process get %s signal\n", buffer);
	}

	if (WIFEXITED(status))
		fprintf(stdout, "Normal termination with EXIT STATUS = %d\n",
			WEXITSTATUS(status));

	return 0;
}