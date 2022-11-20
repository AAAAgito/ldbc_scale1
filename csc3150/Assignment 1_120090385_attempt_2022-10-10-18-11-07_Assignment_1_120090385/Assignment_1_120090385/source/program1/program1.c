#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){
    
	char *signal[]={"the","SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP","SIGABRT","SIGBUS","SIGFPE","SIGKILL","SIGUSR1","SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM","SIGTERM","unused","SIGCHLD","SIGCONT","SIGSTOP","SIGTSTP","SIGTTIN","SIGTTOU","SIGURG","SIGXCPU","SIGXFSZ","SIGVTALRM","SIGPROF","SIGWINCH","SIGIO","SIGPWR","SIGSYS"};

	/* fork a child process */
	pid_t pid;
	int status;
	int state;

	printf("Process start to fork\n");
	
	//pid_t pid = fork();
	pid=fork();
	// fflush(stdout);

	if(pid==-1){
		printf("Fork error:\n");
		perror("fork");
		exit(1);
	}
	else{
		//child
        if(pid==0){

			int i;
			char *arg[argc];

			sleep(10);

			printf("I'm the Child Process, my pid = %d\n" ,getpid());
			
			/* execute test program */ 

			for(i=0; i<argc-1; i++){
				arg[i]=argv[i+1];
			}
			arg[argc-1]=NULL;

			printf("Child Process start to execute test program:\n");

			execve(arg[0],arg,NULL);
			raise(SIGCHLD);
			perror("execve");
			exit(EXIT_FAILURE);
		}
		//parent
		else{

			printf("I'm the Parent Process, my pid = %d\n" ,getpid());

			/* wait for child process terminates */
			
			waitpid(pid, &status, WUNTRACED);

			printf("Parent process receives SIGCHLD signal\n");

			/* check child process'  termination status */

			if(WIFEXITED(status)){
				printf("Normal termination with EXIT STATUS = %d\n" ,WEXITSTATUS(status));
			}
			else if(WIFSIGNALED(status)){
				printf("Child process get %s signal\n",signal[WTERMSIG(status)]);
				printf("CHILD EXECUTION FAILED: %d\n" ,WTERMSIG(status));
			}
			else if(WIFSTOPPED(status)){
				printf("Child process get %s signal\n",signal[WSTOPSIG(status)]);
				printf("CHILD PROCESS STOPPED: %d\n" ,WSTOPSIG(status));
			}
			else{
				printf("CHILD PROCESS CONTINUED\n");
			}
			exit(1);
		}
	}
	return 0;
}
