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
	if(pid==-1){
		perror("fork");
		exit(1);
	}
	else{
		if(pid==0){
			int i;
			char* arg[argc];
			for(i=0;i<argc-1;i++){
				arg[i]=argv[i+1];
			}
			arg[argc-1]=NULL;
			printf("I'm the Child Process, my pid = %d\n", getpid());
			/* execute test program */ 
			printf("Child process start to execute test program\n");
			execve(arg[0],arg,NULL);
			// raise(SIGCHLD);
			exit(0);
		}
		else{
			printf("I'm the Parent Process, my pid =%d\n", getppid());
			/* wait for child process terminates */
			waitpid(pid,&status,WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");
			/* check child process'  termination status */
			if(WIFEXITED(status)){
				printf("Normal termination with EXIT STATUS = %d\n",WIFEXITED(status));
			}
			else if(WIFSTOPPED(status)){
				printf("child process get SIGSTOP signal.");
			}
			else
				switch(status){
					case 6  :
						printf("child process receives SIGABRT signal\n");
					break; 
					case 14  :
						printf("child process receives SIGALRM signal\n");
					break; 
					case 7  :
						printf("child process receives SIGBUS signal\n");
					break; 
					case 8  :
						printf("child process receives SIGFPE signal\n");
					break; 
					case 1  :
						printf("child process receives SIGHUP signal\n");
					break; 
					case 4  :
						printf("child process receives SIGILL signal\n");
					break; 
					case 2  :
						printf("child process receives SIGINT signal\n");
					break; 
					case 9  :
						printf("child process receives SIGKILL signal\n");
					break; 
					case 13  :
						printf("child process receives SIGPIPE signal\n");
					break; 
					case 11  :
						printf("child process receives SIGSEGV signal\n");
					break; 
					case 15  :
						printf("child process receives SIGTERM signal\n");
					break; 
					case 5  :
						printf("child process receives SIGTRAP signal\n");
					break; 
			}
			exit(0);
		}
		
		
	}

	
	
}
