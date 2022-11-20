#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

	/* fork a child process */
	
	/* execute test program */ 
	
	/* wait for child process terminates */
	
	/* check child process'  termination status */
	pid_t pid;
	printf("Process start to fork\n");
	int status;
	pid=fork();

	if (pid==-1){
		perror("fork");
		exit(1);
	}else{
		if (pid==0){
			int i;
			char *arg[argc];
			printf("I'm the Parent Process, my pid = %d\n",getppid());
			printf("I'm the Child Process, my pid = %d\n",getpid());
			for (i=0;i<argc-1;i++){
				arg[i]=argv[i+1];
			}
			arg[argc-1]=NULL;
			execve(arg[0],arg,NULL);

			perror("execve");
			exit(EXIT_FAILURE);
		}else{
			waitpid(pid,&status,WUNTRACED);
			printf("Parent process receives SIGCHILD signal\n");
			if (WIFEXITED(status)){
				printf("Normal termination with EXIT STATUS = %d\n",WEXITSTATUS(status));
			}
			if (status==9){
				printf("child process get SIGKILL signal\n");
			}else if(status==134){
				printf("child process get SIGABRT signal\n");
			}else if(status==131){
				printf("child process get SIGQUIT signal\n");
			}else if(status==15){
				printf("child process get SIGTERM signal\n");
			}else if(WEXITSTATUS(status)==19){
				printf("child process get SIGSTOP signal\n");
			}else if(status==14){
				printf("child process get SIGALRM signal\n");
			}else if(status==135){
				printf("child process get SIGBUS signal\n");
			}else if(status==136){
				printf("child process get SIGFPE signal\n");
			}else if(status==1){
				printf("child process get SIGHUP signal\n");
			}else if(status==132){
				printf("child process get SIGILL signal\n");
			}else if(status==2){
				printf("child process get SIGINT signal\n");
			}else if(status==13){
				printf("child process get SIGPIPE signal\n");
			}else if(status==133){
				printf("child process get SIGTRAP signal\n");
			}else if(status==139){
				printf("child process get SIGSEGV signal\n");
			}			
			exit(0);
		}
	}
}

