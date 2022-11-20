#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
//#include <string.h>

void num_to_signal(int num){
	int num_set[14] = {SIGABRT, SIGALRM, SIGBUS, SIGFPE, SIGHUP, SIGILL, SIGINT, SIGKILL, 
	SIGPIPE, SIGQUIT, SIGSEGV, SIGSTOP, SIGTERM, SIGTRAP};
	char name_set[][8] = {"SIGABRT", "SIGALRM", "SIGBUS", "SIGFPE", "SIGHUP", "SIGILL", "SIGINT",
	"SIGKILL", "SIGPIPE", "SIGQUIT", "SIGSEGV", "SIGSTOP", "SIGTERM", "SIGTRAP"};
	for(int i = 0; i<15; i++){
		if(num_set[i] == num){
			printf("child progress get %s signal\n", name_set[i]);
		}
	}

}




int main(int argc, char *argv[]){

	int status;
	printf("Porcess start to fork\n");
	pid_t pid = fork();
	int child_pid;
	if(pid == -1){
		perror("fork");
		exit(1);
	}
	else{
		if(pid == 0){
			int i;
			char *arg[argc];
			child_pid = getpid();
			for(i=0; i<argc-1; i++){
				arg[i] = argv[i+1];
			}
			arg[argc-1] = NULL;
			printf("I'm the Child Process, my pid =%d\n", getpid());
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);
			printf("???\n");
		}
		else{
			printf("I'm the Parent Process, my pid =%d\n", getpid());
			waitpid(child_pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHILD signal\n");
			if(WIFEXITED(status)){
				printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
			}
			else if(WIFSIGNALED(status)){
				num_to_signal(WTERMSIG(status));

			}
			else if(WIFSTOPPED(status)){
				num_to_signal(WSTOPSIG(status));
			}
			else {
				exit(1);
			}
			
			exit(0);
		}

	}
	return 0;
	
}
