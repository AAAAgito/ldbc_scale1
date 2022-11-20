#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

	/* fork a child process */
	pid_t pid;
	int status;
	printf("Process start to fork\n");
	pid = fork();

	if (pid < 0) {
		// fail fork
		perror("Fork failed");
		exit(1);
	} else if (pid == 0) {
		// Child process
		sleep(1);
		printf("I'm the Child Process: my pid = %d\n", getpid());
		/* execute test program */ 
		int i;
		char *arg[argc];
		for(i=0;i<argc-1;i++){
			arg[i] = argv[i+1];
		}
		arg[argc-1] = NULL;
		printf("Child process start to execute test program:\n");
		execve(arg[0], arg, NULL);
		raise(SIGCHLD);
	} else {
		// Parent process
		printf("I'm the Parent Process: my pid = %d\n", getpid());
		/* wait for child process terminates */
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");
		/* check child process'  termination status */
		if(WIFEXITED(status)){
			printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
		}
		else if(WIFSTOPPED(status)){
			printf("CHILD PROCESS STOPPED\n");
		}
		else{
			exit(0);
		}

	}

	return 0;
}
