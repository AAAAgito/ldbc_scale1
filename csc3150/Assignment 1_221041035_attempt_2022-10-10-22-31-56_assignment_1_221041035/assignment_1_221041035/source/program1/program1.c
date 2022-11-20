#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

void handle(int sig) {
    if (sig == SIGCHLD) {
        printf("Parent process receives SIGCHLD signal.\n");
    }
}

int main(int argc, char *argv[]) {
    signal(SIGCHLD, handle);
    pid_t fpid;
    int status;
    char *arguments[100];
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            arguments[i - 1] = argv[i];
        }
    }
    arguments[argc - 1] = NULL;

    /* fork a child process */
    printf("Process start to fork\n");
    fpid = fork();
    if (fpid < 0) {
        printf("Failed.\n");
    } else if (fpid == 0) {
        printf("I'm the Child Process, my pid = %d\n", getpid());
        printf("Child process start to execute test program:\n");
        /* execute test program */
        execv(argv[1], arguments);
    } else {
        printf("I'm the Parent Process, my pid = %d\n", getpid());

        /* wait for child process terminates */
        wait(&status);

        /* check child process'  termination status */
        if (status == 0) {
            printf("Normal termination with EXIT STATUS = 0\n");
        } else {
            printf("CHILD PROCESS STOPPED, BUT EXIT STATUS = %d\n", status);
        }
    }
}
