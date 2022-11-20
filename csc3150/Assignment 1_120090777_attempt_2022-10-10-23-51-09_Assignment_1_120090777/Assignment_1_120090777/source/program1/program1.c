#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	/* fork a child process */
	pid_t pid;
	int state;
	int term;

	printf("Process start to fork\n");
	pid = fork();
	//fork error
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		//child process
		if (pid == 0) {
			int i;
			char *arg[argc];
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			/* execute test program */
			printf("Child process start to execute test program:\n");
			// printf("------------CHILD PROCESS START------------");
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			execve(arg[0], arg, NULL);
			// raise(SIGCHLD);
			exit(0);
		}
		//parent process
		else {
			// sleep(3);
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			/* wait for child process terminates */
			//use the third parameters in waitpid to catch stop
			if (waitpid(pid, &state, WUNTRACED) < 0) {
				perror("Wait failed");
				exit(1);
			}
			/* check child process'  termination status */
			printf("\nParent process receives SIGCHLD signal\n");
			//normal exit
			if (WIFEXITED(state)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(state));
				// printf("------------CHILD PROCESS END------------\n");
			}
			//abnormal termination
			if (WIFSIGNALED(state)) {
				term = WTERMSIG(state);
				// printf("This is the %d program\n", WTERMSIG(state));
				// printf("Parent process receives SIGCHLD signal\n");
				if (term == 6) {
					printf("Child process get SIGABRT signal\n");
				}
				if (term == 7) {
					printf("Child process get SIGBUS signal\n");
				}
				if (term == 1) {
					printf("Child process get SIGHUP signal\n");
				}
				if (term == 14) {
					printf("Child process get SIGALRM signal\n");
				}
				if (term == 8) {
					printf("Child process get SIGFPE signal\n");
				}
				if (term == 4) {
					printf("Child process get SIGILL signal\n");
				}
				if (term == 2) {
					printf("Child process get SIGINT signal\n");
				}
				if (term == 9) {
					printf("Child process get SIGKILL signal\n");
				}
				if (term == 13) {
					printf("Child process get SIGPIPE signal\n");
				}
				if (term == 3) {
					printf("Child process get SIGQUIT signal\n");
				}
				if (term == 15) {
					printf("Child process get SIGTERM signal\n");
				}
				if (term == 11) {
					printf("Child process get SIGSEGV signal\n");
				}
				if (term == 5) {
					printf("Child process get SIGTRAP signal\n");
				}
			}
			if (WIFSTOPPED(state)) {
				printf("child process get SIGSTOP signal\n");
			}
			exit(0);
		}
	}
}
