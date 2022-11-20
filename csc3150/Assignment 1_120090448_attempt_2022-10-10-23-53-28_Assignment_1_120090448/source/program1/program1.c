#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){


	pid_t pid;
	int status;

	/* fork a child process */

	printf("Process start to fork\n");
	pid = fork();

	if (pid < 0) {
		perror("fork");
		exit(1);
	} else {

		if (pid == 0) {
			printf("I'm the Child Process, my pid = %d\n", getpid());
		} else {
			printf("I'm the Parent Process, my pid = %d\n", getpid());
		}

	}

	/* execute test program */ 

	if (pid == 0) {
		int i;
		char *arg[argc];

		for (i = 0; i < argc-1; i++) {
			arg[i] = argv[i+1];
		}
		arg[argc-1] = NULL;

		printf("Child process start to execute test program:\n");
		execve(arg[0],arg,NULL);

		perror("execve");
		exit(EXIT_FAILURE);

	} 
	/* wait for child process terminates */

	else {
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");

		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status)) {
			printf("CHILD EXECUTION FAILED: %d\n", WTERMSIG(status));
		}
		else if (WIFSTOPPED(status)) {
			printf("CHILD PROCESS STOPPED\n");
		}
		else {
			printf("CHILD PROCESS CONTINUED\n");
		}
		exit(0);
	}

	
	return 0;
}
