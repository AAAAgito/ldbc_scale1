#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

	/* fork a child process */
	int main(int argc, char *argv[]){

		pid_t pid;
		int status;

		printf("Process start to fork\n");
		pid=fork();

		if(pid==-1){
			perror("fork");
			exit(1);
		}
		else{
			if(pid==0){
			/* execute test program */ 
				int i;
    			char *arg[argc];
				for(i=0;i<argc-1;i++){
                	arg[i]=argv[i+1];
            	}
            	arg[argc-1]=NULL;
				//printf("%d\n",argc);
				//printf("%s\n",arg[0]);
				printf("I'm the Parent Process, my pid = %d\n",getpid());
				printf("I'm the Child Process, my pid = %d\n",getpid());
				printf("Child process start to execute test program:\n");
				execve(arg[0],arg,NULL);

				
				// printf("I'm raising SIGCHID signal!\n\n");
				// raise(SIGCHLD);
			}

			//parent process
			else{
				/* wait for child process terminates */
				wait(&status);
				
				printf("Parent process receives the signal: %d\n",status);
				/* check child process'  termination status */

				if(WIFEXITED(status)){//normal
					printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
				}

				else if(WIFSIGNALED(status)){//abnormal
					int num = WTERMSIG(status);
					switch (num){
						case 6: // SIGABRT
							printf("child process get SIGABRT signal\n");
							break;
						case 14: // SIGALRM
							printf("child process get SIGALRM signal\n");
							break;
						case 7: // SIGBUS
							printf("child process get SIGBUS signal\n");
							break;
						case 8: // SIGFPE
							printf("child process get SIGFPE signal\n");
							break;
						case 1: // SIGHUP
							printf("child process get SIGHUP signal\n");
							break;
						case 4: // SIGILL
							printf("child process get SIGILL signal\n");
							break;
						case 2: // SIGINT
							printf("child process get SIGINT signal\n");
							break;
						case 9: // SIGKILL
							printf("child process get SIGKILL signal\n");
							break;
						case 13: // SIGPIPE
							printf("child process get SIGPIPE signal\n");
							break;
						case 3: // SIGQUIT
							printf("child process get SIGQUIT signal\n");
							break;
						case 11: // SIGSEGV
							printf("child process get SIGSEGV signal\n");
							break;
						case 15: // SIGTERM
							printf("child process get SIGTERM signal\n");
							break;
						case 5: // SIGTRAP
							printf("child process get SIGTRAP signal\n");
							break;
					}
					
				}

				else if(WIFSTOPPED(status)){
					printf("CHILD PROCESS STOPPED: %d\n" , WSTOPSIG(status));
				}
				else{
					printf("CHILD PROCESS CONTINUED\n");
				}
				exit(0);
			}
		}
		return 0;

	}

