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
struct wait_opts{
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct waitid_info *wo_info;
	int wo_stat;
	struct rusage *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error ;
};
/* function prototypes */
extern long do_wait (struct wait_opts *wo);

extern int do_execve ( 
struct filename *filename,
const char __user *const __user *__argv,
const char __user *const __user *__envp);

extern struct filename *getname_kernel(const char __user *filename);


int my_exec(void){
	int execu;
	const char path [] = " /tmp/test";
	struct filename * my_filename = getname_kernel(path);
	printk("[program2] : child process\n");	
	execu = do_execve(my_filename,NULL,NULL);
	//if exec success
	if (!execu){
		return 0;
	}
	//if exec failed 
	else{
		printk("doexec fail!!!!");
		printk("%d", execu);
		do_exit(execu);
	}
}


void my_wait(pid_t pid){
	int status;
	int sig_value;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);
	wo.wo_type = type;
    wo.wo_pid = wo_pid;
    wo.wo_flags = WUNTRACED | WEXITED;
    wo.wo_info = NULL;
    wo.wo_stat = status;
    wo.wo_rusage = NULL;
	do_wait(&wo);

	sig_value = wo.wo_stat & 0x7f;

	switch(sig_value){
		case 0:
			printk("[program2] : normal exit");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;
		
		case 1:
			printk("[program2] : get SIGHUP signal\n");
			printk("[program2] : child process hung up\n");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;

		case 2:
			printk("[program2] : get SIGINT signal\n");
			printk("[program2] : child process interrupted\n");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;

		case 3:
			printk("[program2] : get SIGQUIT signal\n");
			printk("[program2] : child process quits\n");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;

		case 4:
			printk("[program2] : get SIGILL signal\n");
			printk("[program2] : child process has illegal instruction\n");
			printk("[program2] : The return signal is %d\n",sig_value);	
			break;

		case 5:
			printk("[program2] : get SIGTRAP signal\n");
			printk("[program2] : child process trapped\n");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;

		case 6:
			printk("[program2] : get SIGABRT signal\n");
			printk("[program2] : child process abort\n");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;

		case 7:
			printk("[program2] : get SIGBUS signal\n");
			printk("[program2] : child process has bus error\n");
			printk("[program2] : The return signal is %d\n",sig_value);	
			break;

		case 8:
			printk("[program2] : get SIGFPE signal\n");
			printk("[program2] : child process has arithmetic excption\n");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;

		case 9:
			printk("[program2] : get SIGKILL signal\n");
			printk("[program2] : child process killed\n");
			printk("[program2] : The return signal is %d\n",sig_value);	
			break;

		//case 10:

		case 11:
			printk("[program2] : get SIGSEGV signal\n");
			printk("[program2] : child process has segment fault\n");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;

		//case 12:

		case 13:
			printk("[program2] : get SIGPIPE signal\n");
			printk("[program2] : child process has broken pipe\n");
			printk("[program2] : The return signal is %d\n",sig_value);	
			break;

		case 14:
			printk("[program2] : get SIGALRM signal\n");
			printk("[program2] : child process alarmed\n");
			printk("[program2] : The return signal is %d\n",sig_value);
			break;

		case 15:
			printk("[program2] : get SIGTERM signal\n");
			printk("[program2] : child process terminated\n");
			printk("[program2] : The return signal is %d\n",sig_value);	
			break;

		case 127:
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is 19");
		break;
	}

	put_pid(wo_pid);	
	return;
}

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
	struct kernel_clone_args kargs = {
	.flags = SIGCHLD,
	.stack = (unsigned long)&my_exec,
	.stack_size = 0,
	.parent_tid = NULL,
	.child_tid = NULL,
	.tls = 0,
	.exit_signal = SIGCHLD,  
};
	/* fork a process using kernel_clone or kernel_thread */
	//pid = kernel_thread(&my_exec, NULL, SIGCHLD);
	pid = kernel_clone(&kargs);
	if (pid > 0){
		printk("[program2] : The child process has pid = %d\n",pid);
		printk("[program2] : This is the parent process,pid = %d\n",current->pid);
	}
	if (pid < 0){
		printk("[program2] : err!");
	}
	/* execute a test program in child process */	
	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}
static struct task_struct *task;
static int __init program2_init(void){

	printk("[program2] : module_init {JiyuanLong} {120090850}\n");
	
	/* write your code here */	
	//int err;
	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(my_fork, NULL, "MyThread");
	if (!IS_ERR(task)){
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	else{
		printk("[program2] : creat thread failed\n");
	}
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
