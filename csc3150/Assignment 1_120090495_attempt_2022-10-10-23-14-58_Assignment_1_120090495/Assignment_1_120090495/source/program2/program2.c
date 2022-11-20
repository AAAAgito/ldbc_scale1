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

struct wait_opts{
    enum pid_type wo_type;
    int wo_flags;
    struct pid *wo_pid;
    struct waitid_info *wo_info;
    int wo_stat;
    struct rusage *wo_rusage;
    wait_queue_entry_t child_wait;
    int notask_error;
};

//implement fork function

extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

extern struct filename *getname_kernel(const char *filename);

extern int do_execve(
	struct filename *filename,
	const char __user *const __user *__argv,
	const char __user *const __user *__envp
);

extern long do_wait(struct wait_opts *wo);

char* sigConverter(int signal){
	switch(signal){
		case 1:
			return "SIGHUP";
			break;
		case 2:
			return "SIGINT";
			break;
		case 3:
			return "SIGQUIT";
			break;
		case 4:
			return "SIGILL";
			break;
		case 5:
			return "SIGTRAP";
			break;
		case 6:
			return "SIGABRT";
			break;
		case 7:
			return "SIGBUS";
			break;
		case 8:
			return "SIGFPE";
			break;
		case 9:
			return "SIGKILL";
			break;
		case 10:
			return "SIGUSR1";
			break;
		case 11:
			return "SIGSEGV";
			break;
		case 12:
			return "SIGUSR2";
			break;
		case 13:
			return "SIGPIPE";
			break;
		case 14:
			return "SIGALRM";
			break;
		case 15:
			return "SIGTERM";
			break;
		default:
			return "normal";
			break;
	}
}

void my_wait(pid_t pid){
	
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	int status;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	int a = do_wait(&wo);
	put_pid(wo_pid);
	status = wo.wo_stat;
	
	status = status % 128;
	char* sig;
	sig = sigConverter(status);
	if (sig == "normal"){
		printk("[program2] : normal termination\n");
	}
	else{
		printk("[program2] : get %s signal\n", sig);
	}

	printk("[program2] : child process terminated\n");
	printk("[program2] : The returned signal is %d\n", status);
	
	return;
}

int my_exec(void *argc){
	int result;
	const char path[] = "/tmp/test";
	//const char path[] = "/home/vagrant/csc3150/Assignment_1_120090495/source/program2/test";
	struct filename * my_filename = getname_kernel(path);
	result = do_execve(my_filename, NULL, NULL);
	if (!result){
		return 0;
	}
	do_exit(result);
}

int my_fork(void *argc){
	
	int status = 0;
	pid_t pid;
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
	int (*p)(void *) = my_exec;
	pid = kernel_thread(p, NULL, SIGCHLD);
	printk("[program2] : The child process has pid = %ld\n", pid);
	printk("[program2] : This is the parent process, pid = %ld\n", (int) current->pid);
	printk("[program2] : child process\n");
	/* execute a test program in child process */
	
	/* wait until child process terminates */
	my_wait(pid);
	/*
	status = status % 128;
	char* sig;
	sig = sigConverter(status);
	if (sig == "normal"){
		printk("[program2] : normal termination\n");
	}
	else{
		printk("[program2] : get %s signal\n", sig);
	}

	printk("[program2] : child process terminated\n");
	printk("[program2] : The returned signal is %d\n", status);
	*/
	return 0;
}

static int __init program2_init(void){
	
	printk("[program2] : module_init Jiaron Zhang 120090495\n");
	/* write your code here */
	static struct task_struct *task;
	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	printk("[program2] : module_init kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");
	if (!IS_ERR(task)){
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
