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
	pid=fork();

	if(pid==-1){
		perror("fork");
		printf("Fork error!\n");
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
			printf("I'm the Child Process, my pid = %d\n",getpid());
			printf("Child process start to execute test program:\n");
			execve(arg[0],arg,NULL);
			perror("execve");
			exit(EXIT_FAILURE);
		}
		else{

			printf("I'm the Parent Process, my pid = %d\n",getpid());
			waitpid(pid,&status,WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");
			if(WIFEXITED(status)){
				printf("Normal termination with EXIT STATUS = %d\n",WEXITSTATUS(status));
			}else if(WIFSIGNALED(status)){
				switch(WTERMSIG(status)) {
					case SIGHUP: //hangup (1)
						printf("child process get SIGHUP signal\nchild process is hung up\n");
						break;
					case SIGINT: //interrupt (2)
						printf("child process get SIGINT signal\nchild process is interrupted\n");
						break;
					case SIGQUIT: //quit (3)
						printf("child process get SIGQUIT signal\nchild process is quitted\n");
						break;
					case SIGILL: //illegal_instr (4)
						printf("child process get SIGILL signal\nchild process executes illegal instruction\n");
						break;
					case SIGTRAP: //trap (5)
						printf("child process get SIGTRAP signal\nchild process is trapped\n");
						break;
					case SIGABRT: //abort (6)
						printf("child process get SIGABRT signal\nchild process is aborted\n");
						break;
					case SIGBUS: //bus (7)
						printf("child process get SIGBUS signal\nchild process encounters bus error\n");
						break;
					case SIGFPE: //floating (8)
						printf("child process get SIGFPE signal\nchild process encounters floating point error\n");
						break;
					case SIGKILL: //kill (9)
						printf("child process get SIGKILL signal\nchild process is killed\n");
						break;
					case SIGSEGV: //segment_fault (11)
						printf("child process get SIGSEGV signal\nchild process encounters segmentation fault\n");
						break;
					case SIGPIPE: //pipe (13)
						printf("child process get SIGPIPE signal\nchild process encounters broken pipe\n");
						break;
					case SIGALRM: //alarm (14)
						printf("child process get SIGALRM signal\nchild process is alarmed\n");
						break;
					case SIGTERM: //terminate (15)
						printf("child process get SIGTERM signal\nchild process is terminated\n");
						break;
					default: //other signals
						printf("child process get other signals\n");
						break;
				}
				printf("CHILD EXECUTION FAILED\n");
        
			}else if(WIFSTOPPED(status)){
				printf("CHILD PROCESS STOPPED: %d\n",WSTOPSIG(status));
			}else{
				printf("CHILD PROCESS CONTINUED\n");
			}
			exit(0);
		}

	}
	/* execute test program */ 
	
	/* wait for child process terminates */
	
	/* check child process'  termination status */
	
	return 0;
}
