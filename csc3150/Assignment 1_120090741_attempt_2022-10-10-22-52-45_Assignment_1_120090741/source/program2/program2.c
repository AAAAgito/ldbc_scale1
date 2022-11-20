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
static struct wait_opts *wo;
static struct kernel_clone_args *args;
pid_t pid;
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern long do_wait(struct wait_opts *wo);


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


	pid = kernel_clone(args);
	if(pid == -1){
		exit(1);
	}
	else{
		if(pid == 0){
			int i;
			char *arg[argc];
			child_pid = getpid();
			for(i=0; i<argc-1; i++){
				arg[i] = argv[i+1];
			}
			arg[argc-1] = NULL;
			printk("child process\n");
			execve(arg[0], arg, NULL);
			printf("???\n");
		}
		else{}
	
	/* fork a process using kernel_clone or kernel_thread */
	
	/* execute a test program in child process */
	
	/* wait until child process terminates */
	
	return 0;
}

static int __init program2_init(void){

	printk("[program2] : module_init{Li Chengxi}{120090741}\n");
	printk("[program2] : module_init create kthread start\n");
	
	task = kthread_create(&my_fork, NULL, "MyThread");

	if(!IS_ERR(task)){
		printk("kthread starts\n");
		wake_up_process(task);
	}
	
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
	kthread_stop(task);
}

module_init(program2_init);
module_exit(program2_exit);
