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
#include <linux/signal.h>

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

static struct task_struct *task;

extern pid_t kernel_clone(struct kernel_clone_args *kargs);

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname_kernel(const char *filename);

int my_exec(void)
{
	int result;
	const char path[] = "/tmp/test";
	const char *const argv[] = { path, NULL, NULL };
	const char *const envp[] = { "HOME=/",
				     "PATH=/sbin:/user/sbin:/bin:/usr/bin",
				     NULL };

	struct filename *my_filename = getname_kernel(path);

	printk("[program2] : child process");
	result = do_execve(my_filename, NULL, NULL);
	// printk("result: %d\n", result);
	// printk("!result: %d\n", !result);

	if (!result) {
		// printk("abnormal");
		return 0;
	}
	// printk("normal");
	do_exit(result);
}

void my_wait(pid_t pid)
{
	// printk("pid: %d\n", pid);
	int status = 0;
	int value;

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

	value = do_wait(&wo);

	int s = wo.wo_stat;
	int flag = wo.wo_flags;
	// printk("flag: %d\n", flag);
	// printk("status: %d\n", s);

	switch (s) {
	case 134:
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 6\n");
		break;
	case 14:
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 14\n");
		break;
	case 135:
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 7\n");
		break;
	case 136:
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 8\n");
		break;
	case 1:
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 1\n");
		break;
	case 132:
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 4\n");
		break;
	case 2:
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 2\n");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 9\n");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 13\n");
		break;
	case 131:
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 3\n");
		break;
	case 139:
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 11\n");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 15\n");
		break;
	case 133:
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 6\n");
		break;
	case 0:
		printk("[program2] : No special signal\n");
		printk("[program2] : Child process exit normally\n");
		printk("[program2] : The return signal is 0\n");
		break;
	default:
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : Child process terminated\n");
		printk("[program2] : The return signal is 19\n");
		break;
	}
	put_pid(wo_pid);
	return;
}

//implement fork function
int my_fork(void *argc)
{
	struct kernel_clone_args args = { .flags = SIGCHLD,
					  .stack = (unsigned long)&my_exec,
					  .stack_size = 0,
					  .exit_signal = SIGCHLD,
					  .parent_tid = NULL,
					  .child_tid = NULL,
					  .tls = 0 };

	pid_t pid;
	//set default sigaction for current process
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

	/* fork a process using do_fork */

	pid = kernel_clone(&args);

	printk("[program2] : The child process has pid= %d\n", pid);
	printk("[program2] : The parent process has pid= %d\n",
	       (int)current->pid);
	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Junyu Jin} {120090477}\n");
	printk("[program2] : Module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "Mythread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread starts\n");
		wake_up_process(task);
	}
	//wake up new thread if ok
	/* create a kernel thread to run my_fork */

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
