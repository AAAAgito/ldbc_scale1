#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *Signal_info(int sig) {
  switch (sig) {
  case 1:
    return "Child process is hung up\nSIGHUP signal was raised in child "
           "process\n";
    break;
  case 2:
    return "Child process is aborted by SIGINT signal\nSIGINT signal was "
           "raised in child process\n";
    break;
  case 3:
    return "Child process is aborted by SIGQUIT signal\nSIGQUIT signal was "
           "raised in child process\n";
    break;
  case 4:
    return "Child process is aborted by SIGILL signal\nSIGILL signal was "
           "raised in child process\n";
    break;
  case 5:
    return "Child process is aborted by trap signal\nSIGTRAP signal was raised "
           "in child process\n";
    break;
  case 6:
    return "Child process is aborted by abort signal\nSIGABRT signal was "
           "raised in child process\n";
    break;
  case 7:
    return "Child process is aborted by bus signal\nSIGBUS signal was raised "
           "in child process\n";
    break;
  case 8:
    return "Child process is aborted by SIGFPE signal\nSIGFPE signal was "
           "raised in child process\n";
    break;
  case 9:
    return "Child process is aborted by SIGKILL signal\nSIGKILL signal was "
           "raised in child process\n";
    break;
  case 11:
    return "Child process is aborted by SIGSEGV signal\nSIGSEGV signal was "
           "raised in child process\n";
    break;
  case 13:
    return "Child process is aborted by SIGPIPE signal\nSIGPIPE signal was "
           "raised in child process\n";
    break;
  case 14:
    return "Child process is aborted by alarm signal\nSIGALRM signal was "
           "raised in child process\n";
    break;
  case 15:
    return "Child process is aborted by SIGTERM signal\nSIGTERM signal was "
           "raised in child process\n";
    break;
  case 19:
    return "SIGSTOP";
    break;
  default:
    return "UNKOWN";
    break;
  }
  return "UNKNOWN";
}

void ParentMessage() { printf("Parent Process receives the SIGCHLD signal\n"); }

int main(int argc, char *argv[]) {
  pid_t pid;
  int status;
  pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  }

  else {
    if (pid == 0) {
      char *arg[argc];
      for (int i = 0; i < argc - 1; ++i) {
        arg[i] = argv[i + 1];
      }
      arg[argc - 1] = NULL;
      printf("I'm the Child Process, my pid = %d\n", getpid());
      printf("Child process start to execute the program:\n");
      execve(arg[0], arg, NULL);

      printf("Continue to run original child process!\n");
      perror("execve");
      exit(EXIT_FAILURE);
    }

    else {
      printf("Process statrt to fork\n");
      printf("I'm the Parent Process, my pid = %d\n", getpid());
      waitpid(pid, &status, WUNTRACED);
      if (WIFEXITED(status)) {
        ParentMessage();
        printf("Normal termination with EXIT STATUS = %d\n",
               WEXITSTATUS(status));
      } else if (WIFCONTINUED(status)) {
        ParentMessage();
        printf("CHILD PROCESS CONTINUED\n");
      } else if (WIFSTOPPED(status)) {
        ParentMessage();
        printf("CHILD PROCESS STOPPED\n");
      } else if (WIFSIGNALED(status)) {
        ParentMessage();
        char *sig_message = Signal_info(WTERMSIG(status));
        printf("%s", sig_message);
      } else {
        printf("Execeptions\n");
      }
      exit(0);
    }
  }
}