#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

	/* fork a child process */
	printf("Process start to fork\n");
	int status;
	pid_t pid = fork();
 
	if (pid < 0){
        perror("fork");
        exit(1);
    }
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
                printf("child process get SIGSTOP signal\n");
            }
            else if (WIFSIGNALED(status)){
                switch(WTERMSIG(status)){
                    case 1:
                        printf("child process get SIGHUP signal\n");
                        break;
                    case 2:
                        printf("child process get SIGINT signal\n");
                        break;
                    case 3:
                        printf("child process get SIGQUIT signal\n");
                        break;
                    case 4:
                        printf("child process get SIGILL signal\n");
                        break;
                    case 5:
                        printf("child process get SIGTRAP signal\n");
                        break;
                    case 6:
                        printf("child process get SIGABRT signal\n");
                        break;
                    case 7:
                        printf("child process get SIGBUS signal\n");
                        break;
                    case 8:
                        printf("child process get SIGFPE signal\n");
                        break;
                    case 9:
                        printf("child process get SIGKILL signal\n");
                        break;
                    case 10:
                        printf("child process get SIGUSR1 signal\n");
                        break;
                    case 11:
                        printf("child process get SIGSEGV signal\n");
                        break;
                    case 12:
                        printf("child process get SIGUSR2 signal\n");
                        break;
                    case 13:
                        printf("child process get SIGPIPE signal\n");
                        break;
                    case 14:
                        printf("child process get SIGALRM signal\n");
                        break;
                    case 15:
                        printf("child process get SIGTERM signal\n");
                        break;
                    default:
                        printf("Unknown signal! Signal Number is: %d\n", WTERMSIG(status));
                        break;
                }
            }
            exit(0);
        }
    }
	
	return 0;
}
