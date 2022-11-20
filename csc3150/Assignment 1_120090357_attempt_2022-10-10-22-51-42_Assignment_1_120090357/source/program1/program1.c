#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

char* processTerminatedSignal[] = {
	"SIGHUP",      "SIGINT",       "SIGQUIT",      "SIGILL",      "SIGTRAP",
	"SIGABRT",     "SIGBUS",        "SIGFPE",       "SIGKILL",     NULL,
    "SIGSEGV",         NULL,       "SIGPIPE",     "SIGALRM",    "SIGTERM"
};

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
		//child process
		if(pid==0){
			printf("I'm the Child Process, my pid = %d\n", getpid());
			int i;
			char *arg[argc];
			//copy the execute file
			for(i=0;i<argc-1;i++){
				arg[i]=argv[i+1];
			}
			arg[argc-1]=NULL;

			/* execute test program */ 
			printf("Child process start to execute the program\n");
			execve(arg[0],arg,NULL);
			exit(EXIT_FAILURE);
		}

		//parent process
		else{
			printf("I'm the Parent Process, my pid = %d\n", getpid());

			/* wait for child process terminates */
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receiving the SIGCHLD signal\n");

			/* check child process'  termination status */
			//normal termination
			if(WIFEXITED(status)){
                printf("Normal termination with EXIT STATUS = %d\n",WEXITSTATUS(status));
            }
			//process failure
            else if(WIFSIGNALED(status)){
				int terminationStatus = WTERMSIG(status);
				if(terminationStatus>=1 && terminationStatus <=15 && processTerminatedSignal[terminationStatus-1]!=NULL){
					printf("child process get %s signal\n", processTerminatedSignal[terminationStatus-1]);
				}
				else{
					printf("child process get a signal not in samples\n");
				}
            }
			//process stopped
            else if(WIFSTOPPED(status)){
				int stopStatus = WSTOPSIG(status);
				if(stopStatus==SIGSTOP){
					printf("child process get SIGSTOP signal\n");
				}
				else{
					printf("child process get a signal not in the samples\n");
				}
            }
            exit(0);
		}
	}	
}