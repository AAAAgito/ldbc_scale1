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
#include "wait.h"

MODULE_LICENSE("GPL");

int my_execve(void *a);
static const char *signal_name(long signal);
int my_fork(void *argc);
void my_wait(pid_t pid);

extern long do_wait(struct wait_opts *wo);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);

static const char *signal_name(long signal)
{
	char *siganls[32] = { "zero",      "SIGHUP",  "SIGINT",    "SIGQUIT",
			      "SIGILL",    "SIGTRAP", "SIGABRT",   "SIGBUS",
			      "SIGFPE",    "SIGKILL", "SIGUSR1",   "SIGSEGV",
			      "SIGUSR2",   "SIGPIPE", "SIGALRM",   "SIGTERM",
			      "SIGSTKFLT", "SIGCHLD", "SIGCONT",   "SIGSTOP",
			      "SIGTSTP",   "SIGTTIN", "SIGTTOU",   "SIGURG",
			      "SIGXCPU",   "SIGXFSZ", "SIGVTALRM", "SIGPROF",
			      "SIGWINCH",  "SIGIO",   "SIGPWR",    "SIGSYS" };
	return siganls[signal];
}


int my_execve(void *a)
{
	char path[] = "/tmp/test";
	int execve_return;

	execve_return = do_execve(getname_kernel(path), NULL, NULL);

	return 0;
}

//implement fork function
int my_fork(void *argc)
{
	//set default sigaction for current process
	int i;
	pid_t pid;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_thread(my_execve, NULL, SIGCHLD);
	/* execute a test program in child process */
	printk("[program2] : The child process has pid = %ld\n", pid);
	printk("[program2] : This is the parent process, pid = %ld\n",
	       (long)current->pid);
	printk("[program2] : child process\n");
	/* wait until child process terminates */
	my_wait(pid);

	return 0;
}

void my_wait(pid_t pid)
{
	int status = 100;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = find_get_pid(pid);
	wo.wo_flags = WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	long a;
	a = do_wait(&wo);

	// output child process exit status

	status = wo.wo_stat;
	if (__WIFEXITED(status)) {
		printk("[program2] : get normal signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 0\n");
	} else if (__WTERMSIG(status) != 19) {
		printk("[program2] : get %s signal\n",
		       signal_name(__WTERMSIG(status)));
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n",
		       __WTERMSIG(status));
	} else if (__WIFSTOPPED(status)) {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stoped\n");
		printk("[program2] : The return signal is 19\n");
	}
	return;
	put_pid(wo.wo_pid);
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {施若兰} {120090757}\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");

	struct task_struct *task;
	task = kthread_create(&my_fork, NULL, "my_thread");
	if (IS_ERR(task)) {
		printk("[program2] : module_init create kthread error\n");
		return 0;
	}
	printk("[program2] : module_init kthread start\n");
	wake_up_process(task);
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
	return;
}

module_init(program2_init);
module_exit(program2_exit);
