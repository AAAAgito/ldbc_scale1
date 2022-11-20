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

#define WEXITSTATUS(status) (((status)&0xff00) >> 8)

#define WTERMSIG(status) ((status)&0x7f)

#define WSTOPSIG(status) (((status)&0xff00) >> 8)

#define WIFSTOPPED(status) (((status)&0xff) == 0x7f)

#define WIFEXITED(status) (((status)&0x7f) == 0)

#define WIFSIGNALED(status) (((signed char)(((status & 0x7f) + 1) >> 1)) > 0)

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

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname_kernel(const char *filename);

extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

int my_exec(void *argc)
{
	struct filename *filename = getname_kernel("/tmp/test");

	int result = do_execve(filename, NULL, NULL);

	printk("[program2] : child process");

	if (!result) {
		return 0;
	}
	do_exit(result);
}

void my_wait(pid_t pid)
{
	int status = 0;
	int a = 0;

	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type = PIDTYPE_PID;

	wo_pid = find_get_pid(pid);
	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);
	if (WIFEXITED(wo.wo_stat)) {
		printk("[program2] : child process gets normal termination\n");
		printk("[program2] : The return signal is %d", wo.wo_stat);
	} else if (WIFSTOPPED(wo.wo_stat)) {
		int is_stop = WSTOPSIG(wo.wo_stat);
		printk("[program2] : CHILD PROCESS STOPPED\n");
		printk("[program2] : child process get SIGSTOP signal\n");
		printk("[program2] : The return signal is %d", is_stop);
	} else if (WIFSIGNALED(wo.wo_stat)) {
		switch (WTERMSIG(wo.wo_stat)) {
		case SIGABRT:
			printk("[program2] : get SIGABRT signal\n");
			printk("[program2] : child process aborted\n");
			break;
		case SIGALRM:
			printk("[program2] : get SIGALRM signal\n");
			printk("[program2] : child process get a SIGALRM signal\n");
			break;
		case SIGTERM:
			printk("[program2] : get SIGTERM signal\n");
			printk("[program2] : child process terminated\n");
			break;
		case SIGQUIT:
			printk("[program2] : get SIGQUIT signal\n");
			printk("[program2] : child process quited\n");
			break;
		case SIGKILL:
			printk("[program2] : get SIGKILL signal\n");
			printk("[program2] : child process are killed\n");
			break;
		case SIGBUS:
			printk("[program2] : get SIGBUS signal\n");
			printk("[program2] : child process got BUS error\n");
			break;
		case SIGFPE:
			printk("[program2] : get SIGFPE signal\n");
			printk("[program2] : child process got Floating-point exception\n");
			break;
		case SIGHUP:
			printk("[program2] : get SIGHUP signal\n");
			printk("[program2] : child process hung up\n");
			break;
		case SIGILL:
			printk("[program2] : get SIGILL signal\n");
			printk("[program2] : child process got Illegal instruction\n");
			break;
		case SIGINT:
			printk("[program2] : get SIGINT signal\n");
			printk("[program2] : child process interrupted\n");
			break;
		case SIGPIPE:
			printk("[program2] : get SIGPIPE signal\n");
			printk("[program2] : child process got Broken pipe\n");
			break;
		case SIGSEGV:
			printk("[program2] : get SIGSEGV signal\n");
			printk("[program2] : child process got Segmentation violation\n");
			break;
		case SIGTRAP:
			printk("[program2] : get SIGTRAP signal\n");
			printk("[program2] : child process trapped\n");
			break;
		default:
			printk("[program2] : child process get a signal not in samples\n");
			break;
		}
		printk("[program2] : The return signal is %d",
		       WTERMSIG(wo.wo_stat));
	} else {
		printk("[program2] : CHILD PROCESS CONTINUED\n");
	}

	put_pid(wo_pid);
	return;
}

int my_fork(void *argc)
{
	pid_t pid;
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

	pid = kernel_thread(&my_exec, NULL, SIGCHLD);

	printk("[program2] : The Child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {%s} {%d}\n", "Yang zhao", 120090865);
	printk("[program2] : module_init create kthread start\n");

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
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
