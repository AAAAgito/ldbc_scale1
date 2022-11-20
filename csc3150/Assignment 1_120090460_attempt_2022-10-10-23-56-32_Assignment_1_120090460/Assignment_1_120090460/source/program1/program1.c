#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[]){

    int status;

    printf("Process start to fork\n");
	/* fork a child process */
    pid_t pid = fork();


    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    else {
        //Child Process
        if (pid == 0) {
            printf("I'm the Child Process, my pid = %d\n", getpid());
            char * arg[argc];
// why here has "- 1"
            for (int i = 0; i < argc - 1; i++) {
                arg[i] = argv[i + 1];
            }
            arg[argc - 1] = NULL;
            printf("Child Process starts to execute test program:\n");
	/* execute test program */
            execve(arg[0], arg, NULL);
            printf("Continue to run original CHILD Process");
            exit(SIGCHLD);

        } else {
//            Parent Process
            printf("I'm the Parent Process, my pid = %d\n", getpid());
	        /* wait for child process terminates */
            waitpid(pid, &status, WUNTRACED);
            printf("Parent process receives SIGCHLD signal\n");
	        /* check child process'  termination status */
            if (WIFEXITED(status)) {
                printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                int signal_result = WTERMSIG(status);
                switch (signal_result) {
                    case SIGABRT: {
                        printf("Child Process Failed by the ABORT Signal\n");
                        break;
                    }case SIGALRM: {
                        printf("Child Process Failed by the ALARM Signal\n");
                        break;
                    }case SIGBUS: {
                        printf("Child Process Failed by the SIGBUS Signal\n");
                        break;
                    }case SIGFPE: {
                        printf("Child Process Failed by the SIGFPE Signal\n");
                        break;
                    }case SIGHUP: {
                        printf("Child Process Failed by the SIGHUP Signal\n");
                        break;
                    }case SIGILL: {
                        printf("Child Process Failed by the SIGILL Signal\n");
                        break;
                    }case SIGINT: {
                        printf("Child Process Failed by the SIGHUP Signal\n");
                        break;
                    }case SIGKILL: {
                        printf("Child Process Failed by the SIGKILL Signal\n");
                        break;
                    }case SIGPIPE: {
                        printf("Child Process Failed by the SIGPIPE Signal\n");
                        break;
                    }case SIGQUIT: {
                        printf("Child Process Failed by the SIGQUIT Signal\n");
                        break;
                    }case SIGSEGV: {
                        printf("Child Process Failed by the SIGSEGV Signal\n");
                        break;
                    }case SIGTERM: {
                        printf("Child Process Failed by the SIGTERM Signal\n");
                        break;
                    }case SIGTRAP: {
                        printf("Child Process Failed by the SIGTRAP Signal\n");
                        break;
                    }
                    default:;
                }
                printf("CHILD EXECUTION FAILED: %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("CHILD PROCESS STOPPED: %d\n", WSTOPSIG(status));
            } else {
                printf("CHILD PROCESS CONTINUED\n");
            }
            exit(0);
        }
    }
}
