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
MODULE_LICENSE("GPL");
static struct task_struct *task;

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);

int my_exec(void)
{
	int result;
	const char *path = "/tmp/test ";
	struct filename *my_filename = getname_kernel(path);
	printk("[program2] : child process");
	result = do_execve(my_filename, NULL, NULL);
	if (!result) {
		return 0;
	} else {
		do_exit(result);
	}
}

struct kernel_clone_args create = {
	.flags = SIGCHLD,
	.exit_signal = SIGCHLD,
	.stack = (unsigned long)&my_exec,
	// u64 flags;
	// int _user *pidfd;
	// int _uset *child_tid;
	// int _user *parent_tid;
	// int exit_signal;
	// unsigned long stack;
	// unsigned long stack_size;
	// unsigned long tls;
	// pid_t *set_tid;
	// size_t set_tid_size;
	// int cgroup;
	// struct cgroup *cgrp;
	// struct css_set *cset;
};

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

//执行等待程序
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
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;
	do_wait(&wo);
	// printk("do_wait return value is %d\n",&a);
	// printk("[Do_Fork]: The return signal is %d\n", *wo.wo_stat);
	put_pid(wo_pid);
	printk("[program2]: Child process pid = %d\n", pid);
	printk("[program2]: This is the parent process, pid = %d\n",
	       (int)current->pid);
	// abort=6,alarm=14,bus=10,
	// floating=8,hangup=1,illegal_instr=4
	// interrupt=2,kill=9,pipe=13,quit=3,
	// segment_fault=11,terminate=15,trap=5
	if (__WTERMSIG(wo.wo_stat)) {
		printk("[program2]:Normol exited, status=%d\n", status);
	}

	int sig_value = wo.wo_stat;
	if (sig_value == 6) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process: abort\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 14) {
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process: alarm]\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if ((sig_value == 7) || (sig_value == 135)) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process: bus\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 8) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process: floating\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 1) {
		printk("[program2] : get SIGUP signal\n");
		printk("[program2] : child process: hung up\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 4) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process: illegal_instr\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 2) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process: interrupt\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 9) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process: killed\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 13) {
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process: pipe\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 3) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process: quit\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 11) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process: segment_fault\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 15) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process: terminated\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	} else if (sig_value == 5) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process: trapped\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	}

	else if (__WIFSTOPPED(wo.wo_stat)) {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process: stopped\n");
		printk("[program2] : The return signal is %d\n", wo.wo_stat);
	} else {
		printk("[program2] : The return signal is %d\n", wo.wo_stat);
	}
}

int my_fork(void *argc)
{
	int check1;
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
	pid = kernel_clone(&create);
	return 0;
}

// static struct task_struct *task;
static int __init program2_init(void)
{
	printk("[program2]:Module_init {XinDong Zhou} {120090835}\n");
	printk("[program2]:Module_init Create kthread start\n");
	printk("[program2]:Module_init Kthread start\n");
	task = kthread_create(&my_fork, NULL,
			      " Thread1"); //创建线程执行my_fork程序
	wake_up_process(task);
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2]:Module_exit ./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
