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
	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1) // fork unsuccessfully
	{
		perror("fork");
		exit(1);
	} else {
		int status;
		if (pid == 0) // Child process
		{
			char *arg[argc]; // file array for execution
			for (int i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("I'm the Child Process, my pid = %d, myppid = %d\n ",
			       getpid(), getppid());
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);
			exit(SIGCHLD);
		} else // Parent process
		{
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			waitpid(-1, &status, WUNTRACED);
			if (WIFEXITED(
				    status)) // The child process terminates normally
			{
				printf("Parent process receives SIGCHLD signal\n");
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(
					   status)) // The child process is terminated by a signal
			{
				int signal = WTERMSIG(
					status); // Get the signal code that aborts the child process
				printf("Parent process receives SIGCHLD signal\n");
				switch (signal) {
				case 1:
					printf("child process get SIGHUP signal\n");
					break;
				case 2:
					printf("child process get SIGINT signal\n");
					break;
				case 3:
					printf("child process get SIGQUIT signal\n");
					break;
				case 4:
					printf("child process get SIGILL signal\n");
					break;
				case 5:
					printf("child process get SIGTRAP signal\n");
					break;
				case 6:
					printf("child process get SIGABRT signal\n");
					break;
				case 7:
					printf("child process get SIGBUS signal\n");
					break;
				case 8:
					printf("child process get SIGFPE signal\n");
					break;
				case 9:
					printf("child process get SIGKILL signal\n");
					break;
				case 10:
					printf("child process get SIGUSR1 signal\n");
					break;
				case 11:
					printf("child process get SIGSEGV signal\n");
					break;
				case 12:
					printf("child process get SIGUSR2 signal\n");
					break;
				case 13:
					printf("child process get SIGPIPE signal\n");
					break;
				case 14:
					printf("child process get SIGALRM signal\n");
					break;
				case 15:
					printf("child process get SIGTERM signal\n");
					break;
				case 16:
					printf("child process get SIGSTKFLT signal\n");
					break;
				case 17:
					printf("child process get SIGCHLD signal\n");
					break;
				case 18:
					printf("child process get SIGCONT signal\n");
					break;
				case 19:
					printf("child process get SIGSTOP signal\n");
					break;
				case 20:
					printf("child process get SIGTSTP signal\n");
					break;
				case 21:
					printf("child process get SIGTTIN signal\n");
					break;
				case 22:
					printf("child process get SIGTTOU signal\n");
					break;
				case 23:
					printf("child process get SIGURG signal\n");
					break;
				case 24:
					printf("child process get SIGXCPU signal\n");
					break;
				case 25:
					printf("child process get SIGXFSZ signal\n");
					break;
				case 26:
					printf("child process get SIGVTALRM signal\n");
					break;
				case 27:
					printf("child process get SIGPROF signal\n");
					break;
				case 28:
					printf("child process get SIGWINCH signal\n");
					break;
				case 29:
					printf("child process get SIGIO signal\n");
					break;
				case 30:
					printf("child process get SIGPWR signal\n");
					break;
				case 31:
					printf("child process get SIGSYS signal\n");
					break;
				case 34:
					printf("child process get SIGTMIN signal\n");
					break;
				};
			} else if (WIFSTOPPED(status)) // Child process stopped
			{
				printf("Parent process receives SIGCHLD signal\n");
				printf("CHILD PROCESS STOPPED\n");
			} else {
				printf("Child PROCESS CONTINUE\n");
			}
			exit(0);
		}
	}
	return 0;
}
