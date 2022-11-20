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

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern pid_t kernel_clone(struct kernel_clone_args *kargs);

extern struct filename *getname_kernel(const char __user *filename);

int my_WTERMSIG(int status)
{
	return (status & 0x7f);
}

int my_WIFEXITED(int status)
{
	return ((status & 0x7f) == 0);
}

signed char my_WIFSIGNALED(int status)
{
	return (((signed char)(((status & 0x7f) + 1) >> 1)) > 0);
}

int my_WIFSTOPPED(int status)
{
	return (((status)&0xff) == 0x7f);
}

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
	wo.wo_stat = &status;
	wo.wo_rusage = NULL;

	do_wait(&wo);

	if (my_WIFEXITED(wo.wo_stat)) {
		printk("[program2] : Normal termination\n");
	} else if (my_WIFSTOPPED(wo.wo_stat)) {
		printk("[program2] : get SIGSTOP signal\n");
	} else if (my_WIFSIGNALED(wo.wo_stat)) {
		if (my_WTERMSIG(wo.wo_stat) == 1) {
			printk("[program2] : get SIGHUP signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 2) {
			printk("[program2] : get SIGINT signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 3) {
			printk("[program2] : get SIGQUIT signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 4) {
			printk("[program2] : get SIGILL signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 5) {
			printk("[program2] : get SIGTRAP signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 6) {
			printk("[program2] : get SIGABRT signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 7) {
			printk("[program2] : get SIGBUS signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 8) {
			printk("[program2] : get SIGFPE signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 9) {
			printk("[program2] : get SIGKILL signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 11) {
			printk("[program2] : get SIGSEGV signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 13) {
			printk("[program2] : get SIGPIPE signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 14) {
			printk("[program2] : get SIGALRM signal\n");
		} else if (my_WTERMSIG(wo.wo_stat) == 15) {
			printk("[program2] : get SIGTERM signal\n");
		}
	} else {
		printk("[program2] :Child process continued\n");
	}

	printk("[program2] : child process terminated\n");
	if (my_WIFSTOPPED(wo.wo_stat)) {
		printk("[program2] : The return signal is %d\n",
		       (wo.wo_stat & 0xff00) >> 8);
	} else {
		printk("[program2] : The return signal is %d\n",
		       my_WTERMSIG(wo.wo_stat));
	}

	put_pid(wo_pid);
	return;
}

int my_exec(void)
{
	int result;
	const char path[] = "/tmp/test";
	//const char *const argv[] = {path, NULL, NULL};
	//const char *const envp[] = {"HOME=/", "PATH=/sbin:/user/sbin:/bin:/usr/bin", NULL};

	struct filename *my_filename = getname_kernel(path);

	printk("[program2] : child process\n");
	result = do_execve(my_filename, NULL, NULL);
	return 0;
}

//implement fork function
int my_fork(void *argc)
{
	//set default sigaction for current process
	int i;
	pid_t pid;
	struct kernel_clone_args kargs = {
		.flags = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};

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

	pid = kernel_clone(&kargs);

	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */
	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Hu Wenhan} {120090565}\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");

	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "MyThread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);