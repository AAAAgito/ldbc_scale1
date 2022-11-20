#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void output_signal(int signal)
{
	printf("Child process raised: ");
	switch (signal) {
	case 6:
		printf("%s\n", "SIGABRT");
		break;
	case 14:
		printf("%s\n", "SIGALRM");
		break;
	case 7:
		printf("%s\n", "SIGBUS");
		break;
	case 8:
		printf("%s\n", "SIGFPE");
		break;
	case 1:
		printf("%s\n", "SIGHUP");
		break;
	case 4:
		printf("%s\n", "SIGILL");
		break;
	case 2:
		printf("%s\n", "SIGINT");
		break;
	case 9:
		printf("%s\n", "SIGKILL");
		break;
	case 13:
		printf("%s\n", "SIGPIPE");
		break;
	case 3:
		printf("%s\n", "SIGQUIT");
		break;
	case 11:
		printf("%s\n", "SIGSEGV");
		break;
	case 19:
		printf("%s\n", "SIGSTOP");
		break;
	case 15:
		printf("%s\n", "SIGTERM");
		break;
	case 5:
		printf("%s\n", "SIGTRAP");
		break;

	default:
		printf("%s\n", "undefined!");
		break;
	}
}
int main(int argc, char *argv[])
{
	/* fork a child process */
	/* execute test program */
	/* wait for child process terminates */
	/* check child process'  termination status */

	printf("Process start to fork\n");
	pid_t pid = fork();
	int status;
	int child_pid;

	// Child:
	if (pid == 0) {
		printf("I'm the Child Process, my pid = %d\n",
		       child_pid = getpid());

		char *new_arg[argc];
		for (int i = 1; i < argc; i++)
			new_arg[i - 1] = argv[i];
		new_arg[argc - 1] = NULL;
		printf("Child process start to execute test program:\n");
		execve(new_arg[0], new_arg, NULL);
		perror("");
	} else // Parent
	{
		printf("I'm the Parent Process, my pid = %d\n", getpid());
		waitpid(child_pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");
		if (WIFEXITED(status))
			printf("Normal termination with EXIT STATUS = %d.\n",
			       WEXITSTATUS(status));
		else if (WIFSIGNALED(status)) {
			printf("Child execution failed!\n");
			output_signal(WTERMSIG(status));
		} else if (WIFSTOPPED(status)) {
			printf("Child execution stopped\n");
			output_signal(WSTOPSIG(status));
		}
		exit(0);
	}
	return 0;
}