#include <linux/err.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;

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

extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);

// implement wait function
void my_wait(pid_t pid)
{
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	long a;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);

	if ((wo.wo_stat & 0x7f) == 0) {
		printk("[program2] : Normal termination\n");
		printk("[program2] : The return signal is %d\n", 0);
	} else if ((wo.wo_stat & 0x7f) == 6) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process is aborted\n");
		printk("[program2] : The return signal is %d\n", 6);
	} else if ((wo.wo_stat & 0x7f) == 14) {
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process is alarmed\n");
		printk("[program2] : The return signal is %d\n", 14);
	} else if ((wo.wo_stat & 0x7f) == 7) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process has bus error\n");
		printk("[program2] : The return signal is %d\n", 7);
	} else if ((wo.wo_stat & 0x7f) == 8) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process has floating point exception\n");
		printk("[program2] : The return signal is %d\n", 8);
	} else if ((wo.wo_stat & 0x7f) == 1) {
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process terminal hangup or process death\n");
		printk("[program2] : The return signal is %d\n", 1);
	} else if ((wo.wo_stat & 0x7f) == 4) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process has illegal instruction\n");
		printk("[program2] : The return signal is %d\n", 4);
	} else if ((wo.wo_stat & 0x7f) == 2) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process receives interrupt signal from keyboard\n");
		printk("[program2] : The return signal is %d\n", 2);
	} else if ((wo.wo_stat & 0x7f) == 9) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process is killed\n");
		printk("[program2] : The return signal is %d\n", 9);
	} else if ((wo.wo_stat & 0x7f) == 13) {
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process writes on broken pipe\n");
		printk("[program2] : The return signal is %d\n", 13);
	} else if ((wo.wo_stat & 0x7f) == 3) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process has quit signal\n");
		printk("[program2] : The return signal is %d\n", 3);
	} else if ((wo.wo_stat & 0x7f) == 11) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process refers to invalid memory\n");
		printk("[program2] : The return signal is %d\n", 11);
	} else if ((wo.wo_stat & 0x7f) == 15) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminates\n");
		printk("[program2] : The return signal is %d\n", 15);
	} else if ((wo.wo_stat & 0x7f) == 5) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process reaches a breakpoint\n");
		printk("[program2] : The return signal is %d\n", 5);
	} else {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process is stopped\n");
		printk("[program2] : The return signal is %d\n", 19);
	}
	put_pid(wo_pid);
	return;
}

// implement my_exec function
int my_exec(void)
{
	int value;
	const char path[] = "/tmp/test";
	struct filename *my_filename = getname_kernel(path);

	printk("[program2] : child process\n");
	value = do_execve(my_filename, NULL, NULL);

	if (!value) { // succeed
		return 0;
	}
	do_exit(value); // fail
}

// implement fork function
int my_fork(void *argc)
{
	int pid;
	struct kernel_clone_args kargs = {
		.flags = ((SIGCHLD | CLONE_VM | CLONE_UNTRACED) & ~CSIGNAL),
		.pidfd = NULL,
		.child_tid = NULL,
		.parent_tid = NULL,
		.exit_signal = SIGCHLD & CSIGNAL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0
	};
	// set default sigaction for current process
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
	pid = kernel_clone(&kargs);

	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */
	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init Xiang Fei 120090414\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");

	// wake up the process after creating the thread
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
