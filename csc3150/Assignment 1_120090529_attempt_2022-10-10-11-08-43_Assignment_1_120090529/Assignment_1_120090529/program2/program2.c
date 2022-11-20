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
	
	struct kernel_clone_args *args;
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
	args->flags=SIGCHILD;
	args->child_tid = NULL;
	args->exit_signal=SIGCHILD;
	args->stack=(unsigned long)&my_exec;
	args->stack_size=0;
	args->tls=0;
	extern pid_t kernel_clone(&args);

	/* execute a test program in child process */
	char path="/home/vagrant/source/program2/test";
	struct filename *my_file = getname_kernel(path);
	int my_exec=do_execve(*myfile,NULL,NULL);

	/* wait until child process terminates */
	
	return 0;
}


static int __init program2_init(void){

	printk("[program2] : Module_init\n");
	task=kthread_create(&my_fork,NULL,"MyThread");


	if (!IS_ERR(task)){
		printk("Kthread starts\n");
		wake_up_process(task);
	}
	
	/* create a kernel thread to run my_fork */
	
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);

EXPORT_SYMBOL(do_execve);
EXPORT_SYMBOL(kernel_clone);
EXPORT_SYMBOL(kernel_thread);
EXPORT_SYMBOL(do_wait);
EXPORT_SYMBOL(getname_kernel);
