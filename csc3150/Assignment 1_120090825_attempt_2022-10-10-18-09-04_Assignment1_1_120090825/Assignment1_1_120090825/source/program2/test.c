#include <unistd.h>
#include <stdio.h>
#include <signal.h>

int main(int argc,char* argv[]){
	int i=0;

	printf("--------USER PROGRAM--------\n");
//	alarm(2);
	raise(SIGTRAP);
	sleep(5);
	printf("user process success!!\n");
	printf("--------USER PROGRAM--------\n");
	return 100;
}

//use for normal check as program1.
/**
int main(int argc,char* argv[]){
	printf("------------CHILD PROCESS START------------\n");
	printf("This is the normal program\n\n");
	printf("------------CHILD PROCESS END------------\n");
	return 0;
}
*/