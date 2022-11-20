#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;

	/* fork a child process */
	pid = fork();
	if (pid < 0) {
		printf("Fork error!");
	}

	/* execute test program */
	else if (pid == 0) {
		char *arg[argc];

		for (int i = 0; i < argc - 1; i++) {
			arg[i] = argv[i + 1];
		}
		arg[argc - 1] = NULL;

		printf("I'm the Child Process, my pid = %d\n", getpid());
		printf("Child process start to execute test program:\n");

		// executing PATH(arg[0]) with arguments arg
		execve(arg[0], arg, NULL);

		perror("execve");

		exit(EXIT_FAILURE);
	}

	else {
		/* wait for child process terminates */
		printf("Process start to fork\n");
		printf("I'm the Parent Process, my pid = %d\n", getpid());

		// Wait for a child matching PID to die.
		waitpid(pid, &status, WUNTRACED);
		printf("Parent process receives SIGCHLD signal\n");

		/* check child process'  termination status */
		// normal termination
		if (WIFEXITED(status)) {
			printf("Normal termination with EXIT STATUS = %d\n",
			       WEXITSTATUS(status));
		}

		//  abnormal termination
		else if (WIFSIGNALED(status)) {
			switch (status) {
			case 1:
				printf("child process get SIGHUP signal\n");
				break;
			case 2:
				printf("child process get SIGINT signal\n");
				break;
			case 9:
				printf("child process get SIGKILL signal\n");
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
			case 131:
				printf("child process get SIGQUIT signal\n");
				break;
			case 132:
				printf("child process get SIGILL signal\n");
				break;
			case 133:
				printf("child process get SIGTRAP signal\n");
				break;
			case 134:
				printf("child process get SIGABRT signal\n");
				break;
			case 135:
				printf("child process get SIGBUS signal\n");
				break;
			case 136:
				printf("child process get SIGFPE signal\n");
				break;
			case 139:
				printf("child process get SIGSEGV signal\n");
				break;
			default:
				printf("CHILD EXECUTION FAILED\n");
				break;
			}
		}

		// STOP
		else if (WIFSTOPPED(status)) {
			printf("child process get SIGSTOP signal\n");
		}

		exit(1);
	}
}
