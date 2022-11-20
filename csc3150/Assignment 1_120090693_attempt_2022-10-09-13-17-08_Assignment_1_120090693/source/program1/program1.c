#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

	pid_t pid;
	int status;
	
	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		// child
		if (pid == 0) {
		/* execute test program */
		printf("I'm the Child process.  my pid is %d\n", getpid());
		printf("Child process starts to execute test program:\n");
		execve(argv[1], argv + 1, NULL);
		perror("ececve");
		exit(1);
		}

		// parent
		else {
		/* wait for child process terminates */
		printf("I'm the Parent Process, my pid = %d\n", getpid());
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives the SIGCHLD signal\n");

		/* check child process'  termination status */
		// normal exit
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
				WEXITSTATUS(status));
		}
		// abnormal exit
		else if (WIFSIGNALED(status)) {
			int sig = WTERMSIG(status);
			switch (sig) {
			case 6: 
			printf("child process get SIGABRT signal\n");
			break;
			case 14: 
			printf("child process get SIGALRM signal\n");
			break;
			case 7: 
			printf("child process get SIGBUS signal\n");
			break;
			case 8: 
			printf("child process get SIGFPE signal\n");
			break;
			case 1:
			printf("child process get SIGHUP signal\n");
			break;
			case 4: 
			printf("child process get SIGILL signal\n");
			break;
			case 2:
			printf("child process get SIGINT signal\n");
			break;
			case 9: 
			printf("child process get SIGKILL signal\n");
			break;
			case 13:
			printf("child process get SIGPIPE signal\n");
			break;
			case 3:
			printf("child process get SIGQUIT signal\n");
			break;
			case 11:
			printf("child process get SIGSEGV signal\n");
			break;
			case 15:
			printf("child process get SIGTERM signal\n");
			break;
			case 5:
			printf("child process get SIGTRAP signal\n");
			break;
			default:
			printf("child process get ???? signal\n");
			break;
			}
		} else if (WIFSTOPPED(status)) {
			if (WSTOPSIG(status) == SIGSTOP) {
			printf("child process get SIGSTOP signal\n");
			} else {
			printf("child process get ???? signal\n");
			}
			printf("CHILD PROCESS STOPPED\n");
		} else {
			printf("CHILD PROCESS CONTINUED\n");
		}
		exit(0);
		}
	}
}