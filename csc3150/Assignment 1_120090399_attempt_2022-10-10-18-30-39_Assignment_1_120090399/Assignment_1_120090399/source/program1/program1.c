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

  /* execute test program */
  if (pid == -1) {
    perror("fork");
    exit(1);
  }

  /* wait for child process terminates */
  else {
    if (pid == 0) {
      int i;
      char *arg[argc];

      printf("I'm the Child Process, my pid = %d\n", getpid());
      for (i = 0; i < argc; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      printf("Child process start to execute test program\n");
      execve(arg[0], arg, NULL);

      perror("execve");
      exit(EXIT_FAILURE);
    }

    /* check child process'  termination status */
    else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      waitpid(pid, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");

      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      }

      else if (WIFSIGNALED(status)) {
        int TerminationSignal = WTERMSIG(status);
        if (TerminationSignal == 1) {
          printf("child process get SIGHUP signal\n");
        } else if (TerminationSignal == 2) {
          printf("child process get SIGINT signal\n");
        } else if (TerminationSignal == 3) {
          printf("child process get SIGQUIT signal\n");
        } else if (TerminationSignal == 4) {
          printf("child process get SIGILL signal\n");
        } else if (TerminationSignal == 5) {
          printf("child process get SIGTRAP signal\n");
        } else if (TerminationSignal == 6) {
          printf("child process get SIGABRT signal\n");
        } else if (TerminationSignal == 7) {
          printf("child process get SIGBUS signal\n");
        } else if (TerminationSignal == 8) {
          printf("child process get SIGFPE signal\n");
        } else if (TerminationSignal == 9) {
          printf("child process get SIGKILL signal\n");
        } else if (TerminationSignal == 11) {
          printf("child process get SIGSEGV signal\n");
        } else if (TerminationSignal == 13) {
          printf("child process get SIGPIPE signal\n");
        } else if (TerminationSignal == 14) {
          printf("child process get SIGALRM signal\n");
        } else if (TerminationSignal == 15) {
          printf("child process get SIGTERM signal\n");
        }
      }

      else if (WIFSTOPPED(status)) {
        int StopSignal = WSTOPSIG(status);
        if (StopSignal == 19) {
          printf("child process get STOPPED signal\n");
        }
      }
    }
  }
}
