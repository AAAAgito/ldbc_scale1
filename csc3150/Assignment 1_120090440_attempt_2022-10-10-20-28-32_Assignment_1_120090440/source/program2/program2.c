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

static struct task_struct *task;

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
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);

// implement wait function
void my_wait(pid_t pid)
{
	struct wait_opts wo = {
		.wo_type = PIDTYPE_PID,
		.wo_flags = WEXITED | WUNTRACED,
		.wo_pid = find_get_pid(pid),
		.wo_info = NULL,
		.wo_stat = 0,
		.wo_rusage = NULL,
	};
	int a;

	a = do_wait(&wo);

	if ((wo.wo_stat & 127) == 0) {
		printk("[program2] : child process terminates normally\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 6) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process is aborted\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 14) {
		printk("[program2] : get SIGALARM signal\n");
		printk("[program2] : child process releases alarm signal\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 7) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process has bus error\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 8) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process has floating point exception\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 1) {
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process is hung up\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 4) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process has illegal instruction\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 2) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process gets interrupt from keyboard\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 9) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process is killed\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 13) {
		printk("[program2] : get SIGFPIPE signal\n");
		printk("[program2] : child process writes on a pipe with no reader\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 3) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process has terminal quit\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 11) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process has invalid memory segment access\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 15) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminates\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 5) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process reach a breakpoint\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else if ((wo.wo_stat & 127) == 126) { // ivalid path or file name
		printk("[program2] : invalid path in the my_exec function\n");
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 127));
	} else {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stops\n");
		printk("[program2] : The return signal is %d\n", 19);
	}

	put_pid(wo.wo_pid); // decrease the count and free memory

	return;
}

// implement execve function
int my_exec(void)
{
	int result;
	// int retval;
	// struct filename * my_filename = getname_kernel(path);
	// retval = PTR_ERR(my_filename);
	// printk("[program2] : getname return %d", retval);

	/* execute a test program in child process */
	// const char *const argv[] = {"/home/seed/work/proj1/source/program2/test",
	// NULL};
	printk("[program2] : child process");
	result = do_execve(
		getname_kernel("/tmp/test"),
		NULL, NULL);

	if (!result)
		return 0; // succeed

	do_exit(result); // fail
}

// implement fork function
int my_fork(void *argc)
{
	// set default sigaction for current process
	struct kernel_clone_args kargs = { .flags = SIGCHLD,
					   .pidfd = NULL,
					   .child_tid = NULL,
					   .parent_tid = NULL,
					   .exit_signal = SIGCHLD & CSIGNAL,
					   .stack = (unsigned long)&my_exec,
					   .stack_size = 0,
					   .tls = 0 };
	pid_t pid;
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
	pid = kernel_clone(&kargs);

	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	/* execute a test program in child process */

	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init ChenHuaxun 120090440\n");

	/* write your code here */
	/* create a kernel thread to run my_fork */
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
