#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	/* fork a child process */
	pid_t pid;
	int status;
	printf("Process start to fork\n");
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else {
		if (pid == 0) {
			int i;
			char *arg[argc];
			for (i = 0; i < argc - 1; i++) {
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] =
				NULL; // the arg passed to execve function must be ended with null
			printf("I'm the Child process, my pid = %d\n",
			       getpid());
			printf("Child process start to execute test program:\n");
			raise(SIGCHLD);
			/* execute test program */
			execve(arg[0], arg, NULL);
			exit(SIGCHLD);
		} else {
			printf("I'm the Parent process, my pid = %d\n",
			       getpid());
			/* wait for child process terminates */
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				int terminate_types = WTERMSIG(status);
				/* check child process'  termination status */
				switch ((terminate_types)) {
				case 1: // SIGHUP hangup
					printf("child process is hang up by hangup signal\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGHUP);
					break;
				case 2: // SIGINT interrupt
					printf("child process is interrupted by interrupt signal\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGINT);
					break;
				case 3: // SIGQUIT quit
					printf("child process is quited by quit signal\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGQUIT);
					break;
				case 4: // SIGILL illegal
					printf("child process gets illegal instructions\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGILL);
					break;
				case 5: // SIGTRAP trap
					printf("child process is is terminated by trap signal\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGTRAP);
					break;
				case 6: // SIGABRT abort
					printf("child process is abort by abort signal\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGABRT);
					break;
				case 7: // SIGBUS bus
					printf("child process gets bus error\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGBUS);
					break;
				case 8: // SIGFPE floating
					printf("child process gets floating point exception\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGFPE);
					break;
				case 9: // SIGKILL kill
					printf("child process is killed by kill signal\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGKILL);
					break;
				case 11: // SIGSEGV segment
					printf("child process uses invalid memory reference\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGSEGV);
					break;
				case 13: // SIGPIPE pipe
					printf("child process writes to pipe with no readers\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGPIPE);
					break;
				case 14: // SIGALRM alarm
					printf("child process is is terminated by alarm signal\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGALRM);
					break;
				case 15: // SIGTERM terminate
					printf("child process is terminated by termination signal\n");
					printf("The return status is = %d\n",
					       terminate_types);
					raise(SIGTERM);
					break;
				}
			} else if (WIFSTOPPED(status)) {
				printf("CHILD PROCESS STOPPED: %d\n",
				       WSTOPSIG(status));
			} else {
				printf("CHILD PROCESS CONTINUED\n");
				raise(SIGCONT);
			}
			exit(0);
		}
	}
	return 0;
}
