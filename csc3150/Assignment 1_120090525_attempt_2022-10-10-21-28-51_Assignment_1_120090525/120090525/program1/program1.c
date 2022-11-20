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

  printf("Process start to fork\n");
  pid = fork();

  if (pid < 0) {
    printf("Fork error!\n");
  } else {

    // Child process
    if (pid == 0) {
      int i;
      char *arg[argc];
      printf("I'm tbe Child Process, my pid is %d\n", getpid());

      /* execute test program */
      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      printf("Child process start to execute test program:\n");
      raise(SIGCHLD);
      execve(arg[0], arg, NULL);

      printf("Continue to run original child process!\n");

      perror("execve");
      exit(EXIT_FAILURE);
    }

    // Parent process
    else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      /* wait for child process terminates */
      waitpid(pid, &status, WUNTRACED);
      printf("Parent process receving the SIGCHLD signal\n");
      /* check child process' termination status */
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        switch (WTERMSIG(status)) {
        case 6:
          printf("Child process is aborted by abort signal\n");
          printf("SIGABRT signal was raised in child process\n");
          break;
        case 14:
          printf("Child process is alarmed by alarm signal\n");
          printf("SIGALRM signal was raised in child process\n");
          break;
        case 7:
          printf("Child process is bus errored by bus signal\n");
          printf("SIGBUS signal was raised in child process\n");
          break;
        case 8:
          printf(
              "Child process is floating-point excepted by floating signal\n");
          printf("SIGFPE signal was raised in child process\n");
          break;
        case 1:
          printf("Child process is hunguped by hungup signal\n");
          printf("SIGHUP signal was raised in child process\n");
          break;
        case 4:
          printf("Child process is illegally instructed by illegal_instr "
                 "signal\n");
          printf("SIGILL signal was raised in child process\n");
          break;
        case 2:
          printf("Child process is interrupted by interrupt signal\n");
          printf("SIGINT signal was raised in child process\n");
          break;
        case 9:
          printf("Child process is terminat by killed signal\n");
          printf("SIGKILL signal was raised in child process\n");
          break;
        case 13:
          printf("Child process is broken by pipe signal\n");
          printf("SIGPIPE signal was raised in child process\n");
          break;
        case 3:
          printf("Child process is quitted by quit signal\n");
          printf("SIGQUIT signal was raised in child process\n");
          break;
        case 11:
          printf("Child process is segmentated by segment_fault signal\n");
          printf("SIGSEGV signal was raised in child process\n");
          break;
        case 15:
          printf("Child process is terminated by terminate signal\n");
          printf("SIGTERM signal was raised in child process\n");
          break;
        case 5:
          printf("Child process is traped by trap signal\n");
          printf("SIGTRAP signal was raised in child process\n");
          break;
        }
      } else if (WIFSTOPPED(status)) {
        printf("CHILD PROCESS STOPPED\n");
      } else {
        printf("CHILD PROCESS CONTINUED\n");
      }
      exit(0);
    }
  }
  return 0;
}
