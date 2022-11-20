#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *WGETSIGNAL(int status) {
  char *INT_TO_SIGNALS[] = {"SIGHUP",  "SIGINT",  "SIGQUIT", "SIGILL",
                            "SIGTRAP", "SIGABRT", "SIGBUS",  "SIGFPE",
                            "SIGKILL", NULL,      "SIGSEGV", NULL,
                            "SIGPIPE", "SIGALRM", "SIGTERM"};

  return INT_TO_SIGNALS[status - 1];
}

int main(int argc, char *argv[]) {
  /* fork a child process */
  int status;
  printf("Process start to fork.\n");
  pid_t pid = fork();

  /* execute test program */
  if (pid < 0) {
    printf("Fork ERROR\n");
  } else {
    /*	The Chid Process */
    if (pid == 0) {
      int i;
      char *arg[argc];  //	Use another char array to carry the file name.
      printf("I'm the Child process, my pid = %d \n", getpid());

      for (int i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;  //	Set the last byte to NULL manually.

      printf("Child process start to execute test program: \n");

      int EXEC_FLAG = execve(arg[0], arg, NULL);
      if (EXEC_FLAG == -1) {
        printf("ERROR: File Executing FAILED! \n");
      }
    }

    /* The Parent Process */
    else {
      printf("I'm the Parent process, my pid = %d \n", getpid());

      /* wait for child process terminates */
      waitpid(pid, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");

      /* check child process'  termination status */
      if (WIFEXITED(status)) {
        //	Child process exited under control.
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        //	Child process exited because of a signal.
        printf("CHILD EXECUTION FAILED: %d %s raised\n", WTERMSIG(status),
               WGETSIGNAL(WTERMSIG(status)));
      } else if (WIFSTOPPED(status)) {
        //	child process was stopped by a signal.
        printf("CHILD PROCESS STOPPED\n");
        printf("STOP SIG = %d\n", WSTOPSIG(status));
      } else {
        printf("CHILD PROCESS CONTINUED.\n");
      }
    }
  }
  return 0;
}
