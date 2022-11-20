#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <linux/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	char *name[] = {"SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE",
					"SIGKILL", "SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM",
					"SIGSTKFLT", "SIGCHID", "SIGCONT", "SIGSTOP", "SIGTSTP", "SIGTTIN", "SIGTTOU",
					"SIGURG", "SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO"};

	/* fork a child process */
	pid_t pid;
	printf("Process start to fork\n");
	pid = fork();
	/* execute test program */
	int status;
	if (pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else
	{
		// child process
		if (pid == 0)
		{
			int i;
			char *arg[argc];
			printf("I'm the Parent Process, my pid = %d\n", getppid());
			printf("I'm the Child Process, my pid = %d\n", getpid());
			for (i = 0; i < argc - 1; i++)
			{
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			printf("Child process start to execute test program\n");

			execve(arg[0], arg, NULL);
			printf("Continue to run original child process\n");
			perror("execve");
			exit(EXIT_FAILURE);
		}
		// parent process
		else
		{
			/* wait for child process terminates */
			wait(&status);

			printf("Parent process receives SIGCHID signal\n");
			/* check child process' termination status */
			if (WIFEXITED(status))
			{
				printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
			}
			else if (WIFSIGNALED(status))
			{
				// printf("CHILD EXECUTION FAILED: %d\n", WTERMSIG(status));
				printf("child process get %s signal\n", name[WTERMSIG(status) - 1]);
			}
			else if (WIFSTOPPED(status))
			{
				printf("child process get %s signal\n", name[WSTOPSIG(status) - 1]);
			}
			else
			{
				printf("child process get SIGCONT signal\n");
			}
			exit(0);
		}
	}
	return 0;
}
