#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

	/* fork a child process */
  int status;
  pid_t pid = fork();
  if(pid>0){
    printf("Process start to fork\n");
  }
	/* execute test program */ 
  if(pid<0){
    printf("Fork error!\n");
  }
  else{

    if(pid == 0){
      int i;
      char * arg[argc];

      printf("I'm the Child Process, my pid = %d\n",getpid());

      for (i=0;i<argc-1;i++){
        arg[i]=argv[i+1];
      }
      arg[argc-1]=NULL;

      printf("Child process start to execute test program:\n");
      execve(arg[0],arg,NULL);

      printf("Continue to run original child process:\n");

      perror("execve");
      exit(EXIT_FAILURE);
    }
    
    /* wait for child process terminates */
    else{
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      waitpid(pid,&status,WUNTRACED);
      printf("Parent process receives SIGCHILD signal\n");

	/* check child process'  termination status */
      if(WIFEXITED(status)){
        printf("Normal termination with EXIT STATUS = %d\n",WEXITSTATUS(status));
      }./
      else if (WIFSIGNALED(status)){
        if(WTERMSIG(status) == 1){
          printf("child process get SIGHUP signal\n");
        }
        else if(WTERMSIG(status) == 2){
          printf("child process get SIGINT signal\n");
        }
        else if(WTERMSIG(status) == 3){
          printf("child process get SIGQUIT signal\n");
        }
        else if(WTERMSIG(status) == 4){
          printf("child process get SIGILL signal\n");
        }
        else if(WTERMSIG(status) == 5){
          printf("child process get SIGTRAP signal\n");
        }
        else if(WTERMSIG(status) == 6){
          printf("child process get SIGABRT signal\n");
        }
        else if(WTERMSIG(status) == 7){
          printf("child process get SIGBUS signal\n");
        }
        else if(WTERMSIG(status) == 8){
          printf("child process get SIGFPE signal\n");
        }
        else if(WTERMSIG(status) == 9){
          printf("child process get SIGKILL signal\n");
        }
        else if(WTERMSIG(status) == 11){
          printf("child process get SIGSEGV signal\n");
        }
        else if(WTERMSIG(status) == 14){
          printf("child process get SIGALRM signal\n");
        }      
        else if(WTERMSIG(status) == 15){
          printf("child process get SIGTERM signal\n");
        }      
        else{
          printf("CHILD EXECUTION FAILD: %d\n", WTERMSIG(status));
        }
      }
      else if(WIFSTOPPED(status)){
        printf("child process get SIGSTOP signal");
      }
      else{
        printf("CHILE PROCESS CONTINUED\n");
      }
      exit(0);


    }
  }
}
