#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include<string.h>

char signallist[31][11] = 
{
	" ","SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP",
	"SIGABRT","SIGBUS","SIGFPE","SIGKILL","SIGUSR1",
	"SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM","SIGTERM",
	"SIGSTKFLT","SIGCHLD","SIGCONT","SIGSTOP","SIGTSTP",
	"SIGTTIN","SIGTTOU","SIGURG","SIGXCPU","SIGXFSZ",
	"SIGVTALRM","SIGPROF","SIGWINCH","SIGIO","SIGPWR"
	};

int main(int argc, char *argv[])
{
	int status;
	int pid;
	/* fork a child process */
	printf("Process start to fork\n");
	printf("I'm the Parent Process, my pid = %d\n", getpid());
	pid = fork();
	if (pid<0)
	{
		printf("fork failed");
		return 1;
	}

	/* execute test program */ 
	else if (pid == 0)
	{
		printf("I'm the Child Process, my pid = %d\n",getpid());
		printf("Child process start to execute test program:\n");
		char path1[100] ="/home/vagrant/csc3150/ass1/program1/";
		char *file = argv[1];
		strcat(path1,file);
		printf("%s",path1);
		if(execl(path1,argv[1],0,NULL) == -1)
		{
			printf("exe failed\n");
		}		
	}

	/* wait for child process terminates */
	else
	{
		waitpid(-1,&status,WUNTRACED);
		printf("Parent process receices SIGCHLD signal\n");
		
	/* check child process'  termination status */
	 	if (WIFSTOPPED(status))
		{
			printf("child process get SIGSTOP sigal\n");
		}
		else if(WIFEXITED(status))
		{
			printf("Normal termination with EXIT STATUS = 0\n");
		}
		else if(WIFSIGNALED(status))
		{
			int how = WTERMSIG(status);
			printf("child process get %s sigal\n", signallist[how]);
		}
		else 
		{
			printf("CHILD PROCESS continued\n");
		}
		exit(0);
	}
	return(0);
}
