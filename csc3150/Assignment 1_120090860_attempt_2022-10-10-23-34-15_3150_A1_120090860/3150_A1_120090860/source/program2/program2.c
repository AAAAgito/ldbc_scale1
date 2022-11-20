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

extern long do_wait(struct wait_opts *wo);

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern struct filename *getname_kernel(const char *filename);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

// execute test program
int my_execv(void *argc)
{
    const char path[] = "/tmp/test";
	struct filename *my_filename = getname_kernel(path);
	int res = do_execve(my_filename, NULL, NULL); // argv = envp = NULL

	if (!res) {
		return 0;
	}
	return -1;
}

int my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	int d_w;

	wo.wo_type = PIDTYPE_PID;
	wo.wo_flags = 0 | WUNTRACED | WEXITED;
	wo.wo_pid = find_get_pid(pid); // get process descriptor
	wo.wo_info = NULL;
	wo.wo_rusage = NULL;

	d_w = do_wait(&wo);
	int sig = ((wo.wo_stat) & 0x7F);
	printk("[program2]: do wait start: %d\n", wo.wo_stat);

	if (wo.wo_stat == 1) {
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 2) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 9) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 13) {
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 14) {
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);

	}

	else if (wo.wo_stat == 15) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 131) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 132) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if ((wo.wo_stat) == 134) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 133) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 135) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);

	}

	else if (wo.wo_stat == 136) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if (wo.wo_stat == 139) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig);
	}

	else if ((wo.wo_stat) == 0) {
		printk("[program2] : child process terminated normally\n");
		printk("[program2] : The exit status is %d\n",
		       ((status)&0xff00) >> 8);
	}

	else if (((wo.wo_stat) & 0xFF) == 0x7F) {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is %d\n",
		       ((status)&0xff00) >> 8);
	}

	put_pid(wo.wo_pid);
	return 0;
}

// implement fork function
int my_fork(void *argc)
{
	// set default sigaction for current process
	int i;
	pid_t pid;
	int (*fn)(void *) = my_execv;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_thread */
	pid = kernel_thread(fn, NULL, SIGCHLD);

	if (pid == 0) /* execute a test program in child process */
	{
		int e = my_execv(NULL);
	}

	else /* wait until child process terminates */
	{
		printk("[program2] : The child process has pid = %d\n", pid);
		printk("[program2] : This is the parent process, pid = %d\n",
		       (int)current->pid);
		printk("[program2] : child process\n");

		my_wait(pid);
	}

	return 0;
}

static int __init program2_init(void)
{
	/* write your code here */
	printk("[program2] : module_init {Li Nanxuan} {120090860}\n");

	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "mythread");
	printk("[program2]: module_init create kthread start\n");

	if (!IS_ERR(task)) {
		printk("[program2]: module_init kthread start\n");
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
