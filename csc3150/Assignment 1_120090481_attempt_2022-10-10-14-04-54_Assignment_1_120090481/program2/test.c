#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc,char* argv[]){
	int i=0;

	printf("--------USER PROGRAM--------\n");
	// alarm(2);		2
	raise(SIGBUS);	
	// abort();			3
	// raise(SIGFPE);	
	// raise(SIGHUP);	5
	// raise(SIGILL);	6
	// raise(SIGINT);	7
	// raise(SIGKILL);	8
	// noral			9
	// raise(SIGPIPE);	10
	// raise(SIGQUIT);	11
	// raise(SIGSEGV);	12
	// raise(SIGSTOP);	13
	// raise(SIGTERM);	14
	// raise(SIGTRAP);	15
	sleep(5);
	printf("user process success!!\n");
	printf("--------USER PROGRAM--------\n");
	return 100;
}
