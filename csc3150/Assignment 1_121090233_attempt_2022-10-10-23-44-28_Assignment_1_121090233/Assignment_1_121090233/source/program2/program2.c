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

char *table[16] = { "NORMAL", "SIGHUP",  "SIGINT",  "SIGQUIT",
		    "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS",
		    "SIGFPE", "SIGKILL", "10",      "SIGSEGV",
		    "12",     "SIGPIPE", "SIGALRM", "SIGTERM" };

static struct task_struct *task;

extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *envp);
extern struct filename *getname_kernel(const char *filename);
struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct waitid_info *wo_info;
	int wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};
extern long do_wait(struct wait_opts *wo);

// implement fork function

int my_WAIT_INT(int status)
{
	return (*(__const int *)&(status));
}
int my_WEXITSTATUS(int status)
{
	return ((status & 0xff00) >> 8);
}
int my_WTERMSIG(int status)
{
	return (status & 0x7f);
}
int my_WSTOPSIG(int status)
{
	return (my_WEXITSTATUS(status));
}
int my_WIFEXITED(int status)
{
	return (my_WTERMSIG(status) == 0);
}
int my_WIFSIGNALED(int status)
{
	return (((signed char)((status & 0x7f) + 1) >> 1) > 0);
}
int my_WIFSTOPPED(int status)
{
	return ((status & 0xff) == 0x7f);
}

int getsignal(int status)
{
	if (my_WIFEXITED(status))
		return my_WEXITSTATUS(status);
	if (my_WIFSIGNALED(status))
		return my_WTERMSIG(status);
	if (my_WIFSTOPPED(status))
		return my_WSTOPSIG(status);
	printk("[program2] : ERR!!!");
	return -1;
}

void my_exec(void *args)
{
	printk("[program2] : child process\n");
	do_execve(getname_kernel("/tmp/test"), NULL, NULL);
}

int my_wait(pid_t pid)
{
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = 0;
	wo.wo_rusage = NULL;

	do_wait(&wo);

	put_pid(wo_pid);

	return getsignal(my_WAIT_INT(wo.wo_stat));
}

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

	struct kernel_clone_args kargs = { .flags = SIGCHLD,
					   .exit_signal = SIGCHLD,
					   .stack = (unsigned long)&my_exec,
					   .stack_size = 0,
					   .parent_tid = NULL,
					   .child_tid = NULL,
					   .tls = 0 };
	pid_t pid = kernel_clone(&kargs);

	printk("[program2] : The child process has pid = %d", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	int sig = my_wait(pid);
	if (sig > 0 && sig <= 15) {
		printk("[program2] : get %s signal\n", table[sig]);
	} else {
		if (sig == 0) {
			printk("[program2] : Normal termination with EXIT STATUS = 0\n");
		} else if (sig == 19) {
			printk("[program2] : get SIGSTOP signal\n");
		} else {
			printk("[program2] : ERR! sig = %d", sig);
		}
	}
	printk("[program2] : child process terminated\n");
	printk("[program2] : The return signal is %d\n", sig);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Jiang Yige} {121090233}\n");

	printk("[program2] : module_init create kthread start\n");

	task = kthread_create(my_fork, NULL, "mythread");
	if (task) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	} else {
		printk("Thread Creation Failed!\n");
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
