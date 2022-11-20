#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{ // argc是参数数量， argv中存有待执行参数
	pid_t pid;
	int status; //异常返回值

	printf("Process start to fork\n");

	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		if (pid == 0) {
			int i;
			char *arg[argc];
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());
			
			printf("I'm the Child Rrocess, my pid = %d\n",
			       getppid());
			for (i = 0; i < argc - 1; i++) {
				arg[i] =
					argv[i +
					     1]; //可执行文件的第一位是自己，跳过本身
			}
			arg[argc - 1] = NULL;
			printf("Child process start to execute test program:\n");
			execve(arg[0], arg, NULL);
			printf("execve failed \n");
			perror("execve");
			exit(0);
		} else {
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receive SIGCHLD signal \n");

			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				switch (WTERMSIG(status)) {
				case SIGABRT: {
					printf("child process get SIGABRT signal\n");
					break;
				}
				case SIGALRM: {
					printf("child process get SIGALRM signal\n");
					break;
				}
				case SIGBUS: {
					printf("child process get SIGBUS signal\n");
					break;
				}
				case SIGFPE: {
					printf("child process get SIGFPE signal\n");
					break;
				}
				case SIGHUP: {
					printf("child process get SIGHUP signal\n");
					break;
				}
				case SIGILL: {
					printf("child process get SIGILL signal\n");
					break;
				}
				case SIGINT: {
					printf("child process get SIGINT signal\n");
					break;
				}
				case SIGKILL: {
					printf("child process get SIGKILL signal\n");
					break;
				}
				case SIGPIPE: {
					printf("child process get SIGPIPE signal\n");
					break;
				}
				case SIGQUIT: {
					printf("child process get SIGQUIT signal\n");
					break;
				}
				case SIGSEGV: {
					printf("child process get SIGSEGV signal\n");
					break;
				}
				case SIGTERM: {
					printf("child process get SIGTERM signal\n");
					break;
				}
				case SIGTRAP: {
					printf("child process get SIGTRAP signal\n");
					break;
				}
				default: {
					printf("no matching signals\n");
					break;
				}
				}
			} else if (WIFSTOPPED(status)) {
				printf("child process get SIGSTOP signal\n");
			} else {
				printf("CHILD PROCESS CONTINUE");
			}
			exit(0);
		}
	}
	return 0;
}
