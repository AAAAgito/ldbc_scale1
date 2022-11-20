#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

void print_sig(char *tmp)
{
	printf("child process get %s singal\n", tmp);
}
int main(int argc, char *argv[], char *envp[])
{
	sleep(1);
	/*fork a child process*/
	int pid = fork();
	if (pid > 0) {
		printf("Process start to fork\n");
		printf("I'm the Parent Process, my pid = %d\n", getpid());
		/*wait for child process terminates*/
		int status;
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");
		/*check child process' termination status*/
		int value = 0;
		if (WIFEXITED(status))
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			value = WTERMSIG(status);
		else if (WIFSTOPPED(status))
			value = WSTOPSIG(status);
		switch (value) {
		case 1:
			print_sig("SIGHUP");
			break;
		case 2:
			print_sig("SIGINT");
			break;
		case 3:
			print_sig("SIGQUIT");
			break;
		case 4:
			print_sig("SIGILL");
			break;
		case 5:
			print_sig("SIGTRAP");
			break;
		case 6:
			print_sig("SIGABRT");
			break;
		case 7:
			print_sig("SIGBUS");
			break;
		case 8:
			print_sig("SIGFPE");
			break;
		case 9:
			print_sig("SIGKILL");
			break;
		case 11:
			print_sig("SIGSEGV");
			break;
		case 13:
			print_sig("SIGPIPE");
			break;
		case 14:
			print_sig("SIGALRM");
			break;
		case 15:
			print_sig("SIGTERM");
			break;
		case 19:
			print_sig("SIGSTOP");
			break;
		}
	} else {
		/*execute test program*/
		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");
		execve(argv[1], argv, NULL);
	}
}