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
#include<linux/signal.h>

MODULE_LICENSE("GPL");

struct wait_opts {
	enum  pid_type	wo_type;
	int	 wo_flags;
	struct	pid	*wo_pid;
	struct  waitid_info	*wo_info;
	int	 wo_stat;
	struct  rusage	*wo_rusage;
	wait_queue_entry_t	child_wait;
	int	 notask_error;
};

extern long do_wait(struct wait_opts *);
extern pid_t kernel_clone(struct kernel_clone_args *);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern int do_execve(struct filename *filename,
	const char __user *const __user *__argv,
	const char __user *const __user *__envp);
extern struct filename* getname_kernel(const char *);
int my_exec(void);

//wait part
void my_wait(pid_t pid){
	int a;
	int status;
	struct wait_opts wo;
	struct pid *wo_pid=NULL;
	enum pid_type type;
	type=PIDTYPE_PID;
	wo_pid=find_get_pid(pid);
	status = 0;

	wo.wo_type=type;
	wo.wo_pid=wo_pid;
	wo.wo_flags= WEXITED | WUNTRACED;
	wo.wo_info=NULL;
	wo.wo_stat= (int __user) status;
	wo.wo_rusage = NULL;

	a=do_wait(&wo);
	printk("[program2] : The return signal is %d\n", wo.wo_stat);
	put_pid(wo_pid);
	return;
}

//exe
int my_exec(void){
	const char path[]="/tmp/test";
	// const char *const argv[] = {path,NULL,NULL};
	struct filename* filename = getname_kernel(path);
	int a =do_execve(filename,NULL,NULL);
	if(a==0){
		return 1;
	}
	printk("ex error\n");
	return 0;
}

//implement fork function
int my_fork(void *argc){

	int i;
	pid_t pid;
	pid_t ppid;

	struct kernel_clone_args kca = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack =(unsigned long)&my_exec,
		.stack_size = 0,
		.tls=0,
		.exit_signal = SIGCHLD};
	// struct kernel_clone_args kca;
	// kca.flags = SIGCHLD;
	// kca.child_tid = NULL;
	// kca.parent_tid = NULL;	
	// kca.stack =(unsigned long)&my_exec;
	// kca.stack_size = 0;
	// kca.tls=0;
	// kca.exit_signal = SIGCHLD;
	// set default sigaction for current process

	struct k_sigaction *k_action = &current->sighand->action[0];
	for(i=0;i<_NSIG;i++){
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	
	ppid = current->pid;
	/* fork a process using kernel_clone or kernel_thread */
	printk("[program2] : module_init create kthread start\n");
	pid=kernel_clone(&kca);
	printk("[program2] : This is the child process the pid is %d\n",pid);
	printk("[program2] : This is the parent process, pid = %d\n",ppid);
	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}


static int __init program2_init(void){
	void* a;
	printk("[program2] : Module_init {Zhuanghengmeng} {120090648}\n");
	/* write your code here */
	/* create a kernel thread to run my_fork */
	my_fork(a);
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
