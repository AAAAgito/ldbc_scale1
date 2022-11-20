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
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern struct filename *getname_kernel(const char *filename);
extern void put_pid(struct pid *pid);

//implement wait function
void my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;

	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_stat = (int __user *)status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);
	//printk("------------------- %d\n",wo.wo_stat);

	if ((wo.wo_stat & 127) == 0) {
		printk("[program2] : child process runs normally");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 1) {
		printk("[program2] : get SIGHUP signal");
		printk("[program2] : child process is hung up");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 2) {
		printk("[program2] : get SIGINT signal");
		printk("[program2] : child process gets interrupt from keyboard");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 3) {
		printk("[program2] : get SIGQUIT signal");
		printk("[program2] : child process quits");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 4) {
		printk("[program2] : get SIGILL signal");
		printk("[program2] : child process encounters illegal instruction");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 5) {
		printk("[program2] : get SIGTRAP signal");
		printk("[program2] : child process encounters breaking point for debugging");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 6) {
		printk("[program2] : get SIGABRT signal");
		printk("[program2] : child process encounters abnormal termination");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 7) {
		printk("[program2] : get SIGBUS signal");
		printk("[program2] : child process encounters bus error");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 8) {
		printk("[program2] : get SIGFPE signal");
		printk("[program2] : child process encounters floating-point exception");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	}

	else if ((wo.wo_stat & 127) == 9) {
		printk("[program2] : get SIGKILL signal");
		printk("[program2] : child process encounters forced-process termination");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 11) {
		printk("[program2] : get SIGSEGV signal");
		printk("[program2] : child process refers to invalid memory");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 13) {
		printk("[program2] : get SIGPIPE signal");
		printk("[program2] : child process writes to pipe with no readers");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 14) {
		printk("[program2] : get SIGALRM signal");
		printk("[program2] : child process is alarmed by real-timerclock");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 15) {
		printk("[program2] : get SIGTERM signal");
		printk("[program2] : child process terminates");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 126) { // ivalid path or file name
		printk("[program2] : invalid path in the my_exec function");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else {
		printk("[program2] : get SIGSTOP signal");
		printk("[program2] : child process stops");
		printk("[program2] : The return signal is %d\n", 19);
	}

	put_pid(wo_pid);

	return;
}

//implement exec function
int my_exec(void)
{
	int result;
	const char path[] =
		"/home/vagrant/csc3150/Assignment_1_120090533/source/program2/test";
	const char *const argv[] = { path, NULL, NULL };
	const char *const envp[] = { "HOME=/",
				     "PATH=/sbin:/user/sbin:/bin:/usr/bin",
				     NULL };

	struct filename *my_filename = getname_kernel(path);

	result = do_execve(my_filename, NULL, NULL);
	//if exec success
	if (!result)
		return 0;
	do_exit(result);
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
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};
	pid_t pid = kernel_clone(&args);

	printk("[program2] : THe child process has pid = %ld\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	printk("[program2] : child process");

	my_wait(pid);
	return 0;
}
static struct task_struct *task;

static int __init program2_init(void)
{
	printk("[program2] : module_init {Zhouzerui} {120090533}\n");
	/* write your code here */

	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "initiate fork");
	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
