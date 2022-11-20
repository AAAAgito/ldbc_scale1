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
#include <linux/sched/task.h>
#include <linux/signal.h>
#include <linux/slab.h>
#include <linux/wait.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;
struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;

	struct waitid_info *wo_info;
	int wo_start;
	struct rusage *wo_rusage;

	wait_queue_entry_t child_wait;
	int notask_error;
};
extern long do_wait(struct wait_opts *wo);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char __user *filename);

// implement execve function
int my_exec(void)
{
	int result;
	const char path[] = { "/tmp/test" };
	struct filename *my_filename = getname_kernel(path);
	printk("[program2] : chile process\n");
	result = do_execve(my_filename, NULL, NULL);
	if (!result) {
		return 0;
	}

	do_exit(result);
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
signed char my_WIFSIGNALED(int status)
{
	return (((signed char)(((status & 0x7f) + 1) >> 1)) > 0);
}
int my_WIFSTOPPED(int status)
{
	return (((status)&0xff) == 0x7f);
}
// implement wait function
int my_wait(pid_t pid)
{
	int status;
	long a;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	struct wait_opts wo = {
		.wo_type = type,
		.wo_pid = wo_pid,
		.wo_flags = WEXITED | WUNTRACED,
		.wo_info = NULL,
		.wo_start = status,
		.wo_rusage = NULL,
	};
	a = do_wait(&wo);
	put_pid(wo_pid);
	return wo.wo_start;
}

// implement fork function
int my_fork(void *argc)
{
	int status;
	struct kernel_clone_args kargs = {
		.flags = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};
	// set default sigaction for current process
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
	pid = kernel_clone(&kargs);

	/* execute a test program in child process */
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : The parent process has pid = %d\n",
	       (int)current->pid);
	status = my_wait(pid);
	if (my_WIFEXITED(status)) {
		printk("[program2] : child process terminated normally\n");
		printk("[program2] : The return signal is %d", status);
	} else if (my_WIFSTOPPED(status)) {
		int Stop_status = my_WSTOPSIG(status);
		printk("[program2] : CHILD PROCESS STOP\n");
		if (Stop_status == 19) {
			printk("[program2] : child process get SIGSTOP signal\n");
		} else {
			printk("[program2] : child process get a signal not in the sample\n");
		}
	} else if (my_WIFSIGNALED(status)) {
		int Termination_status = my_WTERMSIG(status);
		printk("[program2] : CHILD PROCESS FAILED\n");
		if (Termination_status >= 1 && Termination_status <= 15 &&
		    Termination_status == 1) {
			printk("[program2] : child process get SIGHUP signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 2) {
			printk("[program2] : child process get SIGINT signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 3) {
			printk("[program2] : child process get SIGQUIT signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 4) {
			printk("[program2] : child process get SIGILL signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 5) {
			printk("[program2] : child process get SIGTRAP signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 6) {
			printk("[program2] : child process get SIGABRT signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 7) {
			printk("[program2] : child process get SIGBUS signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 8) {
			printk("[program2] : child process get SIGFPE signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 9) {
			printk("[program2] : child process get SIGKILL signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 11) {
			printk("[program2] : child process get SIGSEGV signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 13) {
			printk("[program2] : child process get SIGPIPE signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 14) {
			printk("[program2] : child process get SIGALRM signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else if (Termination_status == 15) {
			printk("[program2] : child process get SIGTERM signal\n");
			printk("[program2] : The return siganl is %d",
			       Termination_status);
		} else {
			printk("[program2] : child process get a signal not in samples\n");
		}
	} else {
		printk("[program2] : CHILD PROCESS CONTINUE\n");
	}
	do_exit(0);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {He Xingjie} {120090466}\n");
	printk("[program2 : Module_init create kthread start\n");

	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "Mythread");
	if (!IS_ERR(task)) {
		printk("[program2] : Module_init Kthread starts\n");
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
