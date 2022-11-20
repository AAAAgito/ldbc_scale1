#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

char* processTerminatedSignal[] = {
	"SIGABRT",      "SIGALRM",       "SIGBUS",      "SIGILL",      "SIGTRAP",
	"SIGHUP",     "SIGQUIT",        "SIGFPE",       "SIGKILL",     NULL,
    "SIGSEGV",         NULL,       "SIGPIPE",     "SIGINT",    "SIGTERM"
};

char* signalInfomation[] = {
	"is abort by abort signal",
	"is terminated by alarm signal",
	"gets bus error",
	"gets illegal instruction",
	"is terminated by trap signal",
    "is hang up by hangup signal",
    "is quited by quit signal",
	"gets floating point exception",
	"is killed by kill signal",
	NULL,
	"uses invalid memory reference",
	NULL,
	"writes to pipe with no readers",
    "is interrupted by interrupt signal",
	"is terminated by termaniation signal",
};

int main(int argc, char *argv[]){

	pid_t pid;
	int status;

	printf("Process start to fork\n");
	pid = fork();

	if(pid==-1){
		perror("fork");
		exit(1);
	}
	else{
		if(pid==0){
			printf("I'm the Child Process, my pid = %d\n", getpid());
			int i;
			char *arg[argc];
			
			for(i=0;i<argc-1;i++){
				arg[i]=argv[i+1];
			}
			arg[argc-1]=NULL;
 
			printf("Child process start to execute the program\n");
			execve(arg[0],arg,NULL);
			exit(EXIT_FAILURE);
		}

		else{
			printf("I'm the Parent Process, my pid = %d\n", getpid());

			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receiving the SIGCHLD signal\n");

			if(WIFEXITED(status)){
                printf("Normal termination with EXIT STATUS = %d\n",WEXITSTATUS(status));
            }
            else if(WIFSIGNALED(status)){
				int terminationStatus = WTERMSIG(status);
				if(terminationStatus>=1 && terminationStatus <=15 && processTerminatedSignal[terminationStatus-1]!=NULL){
					printf("child process get %s signal\n", processTerminatedSignal[terminationStatus-1]);
					printf("child process %s\n", signalInfomation[terminationStatus-1]);
				}
				else{
					printf("child process get a signal not in samples\n");
				}
                printf("CHILD EXECUTION FAILED!!\n");
            }
            else if(WIFSTOPPED(status)){
				int stopStatus = WSTOPSIG(status);
				if(stopStatus==SIGSTOP){
					printf("child process get SIGSTOP signal\n");
					printf("child process stopped\n");
				}
				else{
					printf("child process get a signal not in the samples\n");
					printf("child process stopped\n");
				}
                printf("CHILD PROCESS STOPPED\n");
            }
            else{
                printf("CHILD PROCESS CONTINUED\n");
            }
            exit(0);
		}
	}	
}