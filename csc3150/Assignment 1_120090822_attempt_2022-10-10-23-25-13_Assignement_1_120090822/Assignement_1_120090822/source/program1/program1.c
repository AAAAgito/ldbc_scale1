#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){
	int status;   
    pid_t pid;
    printf("Process start to fork\n");
	pid = fork();
    if (pid == -1){
        perror("fork");
        exit(1);
    }
    else{
        if (pid == 0){
            char *arg[argc];
            int i;
            for (int i = 0; i < argc-1; i++){
                arg[i] = argv[i+1];
            }
            arg[argc-1] = NULL;

            printf("I'm the Child Process, my pid = %d\n",getpid());
            printf("Child process start to execute test program:\n");

            execve(arg[0], arg, NULL);



            perror("execve");
            exit(EXIT_FAILURE);
        }
        
        else{
            printf("I'm the Parent Process, my pid = %d\n",getpid());

            waitpid(pid, &status, WUNTRACED);
            printf("Parent process receiving the SIGCHLD signal\n");

            if(WIFEXITED(status)){
                printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status)){
                int sig;
                sig = WTERMSIG(status);

                switch (sig){
                case SIGABRT:
                    printf("child process get SIGABRT signal\n");
      
                    break;
                case SIGALRM:
                    printf("child process get SIGALRM signal\n");
       
                    break;
                case SIGBUS:
                    printf("child process get SIGBUS signal\n");
             
                    break;
                case SIGFPE:
                    printf("child process get SIGFPE signal\n");
               
                    break;
                case SIGHUP:
                    printf("child process get SIGHUP signal\n");
                    
                    break;
                case SIGILL:
                    printf("child process get SIGILL signal\n");
               
                    break;
                case SIGINT:
                    printf("child process get SIGINT signal\n");
                 
                    break;
                case SIGKILL:
                    printf("child process get SIGKILL signal\n");
                   
                    break;
                case SIGPIPE:
                    printf("child process get SIGPIPE signal\n");
               
                    break;
                case SIGQUIT:
                    printf("child process get SIGQUIT signal\n");
              
                    break;
                case SIGSEGV:
                    printf("child process get SIGSEGV signal\n");
        
                    break;
                case SIGTERM:
                    printf("child process get SIGTERM signal\n");
               
                    break;
                case SIGTRAP:
                    printf("child process get SIGTRAP signal\n");
              
                    break;
                }
            }
            else if (WIFSTOPPED(status)){
                printf("child process get SIGSTOP signal\n");
                
                
            }          
            exit(0);
        }

    }
    
}	
