#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  pid_t pid;
  int status;

  printf("Process start to fork\n");
  pid = fork(); /* fork a child process */

  if (pid == -1) {
    perror("fork");
    exit(1);
  } else {
    // child process
    if (pid == 0) {
      printf("I'm the Child Process, my pid = %d\n", getpid());
      int i;
      char *arg[argc];
      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      printf("child process start to excute test program:\n");
      execve(arg[0], arg, NULL); /* execute test program */

      printf("continue to run original child process!\n");

      perror("execve");

      exit(EXIT_FAILURE);
    }

    // parent process
    else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      waitpid(-1, &status, WUNTRACED); /* wait for child process terminates */
      printf("parent process receives the signal\n");
      /* check child process'  termination status */
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {

        int signal = WTERMSIG(status);

        // abort
        if (signal == 6) {
          printf("child process get SIGABRT signal\n");
        }

        // alarm
        else if (signal == 14) {
          printf("child process get SIGALRM signal\n");

        }

        // bus
        else if (signal == 7) {
          printf("child process get SIGBUS signal\n");
        }

        // floating
        else if (signal == 8) {
          printf("child process get SIGFPE signal\n");
        }

        // hangup
        else if (signal == 1) {
          printf("child process get SIGHUP signal\n");
        }

        // illgal_instr
        else if (signal == 4) {
          printf("child process get SIGILL signal\n");
        }

        // interrupt
        else if (signal == 2) {
          printf("child process get SIGINT signal\n");
        }

        // kill
        else if (signal == 9) {
          printf("child process get SIGKILL signal\n");
        }

        // pipe
        else if (signal == 13) {
          printf("child process get SIGPIPE signal\n");
        }

        // quit
        else if (signal == 3) {
          printf("child process get SIGQUIT signal\n");
        }

        // segment_fault
        else if (signal == 11) {
          printf("child process get SIGSEGV signal\n");
        }

        // terminates
        else if (signal == 15) {
          printf("child process get SIGTERM signal\n");
        }

        // trap
        else if (signal == 5) {
          printf("child process get SIGTRAP signal\n");
        }
      } else if (WIFSTOPPED(status)) {
        printf("child process get SIGSTOP signal\n");
      } else {
        printf("CHILD PROCESS CONTINUED\n");
      }
      exit(0);
    }
  }
  return (0);
}