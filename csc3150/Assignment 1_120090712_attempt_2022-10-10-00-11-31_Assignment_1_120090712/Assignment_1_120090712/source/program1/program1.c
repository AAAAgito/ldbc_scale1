#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int status;
	pid_t pid;
	printf("Process strat to fork\n");

	/* fork a child process */
	pid = fork();
	if (pid < 0) {
		perror("The creation of a child process was unsuccessful");
		exit(1);
	}
	// child process
	else if (pid == 0) {
		char *arg[argc];
		printf("I'm the Child Process, my pid = %d\n", getpid());
		/* execute test program */
		for (int i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;

		printf("Child process start to execute test program:\n");
		execve(arg[0], arg, NULL);
		perror("Something wrong with the execve\n");
		exit(EXIT_FAILURE);
	}
	// parent process
	else {
		printf("I'm the Parent Process, my pid = %d\n", getpid());
		/* wait for child process terminates */
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");
		/* check child process'  termination status */
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		} else if (WIFSTOPPED(status)) {
			printf("child process get SIGSTOP siganl\n");
		} else if (WIFSIGNALED(status)) {
			switch (WTERMSIG(status)) {
			case 1:
				printf("child process get SIGHUP siganl\n");
				break;
			case 2:
				printf("Child process get SIGINT sigal\n");
				break;
			case 3:
				printf("Child process get SIGQUIT siganl\n");
				break;
			case 4:
				printf("Child process get SIGILL siganl\n");
				break;
			case 5:
				printf("Child process get SIGTRAP siganl\n");
				break;
			case 6:
				printf("Child process get SIGABRT siganl\n");
				break;
			case 7:
				printf("Child process get SIGBUS siganl\n");
				break;
			case 8:
				printf("Child process get SIGFPE siganl\n");
				break;
			case 9:
				printf("Child process get SIGKILL siganl\n");
				break;
			case 11:
				printf("Child process get SIGSEGV siganl\n");
				break;
			case 13:
				printf("Child process get SIGPIPE siganl\n");
				break;
			case 14:
				printf("Child process get SIGALRM siganl\n");
				break;
			case 15:
				printf("Child process get SIGTERM siganl\n");
				break;
			default:
				printf("There child process terminates in an unknown way\n");
				break;
			}
		} else {
			printf("The child process is still running\n");
		}
		exit(0);
	}

	return 0;
}
