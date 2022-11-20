#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  /* fork a child process */
  printf("Process start to fork\n");
  int status;
  pid_t pid = fork();

  if (pid < 0) {
    printf("Fork failed\n");
    perror("fork");
    exit(EXIT_FAILURE);
  }

  else {

    if (pid == 0) {

      printf("I'm the Child Process, my pid = %d\n", getpid());
      char *arg[argc];
      for (int i = 1; i < argc; i++) {
        arg[i - 1] = argv[i];
      }
      arg[argc - 1] = NULL;

      /* execute test program */
      printf("Child process start to execute test program:\n");
      execve(arg[0], arg, NULL);
      perror("execve");
      exit(EXIT_FAILURE);
    }

    else {

      printf("I'm the Parent Process, my pid = %d\n", getpid());

      /* wait for child process terminates */
      waitpid(pid, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");

      /* check child process'  termination status */
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {

        switch (WTERMSIG(status)) {
        case SIGHUP: // 1,hangup
          printf("child process get SIGHUP signal\n");
          break;
        case SIGINT: // 2,interrupt
          printf("child process get SIGINT signal\n");
          break;
        case SIGQUIT: // 3,quit
          printf("child process get SIGQUIT signal\n");
          break;
        case SIGILL: // 4,illegal_instr
          printf("child process get SIGILL signal\n");
          break;
        case SIGTRAP: // 5,trap
          printf("child process get SIGTRAP signal\n");
          break;
        case SIGABRT: // 6,abort
          printf("child process get SIGABRT signal\n");
          break;
        case SIGBUS: // 7,bus
          printf("child process get SIGBUS signal\n");
          break;
        case SIGFPE: // 8,floating
          printf("child process get SIGFPE signal\n");
          break;
        case SIGKILL: // 9,kill
          printf("child process get SIGKILL signal\n");
          break;
        case SIGSEGV: // 11,segment_fault
          printf("child process get SIGSEGV signal\n");
          break;
        case SIGPIPE: // 13,pipe
          printf("child process get SIGPIPE signal\n");
          break;
        case SIGALRM: // 14,alarm
          printf("child process get SIGALRM signal\n");
          break;
        case SIGTERM: // 15,terminate
          printf("child process get SIGTERM signal\n");
          break;
        default:
          printf("child process encounters other situation\n");
          break;
        }
      } else if (WIFSTOPPED(status)) {
        printf("child process get SIGSTOP signal\n");
      } else {
        printf("child process continued\n");
      }
      exit(EXIT_SUCCESS);
    }
  }
}
