#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    printf("Process start to fork\n");
    pid = fork();

    if (pid == 0)
    {
        printf("I'm the child process, my pid = %d \n", getpid());
    }
    else
    {
        printf("I'm the parent process, my pid = %d \n", getpid());
    }
    if (pid == 0)
    {
        int i;
        char *args[argc];
        for (i = 0; i < argc - 1; i++)
        {
            args[i] = argv[i + 1];
        }
        args[argc - 1] = NULL;
        printf("Child process start to execute test program:\n");
        execve(argv[1], args, NULL);
        perror("execve");
    }
    else
    {
        int status;
        waitpid(pid, &status, WUNTRACED);
        printf("Parent process receives SIGCHLD signal\n");
        if (WIFEXITED(status))
        {
            printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            if (WTERMSIG(status) == 1)
            {
                printf("Parent process receives SIGHUP signal\n");
            }
            else if (WTERMSIG(status) == 2)
            {
                printf("Parent process receives SIGINT signal\n");
            }
            else if (WTERMSIG(status) == 3)
            {
                printf("Parent process receives SIGQUIT signal\n");
            }
            else if (WTERMSIG(status) == 4)
            {
                printf("Parent process receives SIGILL signal\n");
            }
            else if (WTERMSIG(status) == 5)
            {
                printf("Parent process receives SIGTRAP signal\n");
            }
            else if (WTERMSIG(status) == 6)
            {
                printf("Parent process receives SIGABRT signal\n");
            }
            else if (WTERMSIG(status) == 7)
            {
                printf("Parent process receives SIGBUS signal\n");
            }
            else if (WTERMSIG(status) == 8)
            {
                printf("Parent process receives SIGFPE signal\n");
            }
            else if (WTERMSIG(status) == 9)
            {
                printf("Parent process receives SIGKILL signal\n");
            }
            else if (WTERMSIG(status) == 11)
            {
                printf("Parent process receives SIGSEGV signal\n");
            }
            else if (WTERMSIG(status) == 13)
            {
                printf("Parent process receives SIGPIPE signal\n");
            }
            else if (WTERMSIG(status) == 14)
            {
                printf("Parent process receives SIGALRM signal\n");
            }
            else if (WTERMSIG(status) == 15)
            {
                printf("Parent process receives SIGTERM signal\n");
            }
        }
        else if (WIFSTOPPED(status))
        {
            if (WSTOPSIG(status) == 19)
            {
                printf("CHILD PROCESS STOPPED\n");
            }
        }
    }
}
