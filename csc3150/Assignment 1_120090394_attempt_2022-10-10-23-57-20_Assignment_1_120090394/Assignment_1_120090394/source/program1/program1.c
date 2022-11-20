#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

int main(int argc, char *argv[]){  
	int status;
    pid_t pid;
    
    printf("Process start to fork\n");
    pid=fork();
    
    if(pid==-1){
        perror("fork");
        exit(1);
    }
    else{
        if(pid==0){
			int i;
            char *arg[argc];
            
            for(i=0;i<argc-1;i++){
                arg[i]=argv[i+1];
            }
            arg[argc-1]=NULL;
            printf("I'm the Child Process, my pid = %d\n", getpid());
			printf("Child process start to execute test program:\n");
            execve(arg[0],arg,NULL);
            exit(0);
        }
        
        else{
            printf("I'm the Parent Process, my pid = %d\n", getpid());
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives the SIGCHIL signal\n");
			
			if(WIFEXITED(status)){
				printf("Normal termination with EXIT STATUS = %d\n",WEXITSTATUS(status));
			}
			else if(WIFSIGNALED(status)){
				if(WTERMSIG(status)==1){
					printf("child process get SIGHUP signal\n");
					printf("child process is hung up\n");
				}
				if(WTERMSIG(status)==2){
					printf("child process get SIGINT signal\n");
					printf("child process is interrupted\n");
				}
				if(WTERMSIG(status)==3){
					printf("child process get SIGQUIT signal\n");
					printf("child process is quited\n");
				}
				if(WTERMSIG(status)==4){
					printf("child process get SIGILL signal\n");
					printf("child process is illegal\n");
				}
				if(WTERMSIG(status)==5){
					printf("child process get SIGTRAP signal\n");
					printf("child process is trapped\n");
				}
				if(WTERMSIG(status)==6){
					printf("child process get SIGABRT signal\n");
					printf("child process is aborted\n");
				}
				if(WTERMSIG(status)==7){
					printf("child process get SIGBUS signal\n");
					printf("child process is bussed\n");
				}
				if(WTERMSIG(status)==8){
					printf("child process get SIGFPE signal\n");
					printf("child process is floated\n");
				}
				if(WTERMSIG(status)==9){
					printf("child process get SIGKILL signal\n");
					printf("child process is killed\n");
				}
				if(WTERMSIG(status)==11){
					printf("child process get SIGSEGV signal\n");
					printf("child process is segment fault\n");
				}
				if(WTERMSIG(status)==13){
					printf("child process get SIGPIPE signal\n");
					printf("child process is piped\n");
				}
				if(WTERMSIG(status)==14){
					printf("child process get SIGALRM signal\n");
					printf("child process alarmed\n");
				}
				if(WTERMSIG(status)==15){
					printf("child process get SIGTERM signal\n");
					printf("child process is TERMINATED\n");
				}
				printf("CHILD EXECUTION FAILED\n");
			}
			else if(WIFSTOPPED(status)){
				if(WSTOPSIG(status)==19){
					printf("child process get SIGSTOP signal\n");
					printf("child process stopped\n");
				}
                printf("CHILD PROCESS STOPPED\n");
            }


            exit(0);
        }
    }
    
    return 0;
	
}
