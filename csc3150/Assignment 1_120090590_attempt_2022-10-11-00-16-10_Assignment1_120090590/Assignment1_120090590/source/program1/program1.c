#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int wait_time, status;
	int id = atoi(argv[1]);
	// map<string,int>
	if (argc != 2) {
		printf("please enter correct argument\n");
		// printf("%d\n",argc);
		return 0;
	}
	char *args[argc];
	for (int i = 0; i < argc; i++) {
		args[i] = argv[i + 1];
		args[argc - 1] = NULL;
	}

	/* fork a child process */
	printf("process start to fork\n");
	pid = fork();

	if (pid == -1) {
		printf("unsuccessful creation!");
		exit(0);
	} else {
		if (pid == 0) { /* execute test program */
			printf("I'm the Child Process, my pid = %d\n",
			       getpid());
			printf("Child process start to execute test program:");
			execve(args[0], args, NULL);
			// switch (id)
			// {
			// case 1:
			// 	execve(".","normal",NULL);

			// case 2:
			// 	execlp("./abort","abort",NULL);
			// case 3:
			// 	execlp("./alarm","alarm",NULL);
			// case 4:
			// 	execlp("./bus","bus",NULL);
			// case 5:
			// 	execlp("./floating","floating",NULL);
			// case 6:
			// 	execlp("./hangup","hangup",NULL);
			// case 7:
			// 	execlp("./illegal_instr","illegal_instr",NULL);
			// case 8:
			// 	execlp("./interrupt","interrupt",NULL);
			// case 9:
			// 	execlp("./kill","kill",NULL);
			// case 10:
			// 	execlp("./pipe","pipe",NULL);
			// case 11:
			// 	execlp("./quit","quit",NULL);
			// case 12:
			// 	execlp("./segment_fault","segment_fault",NULL);
			// case 13:
			// 	execlp("./stop","stop",NULL);
			// case 14:
			// 	execlp("./terminate","terminate",NULL);
			// case 15:
			// 	execlp("./trap","trap",NULL);
			// }
		} else { /* wait for child process terminates */
			printf("I'm the Parent Process, my pid = %d\n",
			       getpid());

			waitpid(pid, &status, WUNTRACED);
			/* check child process'  termination status */
			// printf("%d\n",WIFEXITED(status));
			printf("Parent process receives SIGCHLD signal\n");
			if (WIFEXITED(status)) {
				printf("Normal termination with EXIT STATUS = %d\n",
				       WEXITSTATUS(status));
				// exit(0);
				return 0;
			}
			if (WIFSIGNALED(status)) {
				printf("Child Failed. Parent process receives %s signal\n",
				       strsignal(WTERMSIG(status)));
			} else if (WIFSTOPPED(status)) {
				printf("CHILD PROCESS STOPPED with  %d signal\n",
				       WSTOPSIG(status));
			} else {
				printf("CHILD PROCESS CONTINUE\n");
			}
		}
		return 0;
	}
}
