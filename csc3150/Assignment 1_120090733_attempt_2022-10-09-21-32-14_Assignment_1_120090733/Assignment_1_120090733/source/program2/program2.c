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
	struct siginfo __user *wo_info;
	int wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};

static struct task_struct *task;
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char __user *filename);
extern pid_t kernel_clone(struct kernel_clone_args *args);

void my_wait(pid_t pid)
{
	int status;
	int a;

	// int terminatedStatus;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);
	printk("[program2] : child process");

	if (wo.wo_stat == 0) {
		printk("[program2] : child process exit normally\n");
		printk("[program2] : The return signal is 0\n");
	} else if (wo.wo_stat == 1) {
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process is hung up\n");
		printk("[program2] : The return signal is 1\n");
	}

	else if (wo.wo_stat == 2) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : terminal interrupt\n");
		printk("[program2] : The return signal is 2\n");
	}

	else if (wo.wo_stat == 131) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : terminal quit\n");
		printk("[program2] : The return signal is 3\n");
	}

	else if (wo.wo_stat == 132) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process has illegal instruction error\n");
		printk("[program2] : The return signal is 4\n");
	}

	else if (wo.wo_stat == 133) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process has trap error\n");
		printk("[program2] : The return signal is 5\n");
	}

	else if (wo.wo_stat == 134) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process has abort error\n");
		printk("[program2] : The return signal is 6\n");
	}

	else if (wo.wo_stat == 135) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process has bus error\n");
		printk("[program2] : The return signal is 7\n");
	}

	else if (wo.wo_stat == 136) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process has float error\n");
		printk("[program2] : The return signal is 8\n");
	}

	else if (wo.wo_stat == 9) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process is killed\n");
		printk("[program2] : The return signal is 9\n");
	}

	else if (wo.wo_stat == 139) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process has segmentation fault error\n");
		printk("[program2] : The return signal is 11\n");
	}

	else if (wo.wo_stat == 13) {
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process has pipe error\n");
		printk("[program2] : The return signal is 13\n");
	}

	else if (wo.wo_stat == 14) {
		printk("[program2] : get SIGALARM signal\n");
		printk("[program2] : child process has alarm error\n");
		printk("[program2] : The return signal is 14\n");
	} else if (wo.wo_stat == 15) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process is terminated\n");
		printk("[program2] : The return signal is 15\n");
	} else if (wo.wo_stat == 4991) {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process is terminated\n");
		printk("[program2] : The return signal is 19\n");
	} else {
		printk("[program2] : process continues\n");
		printk("[program2] : process signal is %d \n", wo.wo_stat);
	}
	put_pid(wo_pid);
	return;
}

int my_exec(void *p)
{
	int result;
	const char path[] = "/tmp/test";

	struct filename *my_filename = getname_kernel(path);
	result = do_execve(my_filename, NULL, NULL);
	if (!result) {
		return 0;
	}

	do_exit(result);
}

//implement fork function
int my_fork(void *argc)
{
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};

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
	pid = kernel_clone(&args);

	printk("[program2] : The Child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	my_wait(pid);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Xiaoyi Zheng} {120090733}\n");
	printk("[program2] : module_init create kthread start\n");
	/* write your code here */

	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "Mythread");
	if (!IS_ERR(task)) {
		printk("[program2] : module_init Kthread start\n");
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
