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
    if (pid == 0) {
      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute test program\n");
      /* execute test program */
      int i;
      char *arg[argc];
      for (int i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      execve(arg[0], arg, NULL);
      // printf("run\n");
      // perror("execve");
      // exit(EXIT_FAILURE);
      raise(SIGKILL);
    } else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      /* wait for child process terminates */
      waitpid(pid, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");

      /* check child process'  termination status */
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        char status_name[10];
        int status_num = WTERMSIG(status);
        switch (status_num) {
        case 1:
          strcpy(status_name, "SIGHUP");
          break;
        case 2:
          strcpy(status_name, "SIGINT");
          break;
        case 3:
          strcpy(status_name, "SIGQUIT");
          break;
        case 4:
          strcpy(status_name, "SIGILL");
          break;
        case 5:
          strcpy(status_name, "SIGTRAP");
          break;
        case 6:
          strcpy(status_name, "SIGABRT");
          break;
        case 7:
          strcpy(status_name, "SIGBUS");
          break;
        case 8:
          strcpy(status_name, "SIGFPE");
          break;
        case 9:
          strcpy(status_name, "SIGKILL");
          break;
        case 10:
          strcpy(status_name, "SIGUSR1");
          break;
        case 11:
          strcpy(status_name, "SIGSEGV");
          break;
        case 12:
          strcpy(status_name, "SIGUSR2");
          break;
        case 13:
          strcpy(status_name, "SIGPIPE");
          break;
        case 14:
          strcpy(status_name, "SIGALRM");
          break;
        case 15:
          strcpy(status_name, "SIGTERM");
          break;
        case 16:
          strcpy(status_name, "SIGSTKFLT");
          break;
        case 17:
          strcpy(status_name, "SIGCHLD");
          break;
        case 18:
          strcpy(status_name, "SIGCONT");
          break;
        case 19:
          strcpy(status_name, "SIGSTOP");
          break;
        case 20:
          strcpy(status_name, "SIGTSTP");
          break;
        case 21:
          strcpy(status_name, "SIGTTIN");
          break;
        case 22:
          strcpy(status_name, "SIGTTOU");
          break;
        case 23:
          strcpy(status_name, "SIGURG");
          break;
        case 24:
          strcpy(status_name, "SIGXCPU");
          break;
        case 25:
          strcpy(status_name, "SIGXFSZ");
          break;
        case 26:
          strcpy(status_name, "SIGVTALRM");
          break;
        case 27:
          strcpy(status_name, "SIGPROF");
          break;
        case 28:
          strcpy(status_name, "SIGWINCH");
          break;
        case 29:
          strcpy(status_name, "SIGIO");
          break;
        case 30:
          strcpy(status_name, "SIGPWR");
          break;
        case 31:
          strcpy(status_name, "SIGSYS");
          break;
        default:
          break;
        }
        printf("child process get %s signal\n", status_name);
      } else if (WIFSTOPPED(status)) {
        printf("child process get SIGSTOP signal\n");
      } else {
        printf("CHILD PROCESS CONTINUE\n");
      }
    }
  }
  return 0;
}
