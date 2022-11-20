#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

	/* fork a child process */
    printf("Process start to fork\n");
    int pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else
    if (pid == 0) //child process
    {
	    /* execute test program */ 
        printf("I'm the Child Process, my pid = %d\n", getpid());
        printf("Child process start to execute test program:\n");
        execve(argv[1], NULL, NULL);
        perror("execve");
        exit(0);
    }
    else
    if (pid > 0) //parent process
    {
        printf("I'm the Parent Process, my pid = %d\n", getpid());
        int status;
	    /* wait for child process terminates */
        int wait_res = waitpid(pid, &status, WUNTRACED); // because the stopped process need the WUNTRACED option
	    if (wait_res < 0)
        {
            perror("wait");
            exit(1);
        }
        printf("Parent process receives the SIGCHLD signal\n");
	    /* check child process'  termination status */
        if (WIFEXITED(status)) //terminate normally
        {
            printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
            exit(0);
        }
        else 
        if (WIFSIGNALED(status))//terminate abnormally
        {
            int sig = WTERMSIG(status);
            if (sig == SIGABRT)
            {
                printf("child process get SIGABRT signal\n");
            }
            else
            if (sig == SIGBUS)
            {
                printf("child process get SIGBUS signal\n");
            }
            else
            if (sig == SIGALRM)
            {
                printf("child process get SIGALRM signal\n");
            }
            else
            if (sig == SIGFPE)
            {
                printf("child process get SIGFPE signal\n");
            }
            else
            if (sig == SIGHUP)
            {
                printf("child process get SIGHUP signal\n");
            }
            else
            if (sig == SIGILL)
            {
                printf("child process get SIGILL signal\n");
            }
            else
            if (sig == SIGINT)
            {
                printf("child process get SIGINT signal\n");
            }
            else
            if (sig == SIGKILL)
            {
                printf("child process get SIGKILL signal\n");
            }
            else
            if (sig == SIGPIPE)
            {
                printf("child process get SIGPIPE signal\n");
            }
            else
            if (sig == SIGQUIT)
            {
                printf("child process get SIGQUIT signal\n");
            }
            else
            if (sig == SIGSEGV)
            {
                printf("child process get SIGSEGV signal\n");
            }
            else
            if (sig == SIGTERM)
            {
                printf("child process get SIGTERM signal\n");
            }
            else
            if (sig == SIGTRAP)
            {
                printf("child process get SIGTRAP signal\n");
            }
            exit(1);
        }
        else
        if (WIFSTOPPED(status))
        {
            printf("child process get SIGSTOP signal\n");
            exit(1);
        }
    }
	
}
