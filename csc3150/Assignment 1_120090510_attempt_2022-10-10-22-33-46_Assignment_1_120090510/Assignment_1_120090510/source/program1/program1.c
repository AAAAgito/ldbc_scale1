#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>



int main(int argc, char *argv[])
{

	/* fork a child process */

	printf("Process start to fork\n");
	int status;
	pid_t pid = fork();
	
	/* execute test program */ 

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {

		// Child process
		if (pid == 0) {
			sleep(3);
			printf("I'm the child process, my pid = %d, \
			         my ppid = %d\n", getpid(), getppid());

			char *arg[argc];
			for (int i = 0; i < argc - 1; i++) {
				arg[i] = argv[i+1];
			}
			arg[argc-1] = NULL;

			printf("Child process start to execute the \
			                            test program:\n");
			execve(arg[0], arg, NULL);
			
			perror("execve");
			exit(1);
		} else {
			printf("I'm the parent process, my pid = %d\n", 
			                                       getpid());
			waitpid(0, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS\
				                = %d\n", WEXITSTATUS(status));
			} else if (WIFSTOPPED(status)) {
				printf("Child process gets SIGSTOP signal\n");
			} else {
				char *signame[] = {
					"", "SIGHUP", "SIGINT", "SIGQUIT", 
					"SIGILL", "SIGTRAP", "SIGABRT", 
					"SIGBUS", "SIGFPE", "SIGKILL", "", 
					"SIGSEGV", "", "SIGPIPE", "SIGALRM", 
					"SIGTERM", "", "", "", "SIGSTOP"
				};
				printf("Child process gets %s signal\n", 
				               signame[WTERMSIG(status)]);
			}
			exit(0);
		}
	}	
	
	return 0;
}
