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
	int wo_flags;
	struct pid *wo_pid;

	struct waitid_info *wo_info;
	int wo_stat;
	struct rusage *wo_rusage;

	wait_queue_entry_t child_wait;
	int notask_error;
};

extern pid_t kernel_clone(struct kernel_clone_args *args);

extern long do_wait(struct wait_opts *wo);

extern int do_execve(struct filename *filename,
	const char __user *const __user *__argv,
	const char __user *const __user *__envp);

extern struct filename *getname_kernel(const char * filename);
static struct task_struct *task;	

void my_wait(pid_t pid, int* status)
{	long aa;
	struct pid *wo_pid=NULL;
	struct wait_opts wo;
		wo.wo_type=PIDTYPE_PID;
		wo.wo_flags = WEXITED | WUNTRACED;
		wo.wo_pid = find_get_pid(pid);
		wo.wo_info=NULL;
		wo.wo_stat = 0;
		wo.wo_rusage=NULL;
	wo_pid=find_get_pid(pid);

	aa=do_wait(&wo);
	put_pid(wo_pid);

}
void my_exec(void)
{
	int result;
	const char path[]= "/home/vagrant/csc3150/project1/program2/test";
	const char *const argv[]={path,NULL,NULL};
	const char *const envp[]={"HOME=/","PATH=/sbin:/user/sbin:/bin:/usr/bin",NULL};
	struct filename * my_filename;
	printk("[program2] : Child process pid = %d\n",(int)current->pid);
	printk("[program2] : Child process\n");
	my_filename=getname_kernel(path);
	result=do_execve(my_filename,argv,envp);

	if(!result)
		return;
	do_exit(result);
}
int my_fork(void *argc){
	int pid;
	int status=0;
	int i;
	char* sigName;
	struct kernel_clone_args kargs={
		.flags=SIGCHLD,
		.pidfd=NULL,
		.child_tid=NULL,
		.parent_tid=NULL,
		.exit_signal=SIGCHLD & CSIGNAL,
		.stack=(unsigned long)&my_exec,
		.stack_size=0,
		.tls=0
	};
	
	struct k_sigaction *k_action = &current->sighand->action[0];
	for(i=0;i<_NSIG;i++){
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	printk("[program2] : Parent process pid = %d\n",(int)current->pid);

	/* fork a process using kernel_clone or kernel_thread */
	pid=kernel_clone(&kargs);
	if(pid==0) // child process
	{
		printk("[program2] : exec error");
		do_exit(0);
	}
	/* execute a test program in child process */
	
	/* wait until child process terminates */
	my_wait(pid,&status);
	switch (status)
	{
	case 1:
		sigName ="SIGHUP";
	case 2:
		sigName = "SIGINT";
	case 131:
		sigName = "SIGQUIT";
	case 132:
		sigName = "SIGILL";
	case 133:
		sigName = "SIGTRAP";
	case 134:
		sigName = "SIGABRT";
	case 135:
		sigName = "SIGBUS";
	case 136:
		sigName = "SIGFPE";
	case 9:
		sigName = "SIGKILL";
	case 139:
		sigName = "SIGSEGV";
	case 13:
		sigName = "SIGPIPE";
	case 14:
		sigName = "SIGALRM";
	case 15:
		sigName = "SIGTERM";
	case 4991:
		sigName = "SIGSTOP";
	default:
		sigName = "normal";
	}
	if(sigName[0]!='n'){
		printk("[program2] : get %s signal\n",sigName);
	}else{
		printk("[program2] : normal termination\n");
	}
	printk("[program2] : child process terminated");
	printk("[program2] : The return signal is %d",status);
	do_exit(0);
	return 0;
}

static int __init program2_init(void){
	printk("[program2] : Module_init{Nuoan Zhang} {120090851}\n");
	
	/* write your code here */
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "my_fork_thread");
	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	/* create a kernel thread to run my_fork */
	
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
