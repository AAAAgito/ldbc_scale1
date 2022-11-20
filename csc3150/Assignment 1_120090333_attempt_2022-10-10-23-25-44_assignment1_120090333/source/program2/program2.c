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


static struct task_struct *my_task;
//output symbol
extern struct filename *getname_kernal(const char*filename);
extern int do_execve(struct filename *filename,
    const char __user *const __usser *__argv,
	const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern pid_t kernal_clone(struct kernal_clone_args *arg);

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
    /*using kernal_clone to accomplish the fork function*/
	struct kernal_clone_args args={
		.flags=SIGCHLD,
		.exit_signal=SIGCHLD,
		.stack=&my_child_stack,
		.stack_size=0,
		,child_tid=NULL,
		.parent_tid=NULL,

	};



}
static int __init program2_init(void){
		printk("[program2] : Module_init XiangyuLiu 120090333 \n");
    /*create a kernal thread*/
	printk("[program2]: Module_init kthread start \n");
	my_task = kthread_create(&my_fork, NULL, "MyThread");
	if(!IS_ERR(my_task)){
		printk("[program2] : Module_init create kthread start \n");
		wake_up_process(my_task);

	}
	return 0;
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
