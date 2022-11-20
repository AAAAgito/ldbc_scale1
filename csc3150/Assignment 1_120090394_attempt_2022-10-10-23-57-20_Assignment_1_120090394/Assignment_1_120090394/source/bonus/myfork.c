#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

int forkmanytime(int time, int max, char *arg[],int second);
void TerMessage(int sig, pid_t pid);

int main(int argc,char *argv[]){
	int status;
    pid_t pid;
	char *arg[argc];
	
	for(int i=0;i<argc-1;i++){
		arg[i]=argv[i+1];
	}
	forkmanytime(1,argc-1,argv,0);
    return 0;
}


int forkmanytime(int time, int max, char *arg[],int second){
	
	int status;
	if(time>max){return 0;}

	if(time<=max){
		pid_t pid=fork();
		if(pid==-1){
			perror("fork");
			exit(1);
		}
		else{
			if(pid==0){
				forkmanytime(time+1,max, arg,1);
				execve(arg[time],arg,NULL);
				exit(0);
			}
			else{
				waitpid(pid, &status, WUNTRACED);
				if(WIFEXITED(status)){
					printf("   Process %d terminated now!\n",getpid()+1);
					printf("   Child process %d of parent process %d terminated normally with exit code %d (Normal)\n ",getpid()+1,getpid(),WEXITSTATUS(status));
				}
				if(WIFSIGNALED(status)){
					TerMessage(WTERMSIG(status),getpid());
				}
				if(WIFSTOPPED(status)){}
				if(second == 1){
					execve(arg[time-1],arg,NULL);
				}

				printf("-------------------------------------------\n");
				int now = getpid();
				printf("   Process Tree: %d",now);
				for(int i = 0;i<max;i++){
					printf("->%d",++now);
				}
				printf("!\n");
				printf("   My process (%d) terminated normally\n",getpid());
				
				exit(0);
			}
		}
	}
	return 0;
	
}

void TerMessage(int sig,pid_t pid){
	printf("   Process %d terminated now!\n",pid+1);
	if(sig==1){
		printf("   Child process %d of parent process %d is terminated by signal %d (Hangup)\n",pid+1,pid,sig);
	}
	if(sig==2){
		printf("   Child process %d of parent process %d is terminated by signal %d (Interrupt)\n",pid+1,pid,sig);
	}
	if(sig==3){
		printf("   Child process %d of parent process %d is terminated by signal %d (Quit)\n",pid+1,pid,sig);
	}
	if(sig==4){
		printf("   Child process %d of parent process %d is terminated by signal %d (Illegal_instr)\n",pid+1,pid,sig);
	}
	if(sig==5){
		printf("   Child process %d of parent process %d is terminated by signal %d (Trap)\n",pid+1,pid,sig);
	}
	if(sig==6){
		printf("   Child process %d of parent process %d is terminated by signal %d (Abort)\n",pid+1,pid,sig);
	}
	if(sig==7){
		printf("   Child process %d of parent process %d is terminated by signal %d (Bus)\n",pid+1,pid,sig);
	}
	if(sig==8){
		printf("   Child process %d of parent process %d is terminated by signal %d (Floating)\n",pid+1,pid,sig);
	}
	if(sig==9){
		printf("   Child process %d of parent process %d is terminated by signal %d (Kill)\n",pid+1,pid,sig);
	}
	if(sig==11){
		printf("   Child process %d of parent process %d is terminated by signal %d (Segment_fault)\n",pid+1,pid,sig);
	}
	if(sig==13){
		printf("   Child process %d of parent process %d is terminated by signal %d (Pipe)\n",pid+1,pid,sig);
	}
	if(sig==14){
		printf("   Child process %d of parent process %d is terminated by signal %d (Alarm)\n",pid+1,pid,sig);
	}
	if(sig==15){
		printf("   Child process %d of parent process %d is terminated by signal %d (Terminate)\n",pid+1,pid,sig);
	}
}
