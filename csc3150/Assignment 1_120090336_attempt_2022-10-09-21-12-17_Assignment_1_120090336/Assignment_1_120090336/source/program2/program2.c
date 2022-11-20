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
#include <linux/signal.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;

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

// extern functions
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);
pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

int my_exec(void *argc)
{
	int rnt;
	const char *path =
		"/home/vagrant/csc3150/Assignment_1_120090336/program2/test";
	// const char *path = "/tmp/test";

	rnt = do_execve(getname_kernel(path), NULL, NULL);

	if (rnt != 0) {
		printk("[program2] : Error of execve!!, %d\n", rnt);
		return 0;
	}
	return rnt;
}

int calc_stat(int stat)
{
	int rnt;
	rnt = stat & 127;
	if (rnt == 127) {
		return 19; // stop signal
	} else {
		return rnt;
	}
}

void my_wait(pid_t pid)
{
	int status, a;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);
	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_stat = (long)&status;
	wo.wo_rusage = NULL;

	do_wait(&wo);

	// print out signal information
	printk("[program2] : child process\n");
	a = calc_stat(wo.wo_stat);
	switch (a) {
	case 6:
		printk("[program2] : get SIGABRT signal\n");
		break;
	case 14:
		printk("[program2] : get SIGALRM signal\n");
		break;
	case 7:
		printk("[program2] : get SIGBUS signal\n");
		break;
	case 8:
		printk("[program2] : get SIGFPE signal\n");
		break;
	case 1:
		printk("[program2] : get SIGHUP signal\n");
		break;
	case 4:
		printk("[program2] : get SIGILL signal\n");
		break;
	case 2:
		printk("[program2] : get SIGINT signal\n");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal\n");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal\n");
		break;
	case 3:
		printk("[program2] : get SIGQUIT signal\n");
		break;
	case 11:
		printk("[program2] : get SIGSEGV signal\n");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal\n");
		break;
	case 5:
		printk("[program2] : get SIGTRAP signal\n");
		break;
	case 0:
		printk("[program2] : get Normal Termination signal\n");
		break;
	default:
		printk("[program2] : get SIGSTOP signal\n");
		break;
	}
	printk("[program2] : child process terminated\n");
	printk("[program2] : The return signal is %d\n", a);

	put_pid(wo_pid);
	return;
}

// implement fork function
int my_fork(void *argc)
{
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

	pid = kernel_thread(&my_exec, NULL, SIGCHLD);

	/* execute a test program in child process */
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */
	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Chen Dekun} {120090336}\n");

	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
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
