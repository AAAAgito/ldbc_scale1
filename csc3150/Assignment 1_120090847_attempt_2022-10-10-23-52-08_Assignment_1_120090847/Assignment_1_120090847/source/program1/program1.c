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
	pid=fork();
	if(pid < 0){
		printf("Fork error!\n");
	}
	else{
		if(pid == 0){
			int i;
			char *arg[argc];
			//printf("I'm the Child Process, my pid = %d\n",getpid());
			for(i=0;i<argc-1;i++){
				arg[i]=argv[i+1];
			}
			arg[argc-1]=NULL;

			printf("I'm the Child Process, my pid = %d\n",getpid());
			printf("Child process start to execute test program:\n");
			execve(arg[0],arg,NULL);
			//raise(SIGCHLD);
		}else{
			printf("I'm the Parent Process, my pid = %d\n",getpid());
			//wait(&status);
			waitpid(pid,&status,WUNTRACED);
			printf("Parent process receives the SIGCHLD signal\n");
			//below is the normal condition.
			if(WIFEXITED(status)){
				printf("Normal termination with EXIT STATUS = %d\n",WEXITSTATUS(status));
			}
			//below are exceptional conditions.
			//If execution failed conditions, use WTERMSIG to judge which signal child process raise.
			else if(WIFSIGNALED(status)){
				if(WTERMSIG(status)==6){
					printf("CHILD PROCESS raise SIGABRT signal\n");
					printf("CHILD EXECUTION FAILED by SIGABRT signal\n");
				}
				else if(WTERMSIG(status)==14){
					printf("CHILD PROCESS raise SIGALRM signal\n");
					printf("CHILD EXECUTION FAILED by SIGALRM signal\n");
				}
				else if(WTERMSIG(status)==7){
					printf("CHILD PROCESS raise SIGBUS signal\n");
					printf("CHILD EXECUTION FAILED by SIGBUS signal\n");
				}
				else if(WTERMSIG(status)==8){
					printf("CHILD PROCESS raise SIGFPE signal\n");
					printf("CHILD EXECUTION FAILED by SIGFPE signal\n");
				}
				else if(WTERMSIG(status)==1){
					printf("CHILD PROCESS raise SIGHUP signal\n");
					printf("CHILD EXECUTION FAILED by SIGHUP signal\n");
				}
				else if(WTERMSIG(status)==4){
					printf("CHILD PROCESS raise SIGILL signal\n");
					printf("CHILD EXECUTION FAILED by SIGILL signal\n");
				}
				else if(WTERMSIG(status)==2){
					printf("CHILD PROCESS raise SIGINT signal\n");
					printf("CHILD EXECUTION FAILED by SIGINT signal\n");
				}
				else if(WTERMSIG(status)==9){
					printf("CHILD PROCESS raise SIGKILL signal\n");
					printf("CHILD EXECUTION FAILED by SIGKILL signal\n");
				}
				else if(WTERMSIG(status)==13){
					printf("CHILD PROCESS raise SIGPIPE signal\n");
					printf("CHILD EXECUTION FAILED by SIGPIPE signal\n");
				}
				else if(WTERMSIG(status)==3){
					printf("CHILD PROCESS raise SIGQUIT signal\n");
					printf("CHILD EXECUTION FAILED by SIGQUIT signal\n");
				}
				else if(WTERMSIG(status)==11){
					printf("CHILD PROCESS raise SIGSEGV signal\n");
					printf("CHILD EXECUTION FAILED by SIGSEGV signal\n");
				}
				else if(WTERMSIG(status)==15){
					printf("CHILD PROCESS raise SIGTERM signal\n");
					printf("CHILD EXECUTION FAILED by SIGTERM signal\n");
				}
				else if(WTERMSIG(status)==5){
					printf("CHILD PROCESS raise SIGTRAP signal\n");
					printf("CHILD EXECUTION FAILED by SIGTRAP signal\n");
				}
			}
			else if(WIFSTOPPED(status)){
				printf("CHILD PROCESS STOPPED: %d\n", WSTOPSIG(status));
				printf("CHILD PROCESS STOPPED\n");
			}
			else{
				printf("CHILD PROCESS CONTINUED\n");
			}
			exit(0);

		}
	}
	return 0;
	/* execute test program */ 
	
	/* wait for child process terminates */
	
	/* check child process'  termination status */
	
	
}
