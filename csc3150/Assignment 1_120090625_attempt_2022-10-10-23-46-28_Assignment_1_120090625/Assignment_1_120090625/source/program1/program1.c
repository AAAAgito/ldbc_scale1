#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *allSignal[] = {"SIGHUP",  "SIGINT", "SIGQUIT", "SIGILL",  "SIGTRAP",
                     "SIGABRT", "SIGBUS", "SIGFPE",  "SIGKILL", NULL,
                     "SIGSEGV", NULL,     "SIGPIPE", "SIGALRM", "SIGTERM",
                     NULL,      NULL,     NULL,      "SIGSTOP", NULL};

int main(int argc, char *argv[]) {
  pid_t pid;
  int status;

  printf("Process start to fork\n");

  /* fork a child process */
  pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  } else {
    // Child process
    if (pid == 0) {
      int i;
      char *arg[argc];

      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute test program:\n");

      /* execute test program */
      execve(arg[0], arg, NULL);

      perror("execve");
      exit(SIGCHLD);
    }
    // Parent process
    else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());

      /* wait for child process terminates */
      waitpid(-1, &status, WUNTRACED);

      printf("Parent process receives SIGCHLD signal\n");

      /* check child process'  termination status */
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        printf("child process get %s signal\n",
               allSignal[WTERMSIG(status) - 1]);
      } else if (WIFSTOPPED(status)) {

        printf("child process get SIGSTOP signal\n");
      } else {
        printf("CHILD PROCESS CONTINUED\n");
      }

      exit(0);
    }
  }
}
