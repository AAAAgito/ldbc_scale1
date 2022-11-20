#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  pid_t pid;
  int status;

  /* fork a child process */
  printf("Process start to fork\n");
  pid = fork();

  if (pid == -1) {
    // For error
    perror("fork");
    exit(1);
  }

  else {
    // For child process
    if (pid == 0) {

      printf("I'm the Child Process:, my pid = %d\n", getpid());

      int i;
      char *arg[argc];
      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      /* execute test program */
      printf("Child process start to execute test program:\n");
      execve(arg[0], arg, NULL);

      // Check whether the child process is replaced by the new program
      printf("Continue to run original child process!\n");

      perror("execve");
      exit(EXIT_FAILURE);

    }

    else {
      // For parent process
      printf("I'm the Parent Process:, my pid = %d\n", getpid());

      /* wait for child process terminates */
      waitpid(pid, &status, WUNTRACED);
      // print the receives
      printf("Parent process receives SIGCHLD signal\n");

      /* check child process'  termination status */

      // status error here?

      if (WIFEXITED(status)) {
        // normal
        printf(
            "Normal termination with EXIT STATUS = 0\n"); // WIFEXITED(status) 1
      }

      else if (WIFSIGNALED(status)) {

        // abort
        if (WTERMSIG(status) == SIGABRT) {
          printf("Child process is aborted by abort signal!\n");
          printf("SIGABRT was raised in child process\n");
        }

        // alarm
        else if (WTERMSIG(status) == SIGALRM) {
          printf("Child process is aborted by alarm signal!\n");
          printf("SIGALRM was raised in child process\n");
        }

        // bus
        else if (WTERMSIG(status) == SIGBUS) {
          printf("Child process is aborted by bus signal!\n");
          printf("SIGBUS was raised in child process\n");
        }
        // floating
        else if (WTERMSIG(status) == SIGFPE) {
          printf("Child process is aborted by floating signal!\n");
          printf("SIGFPE was raised in child process\n");
        }
        // hangup
        else if (WTERMSIG(status) == SIGHUP) {
          printf("Child process is aborted by hangup signal!\n");
          printf("SIGHUP was raised in child process\n");
        }
        // illegal_instr
        else if (WTERMSIG(status) == SIGILL) {
          printf("Child process is aborted by illegal_instr signal!\n");
          printf("SIGILL was raised in child process\n");
        }
        // interrupt
        else if (WTERMSIG(status) == SIGINT) {
          printf("Child process is aborted by interrupt signal!\n");
          printf("SIGINT was raised in child process\n");
        }
        // kill
        else if (WTERMSIG(status) == SIGKILL) {
          printf("Child process is aborted by kill signal!\n");
          printf("SIGKILL was raised in child process\n");
        }
        // pipe
        else if (WTERMSIG(status) == SIGPIPE) {
          printf("Child process is aborted by pipe signal!\n");
          printf("SIGPIPE was raised in child process\n");
        }
        // quit
        else if (WTERMSIG(status) == SIGQUIT) {
          printf("Child process is aborted by quit signal!\n");
          printf("SIGQUIT was raised in child process\n");
        }
        // segment_fault
        else if (WTERMSIG(status) == SIGSEGV) {
          printf("Child process is aborted by segment_fault signal!\n");
          printf("SIGSEGV was raised in child process\n");
        }
        // terminate
        else if (WTERMSIG(status) == SIGTERM) {
          printf("Child process is aborted by terminate signal!\n");
          printf("SIGTERM was raised in child process\n");
        }
        // trap
        else if (WTERMSIG(status) == SIGTRAP) {
          printf("Child process is aborted by trap signal!\n");
          printf("SIGTRAP was raised in child process\n");
        }

      }

      else if (WIFSTOPPED(status)) {
        // stop
        printf("Child process is stopped by stop signal!\n");
        printf("SIGSTOP was raised in child process\n");
        printf("CHILD PROCESS STOPPED\n");

      }

      else {
        printf("CHILD PROCESS CONTINUED\n");
      }

      exit(0);
    }
  }
}
