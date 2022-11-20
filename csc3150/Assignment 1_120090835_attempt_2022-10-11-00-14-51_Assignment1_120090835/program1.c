#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	int status;
	/* fork a child process */
	printf("Process start to fork\n");
	pid_t pid = fork(); //利用fork生成子进程
	// printf("I'm the Parent Process, my pid = %d\n", getppid());
	if (pid < 0) { //报错，未生成子进程
		perror("fork");
		exit(1);
	} else { //fork成功的情况下
		if (pid == 0) { //pid=0说明在子进程中
			int i;
			char *arg[argc]; //新建char数组指针
			for (i = 0; i < argc - 1; i++) {
				arg[0] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			printf("I'm the Child process, my pid = %d\n",
			       getpid());
			printf("Child process start to execute test progam:\n");
			printf("------------CHILD PROCESS START------------");
			/* execute test program */
			execve(arg[0], arg, NULL);
			perror("execve");
			exit(EXIT_FAILURE);
		} else { //在父进程中
			/* wait for child process terminates */
			printf("I'm the Parent Process, my pid = %d\n",
			       getppid());
			waitpid(-1, &status, WUNTRACED);
			/* check child process'  termination status */
			if (WIFEXITED(status)) { //检查子进程状态, normal
				// printf("This is the normal program\n");
				// printf("------------CHILD PROCESS END------------\n");
				printf("Parent process receives the SIGCHLD signal\n");
				printf("Normal termination with EXIT STATUS=%d\n",
				       WEXITSTATUS(status)); //返回子进程状态值
			}
			//abort=6,alarm=14,bus=10,
			//floating=8,hangup=1,illegal_instr=4
			//interrupt=2,kill=9,pipe=13,quit=3,
			//segment_fault=11,terminate=15,trap=5
			else if (WIFSIGNALED(status)) {
				int check1 = WTERMSIG(status);
				if (check1 == 6) { //abort
					// printf("CHILD PROCESS FAILED: %d\n", WTERMSIG(status));
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGABRT signal\n");
				} else if (check1 == 1) { //hangup
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGHUP signal\n");
				} else if (check1 == 8) { //floating
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGFPE signal\n");
				} else if (check1 == 10) { //bus
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGBUS signal\n");
				} else if (check1 == 14) { //alarm
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGALRM signal\n");
				} else if (check1 == 4) { //illegal_instr
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGILL signal\n");
				} else if (check1 == 2) { //interrupt
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGINT signal\n");
				} else if (check1 == 9) { //kill
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGKILL signal\n");
				} else if (check1 == 13) { //pipe
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGPIPE signal\n");
				} else if (check1 == 3) { //quit
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGQUIT signal\n");
				} else if (check1 == 11) { //segment_fault
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGSEGV signal\n");
				} else if (check1 == 15) { //terminate
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGTERM signal\n");
				} else if (check1 == 5) { //trap
					printf("Parent process receives SIGCHLD signal\n");
					printf("Child process get SIGTRAP signal\n");
				}
			}
			//stop=23
			else if (WIFSTOPPED(status)) {
				printf("Parent process receives SIGCHLD signal\n");
				printf("Child process get SIGSTOP signal\n");
				printf("CHILD PROCESS STOPPED: %d\n",
				       WSTOPSIG(status));

			}
			//
			else {
				printf("CHILD PROCESS CONTINUED\n");
			}
			exit(0);
		}
	}
	return 0;
}
