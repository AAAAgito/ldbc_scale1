#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  pid_t pid;
  int status;

  printf("Process start to fork\n");
  pid = fork();

  if (pid == -1) { /*创建进程失败*/
    perror("fork");
    exit(1);
  } else {
    if (pid == 0) {
      char *arg[argc];
      for (int i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;

      printf("I'm the Child Process, my pid = %d\n",
             getpid()); /* execute test program */
      printf("Child process start to execute test program:\n");
      execve(arg[0], arg, NULL);

      printf("Run original child process!\n"); /* check if the child process is
                                                  replaced by new process */
      perror("execve");
      exit(EXIT_FAILURE);
    } else {
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      waitpid(-1, &status, WUNTRACED);
      printf("Parent process receives SIGCHLD signal\n");

      if (WIFEXITED(status)) { /* case 1: normal exit */
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      }

      else if (WIFSIGNALED(
                   status)) { /* case 2: abnormal exit and send the siganl */

        int signal = WTERMSIG(status);

        if (signal == 6) { // abort
          printf("Child process get SIGABRT signal\n");
          printf("Child process is abort by abort signal\n");
        }

        else if (signal == 14) { // alarm
          printf("Child process get SIGALRM signal\n");
          printf("Child process is abort by alarm signal\n");
        }

        else if (signal == 7) { // bus
          printf("Child process get SIGBUS signal\n");
          printf("Child process is abort by BUS signal\n");
        }

        else if (signal == 8) { // floating
          printf("Child process get SIGFPE signal\n");
          printf("Child process is abort by SIGFPE signal\n");
        }

        else if (signal == 1) { // hangup
          printf("Child process get SIGHUP signal\n");
          printf("Child process is hung up\n");
        }

        else if (signal == 4) { // illgal_instr
          printf("Child process get SIGILL signal\n");
          printf("Child process is abort by SIGILL signal\n");
        }

        else if (signal == 2) { // interrupt
          printf("Child process get SIGINT signal\n");
          printf("Child process is abort by SIGINT signal\n");
        }

        else if (signal == 9) { // kill
          printf("Child process get SIGKILL signal\n");
          printf("Child process is abort by SIGKILL signal\n");
        }

        else if (signal == 13) { // pipe
          printf("Child process get SIGPIPE signal\n");
          printf("Child process is abort by SIGPIPE signal\n");
        }

        else if (signal == 3) { // quit
          printf("Child process get SIGQUIT signal\n");
          printf("Child process is abort by SIGQUIT signal\n");
        }

        else if (signal == 11) { // segment_fault
          printf("Child process get SIGSEGV signal\n");
          printf("Child process is abort by SIGSEGV signal\n");
        }

        else if (signal == 15) { // terminates
          printf("Child process get SIGTERM signal\n");
          printf("Child process is abort by SIGTERM signal\n");
        }

        else if (signal == 5) { // trap
          printf("Child process get SIGTRAP signal\n");
          printf("Child process is abort by SIGTRAP signal\n");
        }
      }

      else if (WIFSTOPPED(status)) { /* case 3: stop signal*/
        printf("Child process get SIGSTOP signal\n");
        printf("Child process stopped\n");
        printf("CHILD EXECUTION STOPPED\n");
      }

      else {
        printf("Child PROCESS CONTINUED \n");
      }
      exit(0);
    }
  }

  return 0;
}