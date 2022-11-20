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
	struct siginfo __user *wo_info;
	int __user wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};

extern long do_wait(struct wait_opts *wo);

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern struct filename *getname_kernel(const char *filename);

extern pid_t kernel_clone(struct kernel_clone_args *args);

extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

int my_exec(void *argc)
{
	printk("[program2] : The child process has pid = %d\n", current->pid);
	return do_execve(getname_kernel("/tmp/test"), NULL, NULL);
}
// //implement fork function

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
	wo.wo_flags = WEXITED | WUNTRACED;
	// wo.wo_flags = WEXITED;
	// wo.wo_flags = WEXITED | WSTOPPED;

	wo.wo_info = NULL;
	wo.wo_stat = (int __user *)&status;
	wo.wo_rusage = NULL;
	char *sig[15] = {
		"SIGHUP",  "SIGINT",  "SIGQUIT", "SIGILL",  "SIGTRAP",
		"SIGABRT", "SIGBUS",  "SIGFPE",  "SIGKILL", "SIGUSR1",
		"SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM"
	};
	int a;
	a = do_wait(&wo);
	printk("[program2] : This is the parent process, pid = %d\n",
	       current->pid);
	printk("[program2] : child process");
	if ((wo.wo_stat & 0x7f) != 127 && (wo.wo_stat & 0x7f) != 0) {
		printk("[program2] : get %s signal\n",
		       sig[(wo.wo_stat & 0x7f) - 1]);
	} else if ((wo.wo_stat & 0x7f) == 0) {
		printk("[program2] : child process normal exit\n");
	} else {
		printk("[program2] : get SIGSTOP signal\n");
	}
	printk("[program2] : child process terminated\n");
	if ((wo.wo_stat & 0x7f) != 127) {
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else {
		printk("[program2] : The return signal is 19\n");
	}
	put_pid(wo_pid);
	return;
}

int my_fork(void *argc)
{
	pid_t repid = 0;
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	repid = kernel_clone(&args);
	my_wait(repid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Xiao Zitong} {120090766}\n");
	// my_fork((void*)1);
	task = kthread_create(&my_fork, NULL, "MyThread");
	printk("[program2] : module_init create kthread start\n");
	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}

	/* write your code here */

	/* create a kernel thread to run my_fork */

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);