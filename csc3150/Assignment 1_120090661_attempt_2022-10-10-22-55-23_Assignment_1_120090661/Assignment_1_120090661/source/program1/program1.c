#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
const char* status_to_signs[] = {"SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE", "SIGKILL", NULL, "SIGSEGV", NULL, "SIGPIPE", "SIGALRM", "SIGTERM"};

int main(int argc, char *argv[])
{
    
    /* fork a child process */
    pid_t pid;
    printf("Process start to fork\n");
    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    
    /* execute test program */
    if (pid == 0)
    {
        char *new_argv[argc];
        for (int i = 0; i < argc - 1; i++)
            new_argv[i] = argv[i + 1];
        new_argv[argc - 1] = NULL;
        printf("I'm the Child Process, my pid = %d\n", getpid());
        printf("Child process start to execute test program:\n");
        execve(new_argv[0], new_argv, NULL);
        perror("execve");
        exit(SIGCHLD);
    }
	
    /* wait for child process terminates */
    if (pid > 0)
    {
        int status;
        printf("I'm the Parent Process, my pid = %d\n", getpid());
        waitpid(pid, &status, WUNTRACED);
        printf("Parent process receives SIGCHLD signal\n");
	    
        /* check child process'  termination status */
        if (WIFEXITED(status)) printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
          else if (WIFSIGNALED(status))
               {
                    int signs = WTERMSIG(status);
                    printf("child process get %s signal\n", status_to_signs[signs - 1]);
               }
                 else if (WIFSTOPPED(status)) printf("child process get SIGSTOP signal\n");
                        else printf("CHILD PROCESS CONTINUED\n");
        exit(0);
    }

}
