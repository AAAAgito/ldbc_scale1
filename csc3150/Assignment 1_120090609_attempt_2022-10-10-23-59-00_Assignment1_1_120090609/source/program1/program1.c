#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

char sigtrans[15][8]={"SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP","SIGABRT","SIGBUS","SIGFPE","SIGKILL","SIGUSR1","SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM","SIGTERM"};

int main(int argc, char *argv[]){

	/* fork a child process */
	printf("Process start to fork\n");
	int status;
	pid_t pid = fork();
 
	if (pid < 0) printf("Fork error!\n");
    else{
        //Child process
        if (pid == 0){
            int i;
            char *arg[argc];
            printf("I'm the Child Process, my pid = %d\n", getpid());
            for (int i = 0; i < argc; i++)
                arg[i] = argv[i+1];
            arg[argc - 1] = NULL;

			/* execute test program */
            printf("Child process start to execute test program:\n");
            execve(arg[0], arg, NULL);
            
			printf("Continue to run original child process!\n");
            perror("execve");
            exit(EXIT_FAILURE);
        }
        //Parent process
        else{
            printf("I'm the Parent Process, my pid = %d\n", getpid());
			
			/* wait for child process terminates */
            waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");
			
			/* check child process'  termination status */
			if (WIFEXITED(status)){
                printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
            }
            else if (WIFSTOPPED(status)){
                printf("CHILD PROCESS STOPPED\n");
            //     printf("Child process get SIGSTOP signal\n");
            // }
            // else{
            //     printf("Child process get SIGABRT signal\n");
            }
            status = status%128;
            if ((status < 16) && (status > 0)){
                printf("Child process get %s signal\n",sigtrans[status-1]);
            }
            exit(0);
        }
    }
	
	return 0;
}
