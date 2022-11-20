#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void check(int status) {
  if (status == 134) {
    printf("child process get SIGABRT signal\n");
  } else if (status == 14) {
    printf("child process get SIGALRM signal\n");
  } else if (status == 135) {
    printf("child process get SIGBUS signal\n");
  } else if (status == 136) {
    printf("child process get SIGFPE signal\n");
  } else if (status == 1) {
    printf("child process get SIGHUP signal\n");
  } else if (status == 132) {
    printf("child process get SIGILL signal\n");
  } else if (status == 2) {
    printf("child process get SIGINT signal\n");
  } else if (status == 9) {
    printf("child process get SIGKILL signal\n");
  } else if (status == 0) {
    printf("Normal termination with EXIT STATUS = 0\n");
  } else if (status == 13) {
    printf("child process get SIGPIPE signal\n");
  } else if (status == 131) {
    printf("child process get SIGQUIT signal\n");
  } else if (status == 139) {
    printf("child process get SIGSEGV signal\n");
  } else if (status == 15) {
    printf("child process get SIGTERM signal\n");
  } else if (status == 133) {
    printf("child process get SIGTRAP signal\n");
  } else if (status == 4991) {
    printf("child process get SIGSTOP signal\n");
  } else {
    printf("This is a signal we can't know");
  }

  return;
}

int main(int argc, char *argv[]) {
  pid_t pid;
  int status;
  printf("Process start to fork\n");
  pid = fork();
  // printf("YES, It can work!");
  if (pid == -1) { // the creation of a child process was unsuccessful
    perror("fork");
    exit(1);
  } else { // the creation of a child process was successful

    if (pid == 0) { // child process

      int i;
      char *arg[argc];
      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;
      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute test program:\n");
      // execute the test program
      execve(arg[0], arg, NULL);
      // finish the execute and send the signal/return the status signal
      // raise(SIGCHLD); // ? This line can't be sent ????????????????
    }

    else { // parent process use wait function
      printf("I'm the Parent Process, my pid = %d\n", getpid());

      waitpid(pid, &status, WUNTRACED); // receive the signal from the child
      printf("Parent process receives SIGCHLD signal\n");

      check(status);

      // NORMAL
      if (WIFEXITED(status)) {
        // printf("Normal termination with EXIT STATUS = %d\n",
        //        WEXITSTATUS(status));
      }

      // signaled abort
      else if (WIFSIGNALED(status)) {
        // printf("CHILD PROCESS FAILED : %d\n", WTERMSIG(status));
      }

      // stopped
      else if (WIFSTOPPED(status)) {
        // printf("CHILD PROCESS STOPPED : %d\n", WSTOPSIG(status));
      } else {
        // printf("CHILD PROCESS CONTINUED\n");
      }
      exit(0);
    }
  }
  return 0;
}