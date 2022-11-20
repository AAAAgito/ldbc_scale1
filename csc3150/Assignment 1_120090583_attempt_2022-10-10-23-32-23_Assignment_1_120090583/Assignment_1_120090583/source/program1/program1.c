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
    printf("fork error");
    exit(1);
  } else if (pid == 0) {
    printf("I'm the child process, my pid = %d\n", getpid());
    char *arg[argc];
    for (int i = 0; i < argc - 1; i++) {
      arg[i] = argv[i + 1];
    }
    arg[argc - 1] = NULL;
    printf("Child process start to execute test program:\n");
    execve(arg[0], arg, NULL);

    printf("Fail to execve!\n");
  }

  /* wait for child process terminates */
  else {
    printf("I'm the parent process, my pid = %d\n", getpid());
    waitpid(pid, &status, WUNTRACED);
    printf("Parent process receives SIGCHILD signal\n");

    /* check child process'  termination status */
    if (WIFEXITED(status)) {
      printf("Normal termination with EXIT STATUS = %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("CHILD EXECUTION FAILED: %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
      printf("CHILD PROCESS STOPPED\n");
    }
    exit(0);
  }
}
