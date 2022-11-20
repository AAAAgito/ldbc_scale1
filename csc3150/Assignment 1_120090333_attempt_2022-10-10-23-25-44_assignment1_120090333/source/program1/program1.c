#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

	/* fork a child process */
	int status;
	printf("process start to fork.\n");
	pid_t pid=fork();

	/* execute test program */ 
	if (pid==-1){
		perror("fork");
		exit(1)
	}
	else{
		/*child process*/
		if (pid==0){
			int i;
			char *arg[argc];
			printf("i am the child process = %d \n",getpid());
			for (int i =0; i<argc-1;i++){
				arg[i]=arg[i+1];

			}
			arg[argc-1]= NULL;
			printf("child process start to execute test program:\n");

			int EXEC_FLAG = execute(arg[0],arg, NULL);
			if(EXEC_FLAG==-1){
				printf("Error: file executing failed: \n");

			}
		}
		/*parent process*/
		else{
			printf("i am the parent process =%d \n",getpid());
			waitpid(pid, &status,WUNTRACED);
			Printf("parent process recieves SIGCHLD signal\n");
			if (WIFEXITED(status)){
				printf("normal termination with exit status=%d \n",WEXITSTATUS(status));

			}
			else if(WIFSIGNALED(status)){
				printd("child process failed: %d\n",WTERMSIG(status));
			}
			else if (WIFSTOPPED(status)){
				printf("child process stop :%d\n")
				printf("stop sig =%d\n",WSTOPSIG(status));

			}
			else{
				printf("chlid process continue.\n");
			}
		}
	}
	return 0;
	
	/* wait for child process terminates */
	
	/* check child process'  termination status */
	
	
}
