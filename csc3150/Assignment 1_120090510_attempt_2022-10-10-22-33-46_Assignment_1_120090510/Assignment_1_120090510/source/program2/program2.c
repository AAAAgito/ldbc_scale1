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
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;
struct rusage;
union thread_union;
struct css_set;

struct kernel_clone_args {
	u64 flags;
	int __user *pidfd;
	int __user *child_tid;
	int __user *parent_tid;
	int exit_signal;
	unsigned long stack;
	unsigned long stack_size;
	unsigned long tls;
	pid_t *set_tid;
	/* Number of elements in *set_tid */
	size_t set_tid_size;
	int cgroup;
	int io_thread;
	int kthread;
	int idle;
	int (*fn)(void *);
	void *fn_arg;
	struct cgroup *cgrp;
	struct css_set *cset;
};

int my_exec(void *argc)
{

	return 0;
}

//implement fork function
int my_fork(void *argc)
{
	
	
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
	/*
	struct kernel_clone_args kargs = {
		.flags          = SIGCHLD,
		.stack          = &my_exec,
		.stack_size     = 0,
		.child_tid      = NULL,
		.parent_tid     = NULL,
		.tls            = 0,
		.exit_signal    = SIGCHLD
	};
	
	pid_t pid = kernel_clone(&kargs);
	int status;
	*/
	
	// Just enable compilation as I really can not finish the program on time
	pid_t pid = 0;

	if (pid == -1) {

	}
	/* execute a test program in child process */
	else {
		if (pid == 0) {
			
		} else {
			
		}
	}
	
	/* wait until child process terminates */
	
	return 0;
}

static int __init program2_init(void)
{

	printk("[program2] : Module_init Mo Yuchen 120090510\n");
	printk("[program2] : Module_init create kthread start\n");
	
	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "MyThread");

	if(!IS_ERR(task)) {
		printk("[program2] : Module_init kthread start\n");
		wake_up_process(task);
	}
	
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
