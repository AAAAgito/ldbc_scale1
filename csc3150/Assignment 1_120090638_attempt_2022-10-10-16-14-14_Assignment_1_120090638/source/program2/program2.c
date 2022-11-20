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

static struct task_struct *task;

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);
extern long do_wait(struct wait_opts *wo);

int my_exec(void *argc)
{
	const char *path = "/tmp/test";
	do_execve(getname_kernel(path), NULL, NULL);

	return 0;
}

int my_wait(pid_t pid)
{
	int status;
	int return_signal;

	enum pid_type wo_type;
	struct pid *wo_pid = NULL;
	struct wait_opts wo;

	wo_type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = wo_type;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_pid = wo_pid;
	wo.wo_info = NULL;
	wo.wo_stat = (int __user *)&status;
	wo.wo_rusage = NULL;

	do_wait(&wo);

	if (wo.wo_stat == 25600 || wo.wo_stat == 4991) {
		return_signal = (wo.wo_stat & 0xff00) >> 8;
	} else {
		return_signal = wo.wo_stat & 0x7f;
	}

	put_pid(wo_pid);

	return return_signal;
}

// implement fork function
int my_fork(void *argc)
{
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

	/* execute a test program in child process */

	/* wait until child process terminates */

	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.exit_signal = SIGCHLD,
		.stack = &my_exec,
		.stack_size = 0,
	};

	pid_t pid;
	int return_signal;

	pid = kernel_clone(&args);

	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       task->pid);

	return_signal = my_wait(pid);

	printk("[program2] : child process\n");
	switch (return_signal) {
	case 6:
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process Abort (ANSI).\n");
		break;
	case 14:
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process Alarm clock (POSIX).\n");
		break;
	case 7:
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process BUS error (4.2 BSD).\n");
		break;
	case 8:
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process Floating-point exception (ANSI).\n");
		break;
	case 1:
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process Hangup (POSIX).\n");
		break;
	case 4:
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process Illegal instruction (ANSI).\n");
		break;
	case 2:
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process Interrupt (ANSI).\n");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process Kill, unblockable (POSIX).\n");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process Broken pipe (POSIX).\n");
		break;
	case 3:
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process Quit (POSIX).\n");
		break;
	case 11:
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process Segmentation violation (ANSI).\n");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process Termination (ANSI).\n");
		break;
	case 5:
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process Trace trap (POSIX).\n");
		break;
	case 19:
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process Stop, unblockable (POSIX).\n");
		break;
	case 100:
		printk("[program2] : child process Normal termination.\n");
		break;
	default:
		break;
	}

	printk("[program2] : The return signal is %d\n", return_signal);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Shaoqiang Sun} {120090638}\n");
	printk("[program2] : module_init create kthread start\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */

	task = kthread_create(&my_fork, NULL, "my_task");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
