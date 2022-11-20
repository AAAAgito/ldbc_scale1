#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){
	char Signals[65][15] = {"", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP",
						"SIGABRT", "SIGBUS", "SIGFPE", "SIGKILL", "SIGUSR1",
						"SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM",
						"SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP",
						"SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU", "SIGXFSZ",
						"SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO", "SIGPWR",
						"SIGSYS", "SIGRTMIN", "SIGRTMIN+1", "SIGRTMIN+2", "SIGRTMIN+3",
						"SIGRTMIN+4", "SIGRTMIN+5", "SIGRTMIN+6", "SIGRTMIN+7", "SIGRTMIN+8",
						"SIGRTMIN+9", "SIGRTMIN+10", "SIGRTMIN+11", "SIGRTMIN+12", "SIGRTMIN+13",
						"SIGRTMIN+14", "SIGRTMIN+15", "SIGRTMAX-14", "SIGRTMAX-13", "SIGRTMAX-12",
						"SIGRTMAX-11", "SIGRTMAX-10", "SIGRTMAX-9", "SIGRTMAX-8", "SIGRTMAX-7",
						"SIGRTMAX-6", "SIGRTMAX-5", "SIGRTMAX-4", "SIGRTMAX-3", "SIGRTMAX-2",
						"SIGRTMAX-1", "SIGRTMAX"};

	int state;
	pid_t pid = fork();
	
	/* fork a child process */
	if(pid < 0){
		perror("fork");
		exit(0);
	}
	else{
	/* execute test program */
		if(pid == 0){
			sleep(0.1);
			int i;
			char *arg[argc];

			printf("I'm the Child Process, my pid = %d\n", getpid());
			
			for(i=0; i<argc-1; i++){
				arg[i] = argv[i+1];
			}
			arg[argc-1] = NULL;

			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);

			perror("execve");
			exit(EXIT_FAILURE);
		}
		else{
			int status;

			printf("I'm the Parent Process, my pid = %d\n", getpid());

			/* wait for child process terminates */
			waitpid(pid, &state, WUNTRACED|WCONTINUED);
			
			printf("Parent process receives SIGCHLD signal\n");
			/* check child process'  termination status */
			if(state == 0){
				printf("Normal termination with EXIT STATUS = 0\n\n");
				exit(0);
			}
			else{
				if(WIFSTOPPED(state)) status = 19;
				else status = WTERMSIG(state);

				printf("child process get %s signal\n\n", Signals[status]);
				exit(0);
			}
		}
	}
	
	return 0;
}
