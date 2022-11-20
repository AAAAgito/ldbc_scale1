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

#define __WIFSTOPPED(status) (((status)&0xff) == 0x7f)

static struct task_struct *task;

#define kthread_create(threadfn, data, namefmt, arg...)                        \
	kthread_create_on_node(threadfn, data, NUMA_NO_NODE, namefmt, ##arg)

struct task_struct *kthread_create_on_cpu(int (*threadfn)(void *data),
					  void *data, unsigned int cpu,
					  const char *namefmt);

pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags)
{
	struct kernel_clone_args args = {
		.flags = ((lower_32_bits(flags) | CLONE_VM | CLONE_UNTRACED) &
			  ~CSIGNAL),
		.exit_signal = (lower_32_bits(flags) & CSIGNAL),
		.stack = (unsigned long)fn,
		.stack_size = (unsigned long)arg,
	};

	return kernel_clone(&args);
};

typedef unsigned long ul;

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern struct filename *getname_kernel(const char *);

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
extern int do_wait(struct wait_opts *wo);

int my_execve(void *);

extern void my_wait(pid_t pid);

// implement fork function
int my_fork(void *argc)
{
	int i;
	long pid;
	struct k_sigaction *k_action = &current->sighand->action[0];

	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	/*用 kernel-thread 创造程序*/
	pid = kernel_thread(&my_execve, 0, SIGCHLD);

	printk("[program2] : The child process has pid = %ld\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */
	my_wait((pid_t)pid);

	return 0;
}

int my_execve(void *arg)
{
	int final;
	const char path[] =
		"/tmp/test";
	/*标明路径*/
	struct filename *my_filename = getname_kernel(path);
	final = do_execve(my_filename, NULL, NULL);
	if (!final)
		return 0;
	// if fail
	do_exit(final);
}

void my_wait(pid_t pid)
{
	int status;
	int a;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	printk("[program2] : child process\n");
	a = do_wait(&wo);

	if (((wo.wo_stat) & 0x7f) == 6) {
		printk("[program2] : child process get SIGABRT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 14) {
		printk("[program2] : child process get SIGALRM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 7) {
		printk("[program2] : child process get SIGBUS signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 8) {
		printk("[program2] : child process get SIGFPE signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 1) {
		printk("[program2] : child process get SIGHUP signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 4) {
		printk("[program2] : child process get SIGILL signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 2) {
		printk("[program2] : child process get SIGINT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 9) {
		printk("[program2] : child process get SIGKILL signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 13) {
		printk("[program2] : child process get SIGPIPE signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 3) {
		printk("[program2] : child process get SIGQUIT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 11) {
		printk("[program2] : child process get SIGSEGV signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 15) {
		printk("[program2] : child process get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	} else if (((wo.wo_stat) & 0x7f) == 5) {
		printk("[program2] : child process get SIGTRAP signal\n");
		printk("[program2] : child process terminated\n");
		printk("[Kernel Thread] : The return signal is %d\n",
		       (wo.wo_stat) & 0x7f);
	}

	else if (((wo.wo_stat) & 0xff) == 0x7f) {
		printk("[program2] : child process get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		printk("[Kernel Thread] : The return signal is %d\n", SIGSTOP);
	} else {
		printk("[program2] : Child process terminated\n");
	}
	put_pid(wo_pid);

	return;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {梁智昊} {120090780}\n");
	printk("[program2] : Module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");

	if (!IS_ERR(task)) {
		printk("[program2] : Module_init Kthread starts\n");
		wake_up_process(task);
	}

	/* write your code here */

	/* create a kernel thread to run my_fork */

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
