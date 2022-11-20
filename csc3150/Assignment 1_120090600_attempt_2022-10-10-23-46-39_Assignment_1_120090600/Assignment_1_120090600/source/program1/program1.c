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
  pid = fork(); // fork a child process

  if (pid == -1) // fork failed
  {
    perror("fork");
    exit(1);
  } else {
    if (pid == 0) // child process
    {
      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute test program:\n");
      int i;
      char *arg[argc];
      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;
      execve(arg[0], arg, NULL);
      //  error
      printf("Continue to run the original process.\n");
      perror("execve");
      exit(EXIT_FAILURE);
    } else // parent process
    {
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      waitpid(pid, &status, WUNTRACED);
      printf("Parent process receiving the SIGCHLD signal\n");
      /* child process'status */
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        if (WTERMSIG(status) == 1) {
          printf("child process get SIGHUP signal\n");
        } else if (WTERMSIG(status) == 2) {
          printf("child process get SIGINT signal\n");
        } else if (WTERMSIG(status) == 3) {
          printf("child process get SIGQUIT signal\n");
        } else if (WTERMSIG(status) == 4) {
          printf("child process get SIGILL signal\n");
        } else if (WTERMSIG(status) == 5) {
          printf("child process get SIGTRAP signal\n");
        }
        if (WTERMSIG(status) == 6) {
          printf("child process get SIGABRT signal\n");
        } else if (WTERMSIG(status) == 7) {
          printf("child process get SIGBUS signal\n");
        } else if (WTERMSIG(status) == 8) {
          printf("child process get SIGFPE signal\n");
        } else if (WTERMSIG(status) == 9) {
          printf("child process get SIGKILL signal\n");
        } else if (WTERMSIG(status) == 11) {
          printf("child process get SIGSEGV signal\n");
        } else if (WTERMSIG(status) == 13) {
          printf("child process get SIGPIPE signal\n");
        } else if (WTERMSIG(status) == 14) {
          printf("child process get SIGALRM signal\n");
        } else if (WTERMSIG(status) == 15) {
          printf("child process get SIGTERM signal\n");
        }
        printf("CHILD EXECUTION FAILED\n");
      } else if (WIFSTOPPED(status)) {
        printf("child process get SIGSTOP signal\n");
      } else {
        printf("CHILD PROCESS CONTINUED\n");
      }
      exit(1);
    }
  }
}