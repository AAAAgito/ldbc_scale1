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

MODULE_LICENSE("GPL");

typedef unsigned long ul;
static struct task_struct *task;

extern long do_wait(struct wait_opts *wo);
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);

int my_exec(void)
{
	const char path[] =
		"/tmp/test"; ///////////////////////////////////////////path
	struct filename *file1 = getname_kernel(path);
	int i = do_execve(file1, NULL, NULL);
	if (i == 0) {
		return 0;
	} else {
		do_exit(i);
	}
}

void my_wait(pid_t pid)
{
	long a;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = PIDTYPE_PID;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_rusage = NULL;
	a = do_wait(&wo);

	printk("[program2] : Child process\n");
	if ((wo.wo_stat & 127) == SIGABRT) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process was aborted\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGALRM) {
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process was alarmed\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGBUS) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process had bus error\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGFPE) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : Child process had floating point exception\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGHUP) {
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : Child process was hung up\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGILL) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : Child process had illegal instruction\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGINT) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : Child process was interrupted by teminal\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGPIPE) {
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : Child process was interrupted by broken pipe\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGQUIT) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : Child process had terminal quit\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGSEGV) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : Child process had problems in memeory segment access\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGTERM) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGTRAP) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : Child process reached a trap\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == SIGKILL) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : Child process was killed\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 0) {
		printk("[program2] : Normal termination\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : Child process was stopped\n");
		printk("[program2] : The return signal is 19");
	}
	printk("[program2] : Child process terminated");
	put_pid(wo_pid);
	return;
}

int my_fork(void *argc)
{
	struct kernel_clone_args kernel_clone_args_0 = {
		.flags = SIGCHLD,
		.stack = (ul)&my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		.pidfd = NULL,
		.exit_signal = (SIGCHLD & CSIGNAL)
	};
	long pid;
	struct k_sigaction *k_action = &current->sighand->action[0];
	int i;
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	pid = kernel_clone(&kernel_clone_args_0);

	printk("[program2] : The child process has pid = %ld\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	my_wait((pid_t)pid);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init XIAO Weizhao 120090588\n");
	printk("[program2] : Module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");
	if (!IS_ERR(task)) {
		printk("[program2] : Module_init kthread start\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);