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

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define __WAIT_INT(status) (status)
#define __WAIT_STATUS int *
#define __WAIT_STATUS_DEFN int *

/* If WIFEXITED(STATUS), the low-order 8 bits of the status.  */
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

#define WEXITSTATUS(status) __WEXITSTATUS(__WAIT_INT(status))
#define WTERMSIG(status) __WTERMSIG(__WAIT_INT(status))
#define WSTOPSIG(status) __WSTOPSIG(__WAIT_INT(status))
#define WIFEXITED(status) __WIFEXITED(__WAIT_INT(status))
#define WIFSIGNALED(status) __WIFSIGNALED(__WAIT_INT(status))
#define WIFSTOPPED(status) __WIFSTOPPED(__WAIT_INT(status))

char *signals[] = { NULL,     "SIGHUP",  "SIGINT",  "SIGQUIT",
		    "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS",
		    "SIGFPE", "SIGKILL", NULL,      "SIGSEGV",
		    NULL,     "SIGPIPE", "SIGALRM", "SIGTERM" };

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
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname(const char __user *filename);
extern struct filename *getname_kernel(const char *filename);
extern long do_wait(struct wait_opts *wo);

int my_exec(void *unused)
{
	/* execute a test program in child process */
	// printk("current pid is%d\n", (int)current->pid);
	int ret;
	const char *path = "/tmp/test";
	printk("[program2] : child process");
	ret = do_execve(getname_kernel(path), NULL, NULL);
	if (!ret) {
		return 0;
	}
	do_exit(ret);
}

long my_wait(pid_t pid, int *stat)
{
	long ret;
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

	ret = do_wait(&wo);
	*stat = wo.wo_stat;
	// printk("[program2]: The return signal is %d\n", *stat);
	put_pid(wo_pid);
	return ret;
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
	pid_t pid = kernel_thread(my_exec, NULL, SIGCHLD);

	printk("[program2] : This is the Child process, pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */
	int status;
	// printk("initial status is %d\n", status);
	long ret = my_wait(pid, &status);

	if (WIFEXITED(status)) {
		printk("[program2] : Normal termination with EXIT STATUS = %d\n",
		       WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		printk("[program2] : Child process is killed by SIGNAL: %s\n",
		       signals[WTERMSIG(status)]);
		printk("[program2] : children process terminated");
		printk("[program2] : The return signal is: %d\n",
		       WTERMSIG(status));
	} else if (WIFSTOPPED(status)) {
		printk("[program2] : Child process is stopped with SIGNAL: SIGSTOP\n");
		printk("[program2] : children process stopped\n");
		printk("[program2] : Child process is stopped with SIGNAL: %d\n",
		       WSTOPSIG(status));
	}
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init YANTONG 120090454\n");
	printk("[program2] : module_init create kthread start\n");

	/* write your code here */
	/* create a kernel thread to run my_fork */
	struct task_struct *task = kthread_create(
		&my_fork, NULL,
		"MyThread"); // The thread will not start running immeadiately. It will
	// start to execute when returned task_struct is passed to
	// wake_up_process.

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread starts\n");
		wake_up_process(task);
	}

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
