#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int i = 0;

  printf("--------USER PROGRAM--------\n");
  // alarm(2);
  raise(SIGBUS);
  // raise(SIGHUP);
  // raise(SIGINT);
  // raise(SIGQUIT);
  // raise(SIGILL);
  // raise(SIGTRAP);
  // raise(SIGABRT);
  // raise(SIGFPE);
  // raise(SIGKILL);
  // raise(SIGSEGV);
  // raise(SIGPIPE);
  // raise(SIGTERM);
  // raise(SIGSTOP);

  sleep(5);
  printf("user process success!!\n");
  printf("--------USER PROGRAM--------\n");
  return 100;
}
