#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  /* fork a child process */
  int status;
  pid_t pid;

  printf("Process start to fork\n");
  pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(1);
  } else {
    /* child process */
    if (pid == 0) {

      char *new_arg[argc];
      for (int i = 0; i < argc - 1; i++) {
        new_arg[i] = argv[i + 1];
      }
      new_arg[argc - 1] = NULL;

      /* execute test program */
      printf("I'm the Child Process, my pid = %d\n", getpid());
      raise(SIGCHLD);
      printf("Child process start to execute test program:\n");
      execve(new_arg[0], new_arg, NULL);

    }
    /* parent process */
    else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      /* wait for child process terminates */
      waitpid(-1, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");

      /* check child process'  termination status */

      if (WIFEXITED(status)) {
        printf("Normal termination with exit status = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        switch (WTERMSIG(status)) {
        case 1: /* hangup */
          printf("child process get SIGHUP signal\n");
          break;
        case 2: /* interrupt */
          printf("child process get SIGINT signal\n");
          break;
        case 3: /* quit */
          printf("child process get SIGQUIT signal\n");
          break;
        case 4: /* illigal_instr */
          printf("child process get SIGILL signal\n");
          break;
        case 5: /* trap */
          printf("child process get SIGTRAP signal\n");
          break;
        case 6: /* abort */
          printf("child process get SIGABRT signal\n");
          break;
        case 7: /* bus */
          printf("child process get SIGBUS signal\n");
          break;
        case 8: /* floating */
          printf("child process get SIGFPE signal\n");
          break;
        case 9: /* kill */
          printf("child process get SIGKILL signal\n");
          break;
        case 11: /* segment_fault */
          printf("child process get SIGSEGV signal\n");
          break;
        case 13: /* pipe */
          printf("child process get SIGPIPE signal\n");
          break;
        case 14: /* alarm */
          printf("child process get SIGALRM signal\n");
          break;
        case 15: /* terminate */
          printf("child process get SIGTERM signal\n");
        default:
          break;
        }

      } else if (WIFSTOPPED(status)) {
        printf("child process get SIGSTOP signal\n");
      }

      exit(0);
    }
  }

  return 0;
}
