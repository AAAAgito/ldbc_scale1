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

struct wait_opts {
	enum pid_type wo_type;
	int	wo_flags;
	struct pid *wo_pid;
	struct siginfo *wo_info;
	int	wo_stat;
	struct rusage *wo_rusage;
	wait_queue_entry_t child_wait;
	int	notask_error;
};

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern struct filename *getname_kernel(const char * filename);
extern int do_execve(struct filename *filename,const char __user *const __user *__argv,const char __user *const __user *__envp);
static struct task_struct *task;
extern long do_wait(struct wait_opts *wo);
extern signed char my_WIFSIGNALED(int status);
// static char *signals[];

// int my_exec();
// int my_wait();

// struct kernel_clone_args{
// 	u64 flag;
// 	int __user *parent_tid;
// 	int __user *childtid;
// 	int __user *pidfd;
// 	int exit_signal;
// 	unsigned long stack;
// 	unsigned long stack_size;
// 	unsogned long tls;
// 	pid_t *set_tid;
// 	size_t set_tid_size;
// 	int cgroup;
// 	struct cgroup *cgrp;
// 	struct css_set *cset;
// }




int my_exec(void){
	int c = 0;
	if(c==0);
	int output;
	char TestPath[] = "/tmp/test";
	struct filename *FileName = getname_kernel(TestPath);
	int a = 1;
	if(a==0);
	output = do_execve(FileName, NULL, NULL);
	int b = 2;
	if(b==0);
	if(output){
		do_exit(output);
	}
	else{
		return 0;
	}
}

struct kernel_clone_args c_args =
{
	.flags = SIGCHLD,
	.child_tid = NULL,
	.parent_tid = NULL,
	.stack = (unsigned long)&my_exec,
	.stack_size = 0,
	.tls = 0,
	.exit_signal = SIGCHLD,
};
void my_wait(pid_t pid, int status){
	int wat;
	struct wait_opts wo;
	struct pid * wo_pid = NULL;
	enum pid_type type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED|WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_rusage = NULL;
	// wo.wo_stat = status;

	wat = do_wait(&wo);
	if (wo.wo_stat == 0){
		printk("[program2] : This is a normal child process\n");
	}
	else if (wo.wo_stat == 1){
		printk("[program2] : get SIGHUP signal\n");
	}
	else if (wo.wo_stat == 2){
		printk("[program2] : get SIGINT signal\n");
	}
	else if (wo.wo_stat == 9){
		printk("[program2] : get SIGKILL signal\n");
	}
	else if (wo.wo_stat == 13){
		printk("[program2] : get SIGPIPE signal\n");
	}
	else if ( wo.wo_stat == 14){
		printk("[program2] : get SIGALRM signal\n");
	}
	else if (wo.wo_stat == 15){
		printk("[program2] : get SIGTERM signal\n");
	}
	else if (wo.wo_stat == 131){
		printk("[program2] : get SIGQUIT signal\n");
	}
	else if (wo.wo_stat == 132){
		printk("[program2] : get SIGILL signal\n");
	}
	else if (wo.wo_stat == 133){
		printk("[program2] : get SIGTRAP signal\n");
	}
	else if (wo.wo_stat == 134){
		printk("[program2] : get SIGABRT signal\n");
	}
	else if (wo.wo_stat == 135){
		printk("[program2] : get SIGBUS signal\n");
	}
	else if (wo.wo_stat == 136){
		printk("[program2] : get SIGFPE signal\n");
	}
	else if (wo.wo_stat == 139){
		printk("[program2] : get SIGSEGV signal\n");
	}
	else if (wo.wo_stat == 4991){
		printk("[program2] : get SIGSTOP signal\n");
	}
	else{
		printk("[program2] : something other wrong\n");
	}
	printk("[program2] : child process terminates\n");
	printk("[program2] : The return signal is %d\n", wo.wo_stat );
	put_pid(wo_pid);
	return;
}

//implement fork function
int my_fork(void *argc){
	
	
	//set default sigaction for current process
	int i;
	pid_t pid;
	int status;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for(i=0;i<_NSIG;i++){
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	
	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_clone(&c_args);
	int ppid = (int)current->pid;
	if(pid==-1){
		//perror("kernal_clone");
		//exit(1);
	}
	/* execute a test program in child process */
	else{
		if(pid==0){
			printk("[program2] : This is the parent process, pid =  %d\n",pid);
			my_exec();
		}
	
	/* wait until child process terminates */
		else{
			printk("[program2] : This is the parent process, pid =  %d\n", ppid);
			my_wait(pid,status);
		}

	}
	return 0;
}

static int __init program2_init(void){

	printk("[program2] : Module_init Deng Junyuan 120090698\n");
	
	/* write your code here */
	printk("[program2] : Module_init create kthread start\n");
	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork,NULL,"MyThread");
	if (!IS_ERR(task)){
		printk("[program2] : Module_init kthread start\n");
		wake_up_process(task);
	
}
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}








module_init(program2_init);
module_exit(program2_exit);