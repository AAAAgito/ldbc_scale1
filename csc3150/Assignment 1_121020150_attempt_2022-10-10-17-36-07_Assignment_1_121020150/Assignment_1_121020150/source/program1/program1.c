#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[])
{
	/*sleep to ensure the parent process work before the child.*/
	sleep(0.1);
	/*fork a child process*/
	int pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	}
	if (pid > 0) {
		printf("Process start to fork\n");
		printf("I'm the Parent Process, my pid = %d\n", getpid());
		/*wait for child process terminates*/
		int status;
		/*option = WUNTRACED for the case of stop*/
		if (waitpid(pid, &status, WUNTRACED) < 0) {
			/*unsuccessful wait*/
			perror("wait");
			exit(2);
		}
		printf("Parent process receives SIGCHLD signal\n");
		/*check child process' termination status*/
		if (WIFEXITED(status)) {
			int return_value = WEXITSTATUS(status);
			printf("Normal termination with EXIT STATUS = %d\n",
			       return_value);
		} else if (WIFSIGNALED(status)) {
			int return_value = WTERMSIG(status);
			// printf("returnvalue = %d\n", return_value);
			if (return_value == 6)
				printf("child process get SIGABRT signal\n");
			if (return_value == 14)
				printf("child process get SIGALRM signal\n");
			if (return_value == 7)
				printf("child process get SIGBUS signal\n");
			if (return_value == 8)
				printf("child process get SIGFPE signal\n");
			if (return_value == 1)
				printf("child process get SIGHUP signal\n");
			if (return_value == 4)
				printf("child process get SIGILL signal\n");
			if (return_value == 2)
				printf("child process get SIGINT signal\n");
			if (return_value == 9)
				printf("child process get SIGKILL signal\n");
			if (return_value == 13)
				printf("child process get SIGPIPE signal\n");
			if (return_value == 3)
				printf("child process get SIGQUIT signal\n");
			if (return_value == 11)
				printf("child process get SIGSEGV signal\n");
			if (return_value == 15)
				printf("child process get SIGTERM signal\n");
			if (return_value == 5)
				printf("child process get SIGTRAP signal\n");
			// printf("CHILD EXCECUTION FAILED: %d\n", WTERMSIG(status));

		} else if (WIFSTOPPED(status)) {
			int return_value = WSTOPSIG(status);
			if (return_value == 19)
				printf("child process get SIGSTOP signal\n");
			// printf("CHILD PROCESS STOPPED: %d\n", WSTOPSIG(status));
		}
	} else {
		/*execute test program*/
		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");
		execve(argv[1], argv, NULL);
		/*unsuccessful exec*/
		perror("exec");
		exit(4);
	}
}
