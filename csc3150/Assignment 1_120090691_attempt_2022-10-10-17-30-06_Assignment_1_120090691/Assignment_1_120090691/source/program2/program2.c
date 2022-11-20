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

/* Pre-define MACORS*/
#define WIFEXITED(status) (((status)&0x7f) == 0)
#define WEXITSTATUS(status) (((status)&0xff00) >> 8)
#define WIFSIGNALED(status) (((signed char)(((status)&0x7f) + 1) >> 1) > 0)
#define WTERMSIG(status) ((status)&0x7f)
#define WIFSTOPPED(status) (((status)&0xff) == 0x7f)
#define WSTOPSIG(status) (((status)&0xff00) >> 8)

void my_execve(void); // declare the function
int sig_conv_out(int *status); // convert & print_out the signal
void output_signal(int signal); // output related signal

static struct task_struct *new_task;
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
struct kernel_clone_args my_kernel_args = {
	.flags = SIGCHLD,
	.stack = (unsigned long)&my_execve,
	.exit_signal = SIGCHLD,
	.stack_size = 0,
	.parent_tid = NULL,
	.child_tid = NULL,
};

extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *my_filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern pid_t kernel_clone(struct kernel_clone_args *my_kernel_args);
extern struct filename *getname_kernel(const char *raw_filename);

void my_execve(void)
{
	struct filename *my_filename;
	int result;

	const char raw_filename[] = "/tmp/test";
	my_filename =
		getname_kernel(raw_filename); // pointer of struct filename
	result = do_execve(my_filename, NULL, NULL);
}

int my_wait(pid_t pid)
{
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;

	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = -1;
	wo.wo_rusage = NULL;

	do_wait(&wo);
	put_pid(wo_pid);

	return (wo.wo_stat);
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
	/* execute a test program in child process */
	/* wait until child process terminates */

	pid_t child_pid, parent_pid;
	int status;

	child_pid = kernel_clone(&my_kernel_args);
	parent_pid = task_pid_nr(current);
	printk("[program2] : The child process has pid = %d\n", child_pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       parent_pid);
	printk("[program2] : Child process\n");

	status = my_wait(child_pid);

	int real_return_signal;
	real_return_signal = sig_conv_out(&status);
	printk("[program2] : Child process terminated\n");
	printk("[program2] : The return signal is %d\n", real_return_signal);

	return 0;
}

int sig_conv_out(int *status)
{
	if (WIFEXITED(*status)) {
		printk("[program2] : Normal termination with EXIT STATUS = %d.\n",
		       WEXITSTATUS(*status));
		return WEXITSTATUS(*status);
	} else if (WIFSIGNALED(*status)) {
		output_signal(WTERMSIG(*status));
		return WTERMSIG(*status);
	} else // if (WIFSTOPPED(*status))
	{
		output_signal(WSTOPSIG(*status));
		return WSTOPSIG(*status);
	}
}

void output_signal(int signal)
{
	switch (signal) {
	case 6:
		printk("[program2] : get %s signal\n", "SIGABRT");
		break;
	case 14:
		printk("[program2] : get %s signal\n", "SIGALRM");
		break;
	case 7:
		printk("[program2] : get %s signal\n", "SIGBUS");
		break;
	case 8:
		printk("[program2] : get %s signal\n", "SIGFPE");
		break;
	case 1:
		printk("[program2] : get %s signal\n", "SIGHUP");
		break;
	case 4:
		printk("[program2] : get %s signal\n", "SIGILL");
		break;
	case 2:
		printk("[program2] : get %s signal\n", "SIGINT");
		break;
	case 9:
		printk("[program2] : get %s signal\n", "SIGKILL");
		break;
	case 13:
		printk("[program2] : get %s signal\n", "SIGPIPE");
		break;
	case 3:
		printk("[program2] : get %s signal\n", "SIGQUIT");
		break;
	case 11:
		printk("[program2] : get %s signal\n", "SIGSEGV");
		break;
	case 19:
		printk("[program2] : get %s signal\n", "SIGSTOP");
		break;
	case 15:
		printk("[program2] : get %s signal\n", "SIGTERM");
		break;
	case 5:
		printk("[program2] : get %s signal\n", "SIGTRAP");
		break;

	default:
		printk("[program2] : get %s signal\n", "undefined!");
		break;
	}
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init QiXixian 120090691\n");

	/* write your code here */
	/* create a kernel thread to run my_fork */
	printk("[program2] : Module_init create kthread start\n");
	new_task = kthread_create(&my_fork, NULL, "MyThread\n");

	if (!IS_ERR(new_task)) {
		printk("[program2] : Module_init kthread start\n");
		wake_up_process(new_task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
