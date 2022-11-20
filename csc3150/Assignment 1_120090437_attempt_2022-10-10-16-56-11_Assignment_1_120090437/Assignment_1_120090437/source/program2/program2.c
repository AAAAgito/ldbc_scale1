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
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern struct filename *getname_kernel(const char *filename);

void my_wait(pid_t pid)
{
	int status = 0;
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

	do_wait(&wo);

	// printk("%d\n",wo.wo_stat);
	// printk("%d\n",SIGABRT);

	switch (wo.wo_stat & 0x7f) {
	case SIGABRT:
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process has abort error\n");
		break;
	case SIGALRM:
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process reports alarm signal\n");
		break;
	case SIGBUS:
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process has bus error\n");
		break;
	case SIGFPE:
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process has fatal arithmetic error\n");
		break;
	case SIGHUP:
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process has hang-up termination\n");
		break;
	case SIGILL:
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process has illegal instruction error\n");
		break;
	case SIGINT:
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process reports program interrupt signal\n");
		break;
	case SIGKILL:
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process has kill termination\n");
		break;
	case SIGPIPE:
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process has writing to the pipe or FIFO without "
		       "reading\n");
		break;
	case SIGQUIT:
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process has terminal quit\n");
		break;
	case SIGSEGV:
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process has memory segmentation violation\n");
		break;
	case 127: // SIGSTOP
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		break;
	case SIGTERM:
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case SIGTRAP:
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process reaches breakpoint\n");
		break;
	default:
		printk("[program2] : get SIGCHLD signal\n");
		printk("[program2] : normal termination\n");
		break;
	}

	if ((wo.wo_stat & 0x7f) == 127) { // SIGSTOP
		printk("[program2] : The return signal is = %d\n", 19);
	} else if ((wo.wo_stat & 0x7f) == 126) {
		printk("[program2] : The return signal is = %d\n", 0);
	} else {
		printk("[program2] : The return signal is = %d\n",
		       wo.wo_stat & 0x7f);
	}

	put_pid(wo_pid);

	return;
}

int my_exec(void *argc)
{
	// const char path[] = "/home/vagrant/csc3150/project1/program2/test";
	const char path[] = "/tmp/test";
	struct filename *myfilename = getname_kernel(path);
	int exc = do_execve(myfilename, NULL, NULL);

	printk("[program2] : child process");
	// printk("%d\n", exc);

	if (!exc) {
		return 0;
	} else {
		do_exit(exc);
	}
}

// implement fork function
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

	/* fork a process using kernel_clone or kernel_thread */
	/* execute a test program in child process */
	// pid = kernel_thread(my_exec, NULL, CLONE_FS | CLONE_FILES | SIGCHLD);
	pid = kernel_thread(my_exec, NULL, SIGCHLD);
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       current->pid);

	/* wait until child process terminates */
	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {HU Wenxi} {120090437}\n");

	/* create a kernel thread to run my_fork */
	printk("[program2] : Module_init create kthread start");
	task = kthread_create(&my_fork, NULL, "MyThread");

	// wake up new thread
	if (!IS_ERR(task)) {
		printk("[program2] : Module_init kthread start");
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
