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
	int status;
	printf("process start to fork \n");
	pid = fork();

	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		//child process
		if (pid == 0) {
			int i;
			char *arg[argc];
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;

			/* execute test program */
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			printf("Children process start to execute test program:\n");
			execve(arg[0], arg, NULL);
			exit(EXIT_FAILURE);
		}

		//parent process
		else {
			printf("I'm Parent Process, my pid = %d\n", getpid());
			/* wait for child process terminates */
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives the SIGCHLD signal\n");

			/* check child process'  termination status */
			//normal termination
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			}

			//abnormal termination
			else if (WIFSIGNALED(status)) {
				//get terminating signal
				int signal = WTERMSIG(status);

				//SIGHUP
				if (signal == 1) {
					printf("child process gets SIGHUP signal\n");
					printf("child process is hung up\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGINT
				else if (signal == 2) {
					printf("child process gets SIGINT signal\n");
					printf("child process is interrupted\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGQUIT
				else if (signal == 3) {
					printf("child process gets SIGQUIT signal\n");
					printf("child process is quited\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGILL
				else if (signal == 4) {
					printf("child process gets SIGILL signal\n");
					printf("child process gets illegal instruction\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGTRAP
				else if (signal == 5) {
					printf("child process gets SIGTRAP signal\n");
					printf("chid process is terminated by trap signal\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);

				}

				//SIGABRT
				else if (signal == 6) {
					printf("child process gets SIGABRT signal\n");
					printf("child process is abort by abort signal\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGBUS
				else if (signal == 7) {
					printf("child process gets SIGBUS signal\n");
					printf("child process gets bus error\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGFPE
				else if (signal == 8) {
					printf("child process gets SIGFPE signal\n");
					printf("child process gets floating point exception\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGKILL
				else if (signal == 9) {
					printf("child process gets SIGKILL signal\n");
					printf("child process is killed\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGSEGV
				else if (signal == 11) {
					printf("child process gets SIGSEGV signal\n");
					printf("child process uses invalid reference\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGPIPE
				else if (signal == 13) {
					printf("child process gets SIGPIPE signal\n");
					printf("child process writes to pipe without readers\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGALRM
				else if (signal == 14) {
					printf("child process gets SIGALRM signal\n");
					printf("child process is terminated by alarm signal\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				}

				//SIGTERM
				else if (signal == 15) {
					printf("child process gets SIGTERM signal\n");
					printf("child process is terminated by SIGTERM signal\n");
					printf("CHILD EXECUTION FAILED:%d\n",
					       signal);
				} else {
					printf("child process get a illegal signal out of the samples\n");
					printf("CHILD EXECUTION FAILED!!\n");
				}

			}
			//SIGSTOP
			else if (WIFSTOPPED(status)) {
				int stopStatus = WSTOPSIG(status);
				if (stopStatus == SIGSTOP) {
					printf("child process gets SIGSTOP signal\n");
					printf("child process stopped\n");
					printf("CHILD PROCESS STOPPED:%d\n",
					       WSTOPSIG(status));
				} else {
					printf("child process get a illegal signal out of the samples\n");
					printf("child process stopped\n");
					printf("CHILD PROCESS STOPPED\n");
				}
			} else {
				printf("CHILD PROCESS CONTINUED\n");
			}
			exit(0);
		}
	}
}
