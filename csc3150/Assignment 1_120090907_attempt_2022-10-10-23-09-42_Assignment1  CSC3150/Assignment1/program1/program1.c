#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    printf("Process start to fork\n");

    pid_t pid = fork();

    if (pid != 0) {
        // parent process
        printf("I'm the parent process, my pid = %ld\n", (long)getpid());

        int status;
        waitpid(pid, &status, WUNTRACED);

        printf("Parent process receiving the SIGCHLD signal\n");

        if (WIFEXITED(status)) {
            printf("Child process terminates normally\n");
            printf("It's exit status = %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process is terminated by signal\n");
            int signal = WTERMSIG(status);
            printf("It's signal number = %d (%s)\n", signal, strsignal(signal));
        } else if (WIFSTOPPED(status)) {
            printf("Child process is stopped\n");
            int signal = WSTOPSIG(status);
            printf("It's signal number = %d (%s)\n", signal, strsignal(signal));
        }
    } else {
        // child process
        printf("I'm the child process, my pid = %d\n", getpid());
        printf("Child process start to execute the program\n");
        execv(argv[1], argv + 1);
    }
}
