#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

	/* fork a child process */
	int status;
	pid_t pid = fork();
	
	if(pid==-1){
		perror("fork");
		exit(1);

	}
	else{

	
	/* execute test program */ 
	if(pid==0){
		int i;
		char *arg[argc];
		printf("I'm the Child Process, my pid = %d \n", getpid());

		for(i=0;i<argc-1;i++){
			arg[i]=argv[i+1];
	}
		arg[argc-1]=NULL;


		printf("Child process start to execute test program:\n");

		execve(arg[0],arg,NULL);

	}
	/* wait for child process terminates */
	else{


	printf("I'm the Parent Process, my pid = %d \n", getpid());


	wait(&status);

	/* check child process'  termination status */

	printf("Parent process receives SIGCHILD signal\n");


	if(WIFEXITED(status)){
		printf("Normal termination with EXIT STATUS: %d\n",WEXITSTATUS(status));

	}else if(WIFSIGNALED(status)){
		printf("Child execution failed: %d\n",WTERMSIG(status));

	}else if(WIFSTOPPED(status)){
		printf("Child process stopped :%d\n",WSTOPSIG(status));
	}else{
		printf("Child process continued\n");
	}
	exit(0);
	}
	}
	return 0;
}
