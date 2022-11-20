#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int state;
  int status;
  pid_t pid = fork();

  if (pid < 0) {
    printf("Fork error!\n");
  } else {
    if (pid == 0) {
      int i;
      char *arg[argc];
      printf("Process start to fork\n");
      for (i = 0; i < argc - 1; i++) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;
      printf("I'm the Parent Process, my pid = %d\n", getppid());
      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute test program:\n");
      execve(arg[0], arg, NULL);
      printf("fail!\n");
      perror("execve");
      exit(EXIT_FAILURE);
    } else {
      waitpid(pid, &status, WUNTRACED);
      unsigned char low_8 = status & 0x7f;
      // unsigned char high_8 = status>>8;
      printf("Parent process recieves SIGCHLD signal\n");
      if (WIFEXITED(status)) {
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        int8_t sighub = 1;
        int8_t sigint = 2;
        int8_t quit = 3;
        int8_t sigill = 4;
        int8_t trap = 5;
        int8_t abort = 6;
        int8_t bus = 7;
        int8_t flo = 8;
        int8_t kill = 9;
        int8_t segv = 11;
        int8_t pipe = 13;
        int8_t term = 15;
        if (low_8 == abort) {
          printf("child process get SIGABRT signal\n");
        } else if (low_8 == bus) {
          printf("child process get SIGBUS signal\n");
        } else if (low_8 == flo) {
          printf("child process get SIGFPE signal\n");
        } else if (low_8 == sighub) {
          printf("child process get SIGHUB signal\n");
        } else if (low_8 == sigill) {
          printf("child process get SIGILL signal\n");
        } else if (low_8 == sigint) {
          printf("child process get SIGINT signal\n");
        } else if (low_8 == kill) {
          printf("child process get SIGKILL signal\n");
        } else if (low_8 == pipe) {
          printf("child process get SIGPIPE signal\n");
        } else if (low_8 == quit) {
          printf("child process get SIGQUIT signal\n");
        } else if (low_8 == segv) {
          printf("child process get SIGSEGV signal\n");
        } else if (low_8 == term) {
          printf("child process get SIGTERM signal\n");
        } else if (low_8 == trap) {
          printf("child process get SIGTRAP signal\n");
        }
      } else if (WIFSTOPPED(status)) {
        printf("child process get SIGSTOP signal\n");
      } else {
        printf("chile continiue");
      }
      exit(0);
    }
  }
  return 0;
}
