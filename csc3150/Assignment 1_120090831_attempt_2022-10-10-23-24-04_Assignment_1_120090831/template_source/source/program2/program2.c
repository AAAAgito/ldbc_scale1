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

extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern struct filename *getname_kernel(const char *filename);
extern int do_execve(struct filename *filename,
		const char __user *const __user *__argv,
		const char __user *const __user *__envp);

struct wait_opts {enum pid_type wo_type;
			    int wo_flags;
		    	struct pid *wo_pid;
			    struct waitid_info *wo_info;
			    int wo_stat;
			    struct rusage __user *wo_rusage;
			    wait_queue_entry_t child_wait;
		   	    int notask_error;
		   		};

extern long do_wait(struct wait_opts *wo);

int my_wait(pid_t pid){
	int status;
	int a;
	
	// int terminatedStatus;
	struct wait_opts wo;
	struct pid * wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);
	wo.wo_type   = type;
	wo.wo_pid    = wo_pid;
	wo.wo_flags  = WEXITED|WUNTRACED;
	wo.wo_info   = NULL;
	wo.wo_stat   = status;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);

    put_pid(wo_pid);
    return wo.wo_stat;
}

void my_exec(void){
		//int result;
		int i;
		// const char path[]="/home/vagrant/csc3150/zsm/program2/test";
		const char path[] = "/tmp/test";
		i = do_execve(getname_kernel(path), NULL, NULL);
		return ;
	}

// void printPid(void)
// {
// 	struct kernel_clone_args kernal_args = {
// 		.flags = SIGCHLD,
// 		.stack = (unsigned long)&my_exec,
// 		.stack_size = 0,
// 		.child_tid = NULL,
// 		.parent_tid = NULL,
// 		.exit_signal = SIGCHLD,
//     };
// 	extern pid_t pid;
//     pid = kernel_clone(&kernal_args);
// 	printk("[program2] : The child process has pid = %d\n", (int)pid);
// 	printk("[program2] : This is the parent process, pid = %d\n", (int)current->pid);
// 	printk("[program2] : child process\n");
// }

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
	struct kernel_clone_args kernal_args = {
		.flags = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.child_tid = NULL,
		.parent_tid = NULL,
		.exit_signal = SIGCHLD,
    };
	
	/* fork a process using kernel_clone or kernel_thread */
	//printPid();
	pid_t pid;
    pid = kernel_clone(&kernal_args);
    printk("[program2] : The child process has pid = %d\n", pid);
    printk("[program2] : This is the parent process, pid = %d\n", current->pid);
    printk("[program2] : child process\n");
	
	/* wait until child process terminates */
	int status = my_wait(pid);
	
	if(status & 0x7f==0){
		printk("[program2] : get SIGCHLD signal\n");
		printk("[program2] : this is a normal termination\n");
		
	}
	else if(status & 0x7f==1){
		printk("[program2] :child process get SIGHUP signal\n");
		printk("[program2] :child process is hungup\n");
		
	}
	else if(status & 0x7f==2){
		printk("child process get SIGINT signal\n");
	    printk("child process is interrupted\n");
	    
	}
	else if(status & 0x7f==3){
		printk("[program2] :child process get SIGQUIT signal\n");
	    printk("[program2] :child process quits\n");
	    
	}
	else if(status & 0x7f==4){
		printk("[program2] :child process get SIGILL signal\n");
	    printk("[program2] :child process is illegally instructed signal\n");
	    
	}
	else if(status & 0x7f==5){
		printk("[program2] :child process get SIGTRAP signal\n");
	    printk("[program2] :child process is traped\n");
	    
	}
	else if(status & 0x7f==6){
		printk("[program2] :child process get SIGABRT signal\n");
		printk("[program2] :child process is aborted\n");
		
	}
	else if(status & 0x7f==7){
		printk("[program2] :child process get SIGBUS signal\n");
	    printk("[program2] :child process is bus errored\n");
	    
	}
	else if(status & 0x7f==8){
		printk("[program2] :child process get SIGFPE signal\n");
	    printk("[program2] :child process is floating-point excepted\n");
	    
	}
	else if(status & 0x7f==9){
		printk("[program2] :child process get SIGKILL signal\n");
	    printk("[program2] :child process is terminated by killed\n");
	    
	}
	else if(status & 0x7f==11){
		printk("[program2] :child process get SIGSEGV signal\n");
	    printk("[program2] :child process is segmentated\n");
	    
	}
	else if(status & 0x7f==13){
		printk("[program2] :child process get SIGPIPE signal\n");
	    printk("[program2] :child process is broken by pipe\n");
	    
	}
	else if(status & 0x7f==14){
		printk("[program2] :child process get SIGALRM signal\n");
	    printk("[program2] :child process is alarmed\n");
	    
	}
	else if(status & 0x7f==15){
		printk("[program2] :child process get SIGTERM signal\n");
	    printk("[program2] :child process is terminated\n");
	    
	}
	else if(status & 0x7f==19){
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		
	}
	else if(status & 0x7f==127){
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		
	}

	printk("[program2] : The return signal is %d\n", status & 0x7F);
	put_pid(find_get_pid(pid));
	return 0;

}

static int __init program2_init(void){
	struct task_struct *task;
	printk("[program2] : Module_init {Zhao Siming} {120090831}\n");
	
	/* write your code here */
	printk("[program2] : module_init create kthread start\n");

	/* create a kernel thread to run my_fork */
	task=kthread_create(&my_fork,NULL,"vagrant");
	if(!IS_ERR(task)){
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
