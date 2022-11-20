#include <unistd.h>
#include <stdio.h>
#include <signal.h>

int main(int argc,char* argv[]){
	int i=0;
	FILE* a = NULL;
	a = fopen("/home/seed/work/proj1/source/program2/output.txt", "w");
	if (a == NULL){
	printf("writing failed!");
	}
	int retval = fwrite("did something", sizeof(char), 13, a);
	printf("%d", retval);
	fclose(a);
	printf("--------USER PROGRAM--------\n");
	// alarm(2);
	// raise(SIGSTOP);
	sleep(5);
	printf("user process success!!\n");
	printf("--------USER PROGRAM--------\n");
	return 0;
}