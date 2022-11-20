#include <linux/module.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/jiffies.h>
#include <linux/kmod.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");


//implement fork function
int my_fork(void *argc){
	
	
	//set default sigaction for current process
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for(i=0;i<_NSIG;i++){
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	
	pid_t pid;
	int status;
	int state;

	printf("Process start to fork\n");
	
	//pid_t pid = fork();
	pid=fork();
	// fflush(stdout);

	if(pid==-1){
		printf("Fork error:\n");
		perror("fork");
		exit(1);
	}
	else{
		//child
        if(pid==0){

			int i;
			char *arg[argc];

			sleep(10);

			printf("I'm the Child Process, my pid = %d\n" ,getpid());
			
			/* execute test program */ 

			for(i=0; i<argc-1; i++){
				arg[i]=argv[i+1];
			}
			arg[argc-1]=NULL;

			printf("Child Process start to execute test program:\n");

			execve(arg[0],arg,NULL);

			perror("execve");
			exit(EXIT_FAILURE);
		}
		//parent
		else{

			printf("I'm the Parent Process, my pid = %d\n" ,getpid());

			/* wait for child process terminates */
			
			waitpid(pid, &status, WUNTRACED);

			printf("Parent process receives the signal\n");

			/* check child process'  termination status */

			// if(WIFEXITED(status)){
			// 	printf("Normal termination with EXIT STATUS = %d\n" ,WEXITSTATUS(status));
			// }
			// else if(WIFSIGNALED(status)){
			// 	printf("CHILD EXECUTION FAILED: %d\n" ,WTERMSIG(status));
			// }
			// else if(WIFSTOPPED(status)){
			// 	printf("CHILD PROCESS STOPPED: %d\n" ,WSTOPSIG(status));
			// }
			// else{
			// 	printf("CHILD PROCESS CONTINUED\n");
			// }
			exit(1);
		}
	}

	/* fork a process using kernel_clone or kernel_thread */
	
	/* execute a test program in child process */
	
	/* wait until child process terminates */
	
	return 0;
}

static int __init program2_init(void){

	printk("[program2] : Module_init\n");
	
	/* write your code here */
	
	/* create a kernel thread to run my_fork */
	
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
