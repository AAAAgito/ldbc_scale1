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

extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

// Retrieve POSIX signal name from signal number
char *get_signal_name(int signum)
{
	static char *signame[] = {
		"NULL",    "SIGHUP",    "SIGINT",  "SIGQUIT",  "SIGILL",
		"SIGTRAP", "SIGABRT",   "SIGBUS",  "SIGFPE",   "SIGKILL",
		"SIGUSR1", "SIGSEGV",   "SIGUSR2", "SIGPIPE",  "SIGALRM",
		"SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT",  "SIGSTOP",
		"SIGTSTP", "SIGTTIN",   "SIGTTOU", "SIGURG",   "SIGXCPU",
		"SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO",
		"SIGPWR",  "SIGSYS"
	};
	if (signum < 0)
		signum = -signum;
	if (signum >= 32)
		signum = 0;
	return signame[signum];
}

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

#define __WEXITSTATUS(status) (((status)&0xff00) >> 8)

/* If WIFSIGNALED(STATUS), the terminating signal.  */
#define __WTERMSIG(status) ((status)&0x7f)

/* If WIFSTOPPED(STATUS), the signal that stopped the child.  */
#define __WSTOPSIG(status) __WEXITSTATUS(status)

/* Nonzero if STATUS indicates normal termination.  */
#define __WIFEXITED(status) (__WTERMSIG(status) == 0)

/* Nonzero if STATUS indicates termination by a signal.  */
#define __WIFSIGNALED(status) (((signed char)(((status)&0x7f) + 1) >> 1) > 0)

/* Nonzero if STATUS indicates the child is stopped.  */
#define __WIFSTOPPED(status) (((status)&0xff) == 0x7f)

// Implement the child process, which executes the test program
int my_exec(void *argc)
{
	char *path = "/tmp/test";
	printk("[program2] : child process\n");

	do_execve(getname_kernel(path), NULL, NULL);
	return 0;
}
// Implement wait function
int my_wait(pid_t pid)
{
	struct pid *wo_pid = find_get_pid(pid);
	struct wait_opts wo = { .wo_type = PIDTYPE_PID,
				.wo_pid = wo_pid,
				.wo_info = NULL,
				.wo_flags = WEXITED | WSTOPPED,
				.wo_rusage = NULL,
				.wo_stat = 0 };
	int ret;
	do_wait(&wo);
	put_pid(wo_pid);

	// Distinguish child process status
	if (__WIFSIGNALED(wo.wo_stat)) {
		// Program raised a signal
		printk("[program2] : get %s signal\n",
		       get_signal_name((__WTERMSIG(wo.wo_stat))));
		ret = __WTERMSIG(wo.wo_stat);
	} else if (__WIFSTOPPED(wo.wo_stat)) {
		// Program stopped
		printk("[program2] : get %s signal\n",
		       get_signal_name((__WSTOPSIG(wo.wo_stat))));
		ret = __WSTOPSIG(wo.wo_stat);
	} else {
		// Program exited
		printk("[program2] : get exit status = %d\n",
		       __WEXITSTATUS(wo.wo_stat));
		ret = __WEXITSTATUS(wo.wo_stat);
	}
	return ret;
}
// Implement fork function
int my_fork(void *argc)
{
	pid_t child_pid;
	int child_ret;

	// set default sigaction for current process
	int i;
	int argdata;
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
	argdata = 0;
	child_pid = kernel_thread(my_exec, &argdata, SIGCHLD);
	printk("[program2] : The child process has pid = %d", child_pid);
	printk("[program2] : This is the parent process, pid = %d",
	       task_pid_nr(current));

	/* wait until child process terminates */
	child_ret = my_wait(child_pid);
	printk("[program2] : child process terminated\n");
	printk("[program2] : The return signal is %d\n", child_ret);

	do_exit(0);
	return 0;
}

static int __init program2_init(void)
{
	struct task_struct *task;
	printk("[program2] : module_init {Li Jiaqi} {120090727}\n");

	// create a kernel thread
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "my_fork");
	// run my_fork function
	printk("[program2] : module_init kthread start\n");
	wake_up_process(task);

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);