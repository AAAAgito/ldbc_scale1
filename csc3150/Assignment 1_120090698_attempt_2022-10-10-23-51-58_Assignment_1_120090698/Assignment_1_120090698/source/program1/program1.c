#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

	/* fork a child process */

	int status;
	pid_t pid;

	printf("Process start to fork\n");
	pid = fork();

	if(pid==-1){
		perror("fork");
		exit(1);
	}
	
	/* execute test program */ 
	else{
		if(pid==0){
			int i;
			char *arg[argc];
			printf("I'm the Child Process, my pid = %d\n", getpid());
			printf("Child process start to execute test program:\n");
			for(i=0;i<argc-1;i++){
				arg[i] = argv[i+1];
			}
			arg[argc-1] = NULL;
			execve(arg[0],arg,NULL);
			perror("execve");
			exit(EXIT_FAILURE);
		}
	
	/* wait for child process terminates */
		else{

			printf("I'm the Parent Process, my pid = %d\n", getpid());
			waitpid(pid,&status,WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");

			
	/* check child process'  termination status */
			if(WIFEXITED(status)){
				printf("Normal termination with EXIT STATUS = %d\n",WEXITSTATUS(status));
			}
			else if(WIFSIGNALED(status)){
				printf("CHILD EXECUTION FAILED: %d\n", WTERMSIG(status));
			}
			else if(WIFSTOPPED(status)){
				printf("CHILD PROCESS STOPPED: %d\n", WSTOPSIG(status));
			}
			else{
				printf("CHILD PROCESS CONTINUED\n");
			}
			exit(0);
		}
	}
	return 0;
}
