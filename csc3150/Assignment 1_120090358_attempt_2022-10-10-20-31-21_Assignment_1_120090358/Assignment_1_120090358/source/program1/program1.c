#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  /* fork a child process */
  pid_t pid;
  int status;

  printf("Process start to fork\n");
  pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(1);
  } else {
    // child process
    if (pid == 0) {
      int i;
      char *arg[argc];

      printf("I'm the Child Process, my pid = %d\n", getpid());

      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      printf("Child process start to execute test program:\n");
      execve(arg[0], arg, NULL);
      raise(SIGCHLD);
    } else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());

      waitpid(pid, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        switch (WTERMSIG(status)) {
        case 6: {
          printf("child process get SIGABRT signal\n");
          break;
        }
        case 7: {
          printf("child process get SIGBUS signal\n");
          break;
        }
        case 8: {
          printf("child process get SIGFPE signal\n");
          break;
        }
        case 1: {
          printf("child process get SIGUP signal\n");
          break;
        }
        case 4: {
          printf("child process get SIGILL signal\n");
          break;
        }
        case 2: {
          printf("child process get SIGINT signal\n");
          break;
        }
        case 9: {
          printf("child process get SIGKILL signal\n");
          break;
        }
        case 13: {
          printf("child process get SIGPIPE signal\n");
          break;
        }
        case 3: {
          printf("child process get SIGQUIT signal\n");
          break;
        }
        case 14: {
          printf("child process get SIGALRM signal\n");
          break;
        }
        case 11: {
          printf("child process get SIGSEGV signal\n");
          break;
        }
        case 15: {
          printf("child process get SIGTERM signal\n");
          break;
        }
        case 5: {
          printf("child process get SIGTRAP signal\n");
          break;
        }
        default: { break; }
        }
      } else if (WIFSTOPPED(status)) {
        printf("child process get SIGSTOP signal\n");
      } else {
        printf("CHILD PROCESS CONTINUED\n");
      }
      exit(0);
    }
  }
}