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
MODULE_AUTHOR("Future");
MODULE_DESCRIPTION("Assignment1, task 2.");
MODULE_VERSION("0.0.2");

// define wait_opts
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

// extern statements
extern long do_wait(struct wait_opts *wo);
extern int wake_up_process(struct task_struct *task);
extern struct filename *getname_kernel(const char *);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern pid_t kernel_clone(struct kernel_clone_args *kargs);

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

// static variable
static struct task_struct *task;
static const char *executable_name = "/tmp/test";
static struct filename *exe_fname;

// utility functions to parse the signal
int _WTERMSIG(int status)
{
	return (status)&0x7f;
}

int _WIFEXITED(int status)
{
	return _WTERMSIG(status) == 0;
}

int _WEXITSTATUS(int status)
{
	return ((status)&0xff00) >> 8;
}

int _WIFSIGNALED(int status)
{
	return ((signed char)(((status)&0x7f) + 1) >> 1) > 0;
}

int _WIFSTOPPED(int status)
{
	return ((status)&0xff) == 0x7f;
}

int _WSTOPSIG(int status)
{
	return _WEXITSTATUS(status);
}

char *_WSIG_TO_NAME(int status)
{
	switch (status) {
	case 1:
		return "SIGHUP";
	case 2:
		return "SIGINT";
	case 3:
		return "SIGQUIT";
	case 4:
		return "SIGILL";
	case 5:
		return "SIGTRAP";
	case 6:
		return "SIGABRT";
	case 7:
		return "SIGBUS";
	case 8:
		return "SIGFPE";
	case 9:
		return "SIGKILL";
	case 10:
		return "SIGUSR1";
	case 11:
		return "SIGSEGV";
	case 12:
		return "SIGUSR2";
	case 13:
		return "SIGPIPE";
	case 14:
		return "SIGALRM";
	case 15:
		return "SIGTERM";
	case 16:
		return "SIGSTKFLT";
	case 17:
		return "SIGCHLD";
	case 18:
		return "SIGCONT";
	case 19:
		return "SIGSTOP";
	case 20:
		return "SIGTSTP";
	case 21:
		return "SIGTTIN";
	case 22:
		return "SIGTTOU";
	case 23:
		return "SIGURG";
	case 24:
		return "SIGXCPU";
	case 25:
		return "SIGXFSZ";
	case 26:
		return "SIGVTALRM";
	case 27:
		return "SIGPROF";
	case 28:
		return "SIGWINCH";
	case 29:
		return "SIGIO";
	default:
		return "UNKNOWN CODE!";
	}
}

// child process function, to execute the executable file
int my_exec(void *data)
{
	int result;

	printk("[program2] : child process\n");
	exe_fname = getname_kernel(executable_name);
	result = do_execve(exe_fname, NULL, NULL);
	if (!result) {
		return 0;
	}
	do_exit(result);
}

int my_wait(pid_t pid)
{
	int do_wait_status;
	int signal;

	// make do_wait option args
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;

	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_stat = 0;
	wo.wo_rusage = NULL;

	do_wait_status = do_wait(&wo);
	// printk("[debug] : Do wait status = %d", do_wait_status);
	signal = wo.wo_stat;
	printk("[program2] : child process terminated. \n");

	put_pid(wo_pid);

	return signal;
}

// implement fork function
int my_fork(void *argc)
{
	int i, signal;
	pid_t execpid;
	struct k_sigaction *k_action;
	k_action = &current->sighand->action[0];
	printk("[program2] : module_init kthread start\n");

	// set default sigaction for current process
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */
	/* execute a test program in child process */
	execpid = kernel_thread(&my_exec, NULL, 17);

	printk("[program2] : The child process has pid = %d\n", execpid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       task->pid);

	/* wait until child process terminates */
	signal = my_wait(execpid);

	// printk("[debug] : my fork ends here %d\n", signal);

	/* parse the signal */
	if (_WIFEXITED(signal)) {
		printk("[program2] : Normal termination with EXIT STATUS = %d\n",
		       _WEXITSTATUS(signal));
	} else if (_WIFSIGNALED(signal)) {
		printk("[program2] : get %s signal\n",
		       _WSIG_TO_NAME(_WTERMSIG(signal)));
		printk("[program2] : The return signal is %d",
		       _WTERMSIG(signal));
	} else if (_WIFSTOPPED(signal)) {
		printk("[program2] : get %s signal\n",
		       _WSIG_TO_NAME(_WSTOPSIG(signal)));
		printk("[program2] : The return signal is %d",
		       _WSTOPSIG(signal));
	} else {
		printk("[program2] : continue!\n");
	}

	return 0;
}

static int __init program2_init(void)
{
	/* write your code here */

	printk("[program2] : Module_init {Lai Wei} {120090485} \n");

	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");

	task = kthread_create(&my_fork, NULL, "my_fork");

	if (!IS_ERR(task)) {
		wake_up_process(task);
	}
	// printk("[program2] : finishing program2_init\n");
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
