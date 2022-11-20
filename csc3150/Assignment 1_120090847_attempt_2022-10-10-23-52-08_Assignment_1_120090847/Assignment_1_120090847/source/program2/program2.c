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
static struct task_struct *task;
static int my_exec;
struct kernel_clone_args duplicate={
	.flags=SIGCHLD,
	.exit_signal=SIGCHLD,
	.stack=&my_exec,
	.stack_size=0
	/*
	u64 flags;
	int __user *pidfd;
	int __user *child_tid;
	int __user *parent_tid;
	int exit_signal;
	unsigned long stack;
	unsigned long stack_size;
	unsigned long tls;
	pid_t *set_tid;
	size_t set_tid_size;
	int cgroup;
	struct cgroup *cgrp;
	struct css_set *cset;*/
};
//extern struct filename *getname_kernel(const char *);

extern pid_t kernel_clone(struct kernel_clone_args *kargs);
/*
extern int do_execve( struct filename *filename,
					const char __user *const__user *__argv,
					const char __user *const__user *__envp);*/
extern int do_execve;
struct wait_opts {
	enum pid_type		wo_type;
	int			wo_flags;
	struct pid		*wo_pid;

	struct waitid_info	*wo_info;
	int			wo_stat;
	struct rusage		*wo_rusage;

	wait_queue_entry_t		child_wait;
	int			notask_error;
};

extern long do_wait( struct wait_opts *wo);
//implement my_exec funtion
static int my_exec={
	int result;
	
	const char path [] = "/home/vagrant/test";
	const char *const argv[] = {path,NULL,NULL};
	const char *const envp[] = {"HOME=/","PATH=/sbin:/user/sbin:/bin:/usr/bin",NULL};

	struct filename * my_filename = getname_kernel(path);

	printk("[program2] : child process\n");
	
	result = do_execve(my_filename,argv,envp);

	//if exec success
	if (!result){
		return 0;
	};

	//if exec failed 
	do_exit(result);
}


	


//implement wait function
int my_wait(pid_t pid){

	int status;
	struct wait_opts wo;
	struct pid *wo_pid=NULL;
	enum pid_type type;
	type=PIDTYPE_PID;
	wo_pid=find_get_pid(pid);

	wo.wo_type=type;
	wo.wo_pid=wo_pid;
    wo.wo_flags=WEXITED | WUNTRACED;
	wo.wo_info=NULL;
	wo.wo_stat=(int __user*)&status;
	wo.wo_rusage=NULL;

	pid = kernel_clone(&create);
	do_wait(&wo);
	if (WIFEXITED(wo.wo_stat)){
		printk("[program2] : normal exit with status = %d\n",WEXITSTATUS(status));
	}

	else if (WIFSIGNALED(wo.wo_stat)){
		
		int sig_value = WTERMSIG(*wo.wo_stat);
		
		if (sig_value == 6){
			printk("[program2] : get SIGABRT signal\n");
			printk("[program2] : child process is abort by abort signal\n");
		}
		else if (sig_value == 14){
			printk("[program2] : get SIGALRM signal\n");
			printk("[program2] : child process is alarmed]\n");
		}

		// why is 7 not 10???
		else if (sig_value == 7){
			printk("[program2] : get SIGBUS signal\n");
			printk("[program2] : child process has bus error\n");
		}
		else if (sig_value == 8){
			printk("[program2] : get SIGFPE signal\n");
			printk("[program2] : child process encounter arithmetic excption\n");
		}
		else if (sig_value == 1){
			printk("[program2] : get SIGUP signal\n");
			printk("[program2] : child process is hung up by hang up signal\n");
		}
		else if (sig_value == 4){
			printk("[program2] : get SIGILL signal\n");
			printk("[program2] : child process has illegal instruction\n");
		}
		else if (sig_value == 2){
			printk("[program2] : get SIGINT signal\n");
			printk("[program2] : child process is interrupted by interrupt signal\n");
		}
		else if (sig_value == 9){
			printk("[program2] : get SIGKILL signal\n");
			printk("[program2] : child process is killed by kill signal\n");
		}
		else if (sig_value == 13){
			printk("[program2] : get SIGPIPE signal\n");
			printk("[program2] : child process encounter broken pipe\n");
		}
		else if (sig_value == 3){
			printk("[program2] : get SIGQUIT signal\n");
			printk("[program2] : child process quits because of quit signal\n");
		}
		else if (sig_value == 11){
			printk("[program2] : get SIGSEGV signal\n");
			printk("[program2] : child process encounter segment fault\n");
		}
		else if (sig_value == 15){
			printk("[program2] : get SIGTERM signal\n");
			printk("[program2] : child process is terminated by terminate signal\n");
		}
		else if (sig_value == 5){
			printk("[program2] : get SIGTRAP signal\n");
			printk("[program2] : child process is trapped by trap signal\n");
		}
		printk("[program2] : The return signal is %d\n",sig_value);
	}
	else if ( WIFSTOPPED(wo.wo_stat)){
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is %d\n",WSTOPSIG(wo.wo_stat));
	}


	put_pid(wo_pid);

	
	return;
};

//implement fork function
int my_fork(void *duplicate){
	//pid=kernel_clone(&duplicate);
	
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
	
	/* fork a process using kernel_clone or kernel_thread */
	
	/* execute a test program in child process */
	//pid=kernel_clone()
	/* wait until child process terminates */
	
	return 0;
};

static int __init program2_init(void){

	printk("[program2] : Module_init\n");
	
	/* write your code here */
	
	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	printk("[program2] : module_init kthread start\n");
	task = kthread_create(&my_fork, NULL, "Mythread");

	if(!IS_ERR(task)){
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	
	return 0;
};

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
};

module_init(program2_init);
module_exit(program2_exit);
