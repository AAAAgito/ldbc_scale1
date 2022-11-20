#include <unistd.h>
#include <stdio.h>
#include <signal.h>

int main(int argc,char* argv[]){

	printf("--------USER PROGRAM--------\n");
	raise(SIGTRAP);
	printf("--------USER PROGRAM--------\n");
	return 0;
}
