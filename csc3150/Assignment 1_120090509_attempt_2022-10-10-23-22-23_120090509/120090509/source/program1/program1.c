#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  /* fork a child process */
  pid_t pid;
  int status;

  printf("Process start to fork \n");
  pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(1);
  }

  else {

    if (pid == 0) {

      /// Child Process ///

      int i;
      char *arg[argc];

      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      /* execute test program */
      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute test program:\n");
      execve(arg[0], arg, NULL);

      /* check if the child process is replaced by new process */
      printf("Continue to run original child process!\n");
      perror("execve");
      exit(EXIT_FAILURE);

    }

    else {

      printf("I'm the Parent Process, my pid = %d\n", getpid());

      waitpid(-1, &status, WUNTRACED);

      printf("Parent process receives SIGCHLD signal\n");

      /* normal exit */
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      }

      /*abnormal exit and send the siganl */
      else if (WIFSIGNALED(status)) {

        int signal = WTERMSIG(status);

        // abort
        if (signal == 6) {
        }

        // alarm
        else if (signal == 14) {
        }

        // bus
        else if (signal == 7) {
        }

        // floating
        else if (signal == 8) {
        }

        // hangup
        else if (signal == 1) {
        }

        // illgal_instr
        else if (signal == 4) {
        }

        // interrupt
        else if (signal == 2) {
        }

        // kill
        else if (signal == 9) {
        }

        // pipe
        else if (signal == 13) {
        }

        // quit
        else if (signal == 3) {
        }

        // segment_fault
        else if (signal == 11) {
        }

        // terminates
        else if (signal == 15) {
        }

        // trap
        else if (signal == 5) {
        }
      }

      /* stop signal*/
      else if (WIFSTOPPED(status)) {
        printf("CHILD PROCESS STOPPED\n");
      }

      else {
        printf("CHILD PROCESS CONTINUED\n");
      }

      exit(0);
    }
  }
}
