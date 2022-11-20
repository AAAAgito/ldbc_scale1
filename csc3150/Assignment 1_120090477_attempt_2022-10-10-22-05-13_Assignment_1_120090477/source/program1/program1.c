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
  }

  else {
    if (pid == 0) {
      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute test program:\n");
      int i;
      char *arg[argc];
      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;
      execve(arg[0], arg, NULL);
      perror("execve");
      exit(EXIT_FAILURE);
    }

    else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      waitpid(-1, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        int s = WTERMSIG(status);
        switch (s) {
        case 6:
          printf("Child process get SIGABRT signal\n");
          break;
        case 14:
          printf("Child process get SIGALRM signal\n");
          break;
        case 7:
          printf("Child process get SIGBUS signal\n");
          break;
        case 8:
          printf("Child process get SIGFPE signal\n");
          break;
        case 1:
          printf("Child process get SIGHUP signal\n");
          break;
        case 4:
          printf("Child process get SIGILL signal\n");
          break;
        case 2:
          printf("Child process get SIGINT signal\n");
          break;
        case 9:
          printf("Child process get SIGKILL signal\n");
          break;
        case 13:
          printf("Child process get SIGPIPE signal\n");
          break;
        case 3:
          printf("Child process get SIGQUIT signal\n");
          break;
        case 11:
          printf("Child process get SIGSEGV signal\n");
          break;
        case 15:
          printf("Child process get SIGTERM signal\n");
          break;
        case 5:
          printf("Child process get SIGTRAP signal\n");
          break;
        default:
          break;
        }
        // printf("CHILD EXECUTION FAILED: %d\n", WTERMSIG(status));
      } else if (WIFSTOPPED(status)) {
        printf("Child process get SIGSTOP signal\n");
      } else {
        printf("Child process get SIGCONT signal\n");
      }
      exit(0);
    }
  }
  return 0;

  /* fork a child process */

  /* execute test program */

  /* wait for child process terminates */

  /* check child process'  termination status */
}
