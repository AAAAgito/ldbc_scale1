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
#include <linux/wait.h>

MODULE_LICENSE("GPL");

struct wait_opts {
	enum pid_type wo_type; // It is defined in ‘/include/linux/pid.h’.
	int wo_flags; // Wait options. (0, WNOHANG, WEXITED, etc.)
	struct pid *wo_pid; // Kernel's internal notion of a process identifier.
	// “Find_get_pid()”
	struct siginfo __user *wo_info; // Singal information.
	int __user wo_stat; // Child process’s termination status
	struct rusage __user *wo_rusage; // Resource usage
	wait_queue_entry_t child_wait; // Task wait queue
	int notask_error;
};

extern pid_t kernel_clone(struct kernel_clone_args *kargs);

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname(const char __user *filename);

// Implement exec function
int my_exec(pid_t pid)
{
	int exec_result;
	const char path[] = "/tmp/test";
	const char *const argv[] = { path, NULL, NULL };
	const char *const envp[] = { "HOME=/",
				     "PATH=/sbin:/user/sbin:/bin:/user/bin",
				     NULL };

	struct filename *my_filename1 = getname_kernel(path);

	exec_result = do_execve(my_filename1, NULL, NULL);

	// If exec successfully terminated
	if (!exec_result)
		return 0;

	// If failed
	do_exit(exec_result);
}

// Implement wait function
void my_wait(pid_t pid)
{
	// int status;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	// wo.wo_stat = (int __user*)&status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	// Output child process exit status
	switch (wo.wo_stat & 0x7f) {
	case 0:
		printk("[program2] : child process normal terminated with EXIT STATUS = 0\n");
		break;
	case 1:
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process was hung up\n");
		printk("[program2] : The return signal is 1\n");
		break;
	case 2:
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process was interrupted\n");
		printk("[program2] : The return signal is 2\n");
		break;
	case 3:
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process quited\n");
		printk("[program2] : The return signal is 3\n");
		break;
	case 4:
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process had illegal instruction\n");
		printk("[program2] : The return signal is 4\n");
		break;
	case 5:
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process was trapped\n");
		printk("[program2] : The return signal is 5\n");
		break;
	case 6:
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process aborted\n");
		printk("[program2] : The return signal is 6\n");
		break;
	case 7:
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process had bus error\n");
		printk("[program2] : The return signal is 7\n");
		break;
	case 8:
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process had floating point exception\n");
		printk("[program2] : The return signal is 8\n");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process was killed\n");
		printk("[program2] : The return signal is 9\n");
		break;
	case 11:
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process had segment fault\n");
		printk("[program2] : The return signal is 11\n");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process had broken pipe\n");
		printk("[program2] : The return signal is 13\n");
		break;
	case 14:
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process was alarmed\n");
		printk("[program2] : The return signal is 14\n");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 15\n");
		break;
	default:
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		break;
	}

	put_pid(wo_pid);

	return;
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

	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.exit_signal = SIGCHLD,
	};
	pid_t pid = kernel_clone(&args);

	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	printk("[program2] : child process\n");

	if (pid == 0) {
		/* execute a test program in child process */

		my_exec(pid);
	} else {
		/* wait until child process terminates */
		my_wait(pid);
	}

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init Xu Xiangyu 120090446\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */
	printk("[program2] : Module_init create kthread start\n");

	struct task_struct *task;
	task = kthread_create(&my_fork, NULL, "my_fork");

	if (!IS_ERR(task)) {
		printk("[program2] : Module_init kthread start\n");
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
