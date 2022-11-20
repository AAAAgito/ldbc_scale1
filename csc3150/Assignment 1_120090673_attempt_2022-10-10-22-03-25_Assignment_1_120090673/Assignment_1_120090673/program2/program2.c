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

//extern struct kernel_clone_args;
/*
struct kernel_clone_args{
	u64 flags;
	int __user *parent_tid;
	int __user *child_tid;
	unsigned long stack;
	unsigned long stack_size;
	unsigned long tls;
};
*/
struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct siginfo __user *wo_info;
	int __user *wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);

static struct task_struct *process;

void print_signal(int arg, int ret)
{
	if (arg == 135) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 7\n");
	} else if (arg == 131) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 3\n");
	} else if (arg == 132) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 4\n");
	} else if (arg == 133) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 5\n");
	} else if (arg == 134) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 6\n");
	} else if (arg == 136) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 8\n");
	} else if (arg == 139) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 11\n");
	} else if (arg == 0) {
		printk("[program2] : child process exit normally\n");
		printk("[program2] : The return signal is 0\n");
	} else if (arg == 1) {
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 1\n");
	} else if (arg == 2) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 2\n");
	} else if (arg == 9) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 9\n");
	} else if (arg == 13) {
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 13\n");
	} else if (arg == 14) {
		printk("[program2] : get SIGALARM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 14\n");
	} else if (arg == 15) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 15\n");
	} else if (arg == 4991) {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 19\n");
	}
	return;
}

void my_wait(pid_t pid) // waitpid
{
	int status;
	// begin initialize wo
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);
	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = (int __user *)&status;
	wo.wo_rusage = NULL;
	// end initialize wo
	int ret = do_wait(&wo);
	//	printk("do_wait return value is %d\n", &ret);
	put_pid(wo_pid);
	// 	printk("!! status = %d, state= %d\n", status, wo.wo_stat);
	print_signal(wo.wo_stat, status);
	return;
}

void my_execve(void *argc)
{
	const char *path = "/tmp/test";
	//	const char *const argv[] = {path, NULL, NULL};
	//	const char *const envp[] = {"HOME=/", "PATH=/sbin:/user/sbin:/bin:/usr/bin", NULL};
	struct filename *name = getname_kernel(path);
	printk("[program2] : child process\n");
	do_execve(name, NULL, NULL);
}

//implement fork function
int my_fork(void *argc)
{
	//set default sigaction for current process
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */
	struct kernel_clone_args kernel_args = {
		.flags = SIGCHLD,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		/* execute a test program in child process */
		.stack = (unsigned long)&my_execve,
		.exit_signal = SIGCHLD,
	};
	int pid = kernel_clone(&kernel_args);
	printk("[program2] : the child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       task_pid_nr(current));

	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init\n");

	/* write your code here */
	printk("[program2] : module_init {Hengyi Qu} {120090673}\n");
	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	process = kthread_create(&my_fork, NULL, "New_Thread");
	if (!IS_ERR(process)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(process);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
