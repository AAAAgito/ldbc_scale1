#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	printf("------------CHILD PROCESS START------------\n");
	printf("This is the SIGKILL program\n\n");
	raise(SIGKILL);
	sleep(5);
	printf("------------CHILD PROCESS END------------\n");

	return 0;
}
