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

//define a struct to pass to kernel_clone()
// struct kernel_clone_args {
// 	u64 flags;
// 	int __user *pidfd;
// 	int __user *child_tid;
// 	int __user *parent_tid;
// 	int exit_signal;
// 	unsigned long stack;
// 	unsigned long stack_size;
// 	unsigned long tls;
// 	pid_t *set_tid;
// 	/* Number of elements in *set_tid */
// 	size_t set_tid_size;
// 	int cgroup;
// 	int io_thread;
// 	int kthread;
// 	int idle;
// 	int (*fn)(void *);
// 	void *fn_arg;
// 	struct cgroup *cgrp;
// 	struct css_set *cset;
// };

static struct task_struct *task;

//extern all needed functions from linux kernel
extern pid_t kernel_clone(struct kernel_clone_args *args);

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname_kernel(const char *filename);

char *failedSignal[] = { NULL,     "SIGHUP",  "SIGINT",  "SIGQUIT",
			 "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS",
			 "SIGFPE", "SIGKILL", NULL,      "SIGSEGV",
			 NULL,     "SIGPIPE", "SIGALRM", "SIGTERM" };

//using macro feature to distinguish the exiting status of the process
//the machanism is the same as it in linux kernel
int _WEXITSTATUS(int state)
{
	return ((state & 0xff00) >> 8);
}

int _WTERMSIG(int state)
{
	return (state & 0x7f);
}

int _WSTOPSIG(int state)
{
	return (_WEXITSTATUS(state));
}

int _WIFEXITED(int state)
{
	return (_WTERMSIG(state) == 0);
}

signed char _WIFSIGNALED(int state)
{
	return ((signed char)(((state & 0x7f) + 1) >> 1) > 0);
}

int _WIFSTOPPED(int state)
{
	return (((state)&0xff) == 0x7f);
}

//execute the test file
int my_exec(void)
{
	int result;
	const char path[] = "/tmp/test";
	// const char *const argv[] = {NULL};
	// const char *const envp[] = {NULL};

	struct filename *my_filename = getname_kernel(path);

	printk("[program2] : child process\n");
	result = do_execve(my_filename, NULL, NULL);

	if (!result) {
		return 0;
	}

	do_exit(result);
}

int my_wait(pid_t pid)
{
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
	wo.wo_stat = 0;
	wo.wo_rusage = NULL;

	do_wait(&wo);
	a = wo.wo_stat;

	put_pid(wo_pid);

	return a;
}

//implement fork function
struct kernel_clone_args kargs = {
	.flags = SIGCHLD,
	.exit_signal = SIGCHLD,
	.stack = (unsigned long)&my_exec,
	.stack_size = 0,
};

int my_fork(void *argc)
{
	//set default sigaction for current process
	int i;
	pid_t pid;
	int status;
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
	status = my_wait(pid);

	if (_WIFEXITED(status)) {
		printk("[program2] : child process get normal termination\n");
		printk("[program2] : The return signal is %d\n", status);
	} else if (_WIFSTOPPED(status)) {
		int stopSig = _WSTOPSIG(status);
		printk("[program2] : CHILD PROCESS STOPPED\n");
		if (stopSig == 19) {
			printk("[program2] : child process get SIGSTOP signal\n");
		} else {
			printk("[program2] : child process get unknown signal: %d\n",
			       stopSig);
		}
		printk("[program2] : The return signal is %d\n", stopSig);
	} else if (_WIFSIGNALED(status)) {
		int terminatedSig = _WTERMSIG(status);
		printk("[program2] : CHILD EXECUTION FAILED!\n");
		if (terminatedSig >= 1 && terminatedSig <= 15 &&
		    failedSignal[terminatedSig] != NULL) {
			printk("[program2] : child process get %s signal\n",
			       failedSignal[terminatedSig]);
		} else {
			printk("[program2] : child process get unknown signal: %d\n",
			       terminatedSig);
		}
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d", terminatedSig);
	} else {
		printk("[CHECK HERE] : THE SIGNAL IS %d\n", status);
		printk("[program2]: CHILD PROCESS CONTINUED\n");
	}
	do_exit(0);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Chen Xuanwen} {120090582}\n");
	printk("[program2] : Module_init creat kthread start\n");

	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "Mythread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init Kthread starts\n");
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