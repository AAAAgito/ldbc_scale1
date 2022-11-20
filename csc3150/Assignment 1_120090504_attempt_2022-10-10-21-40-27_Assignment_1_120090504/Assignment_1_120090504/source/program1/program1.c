#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  pid_t pid;
  int status;

  printf("Process start to fork\n");
  pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(1);
  } else {
    /* fork a child process */
    // child process
    if (pid == 0) {
      int i;
      char *arg[argc];

      for (i = 0; i < argc; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute test program\n");
      /* execute test program */
      execve(arg[0], arg, NULL);

      perror("execve");
      exit(EXIT_FAILURE);

    }

    // parent process
    else {
      /* wait for child process terminates */

      printf("I'm the Parent Process, my pid = %d\n", getpid());
      waitpid(pid, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");
      /* check child process'  termination status */
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        printf("CHILD EXECUTION FAILED: %d\n", WTERMSIG(status));
      } else if (WIFSTOPPED(status)) {
        printf("CHILD PROCESS STOPPED: %d\n", WSTOPSIG(status));
      } else {
        printf("CHILD PROCESS CONTINUED\n");
      }

      exit(0);
    }
  }

  return 0;
}
