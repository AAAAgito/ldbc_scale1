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
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char __user *filename);
extern long do_wait(struct wait_opts *wo);

int my_WEXITSTATUS(int status)
{
	return ((status & 0xff00) >> 8);
}

int my_WTERMSIG(int status)
{
	return (status & 0x7f);
}

int my_WSTOPSIG(int status)
{
	return (my_WEXITSTATUS(status));
}

int my_WIFEXITED(int status)
{
	return (my_WTERMSIG(status) == 0);
}

signed char my_WIFSIGNALED(int status)
{
	return (((signed char)(((status & 0x7f) + 1) >> 1)) > 0);
}

int my_WIFSTOPPED(int status)
{
	return (((status)&0xff) == 0x7f);
}

int my_exec(void *a)
{
	int result;
	const char path[] = "/tmp/test";
	struct filename *my_filename = getname_kernel(path);
	printk("[program2] : child process");
	result = do_execve(my_filename, NULL, NULL);
	if (!result) {
		return 0;
	}
	do_exit(result);
}

int my_wait(pid_t pid)
{
	int a;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = 0;
	wo.wo_rusage = NULL;

	do_wait(&wo);
	a = wo.wo_stat;

	put_pid(wo_pid);
	return a;
}

// implement fork function
int my_fork(void *argc)
{
	// set default sigaction for current process
	int i;
	int status;
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
	pid = kernel_thread(my_exec, NULL, SIGCHLD);
	/* execute a test program in child process */
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */
	status = my_wait(pid);
	if (my_WIFEXITED(status)) {
		printk("[program2] : child process gets normal termination\n");
		printk("[program2] : The return signal is %d", status);
	} else if (my_WIFSTOPPED(status)) {
		int signal = my_WSTOPSIG(status);
		if (signal == 19) {
			printk("[program2] : Child process stopped\n");
		} else {
			printk("[program2] : child process get a siganl not in the samples\n");
		}
		printk("[program2] : The return signal is %d", signal);
	} else if (my_WIFSIGNALED(status)) {
		int signal = my_WTERMSIG(status);
		if (signal == 1) {
			printk("[program2] : get SIGHUP signal\n");
			printk("[program2] : child process hung up\n");
			printk("[program2] : The return signal is 1\n");
		} else if (signal == 2) {
			printk("[program2] : get SIGINT signal\n");
			printk("[program2] : terminal interrupt\n");
			printk("[program2] : The return signal is 2\n");
		} else if (signal == 3) {
			printk("[program2] : get SIGQUIT signal\n");
			printk("[program2] : terminal quit\n");
			printk("[program2] : The return signal is 3\n");
		} else if (signal == 4) {
			printk("[program2] : get SIGILL signal\n");
			printk("[program2] : child process has illegal_instr error\n");
			printk("[program2] : The return signal is 4\n");
		} else if (signal == 5) {
			printk("[program2] : get SIGTRAP signal\n");
			printk("[program2] : child process has trap error\n");
			printk("[program2] : The return signal is 5\n");
		} else if (signal == 6) {
			printk("[program2] : get SIGABRT signal\n");
			printk("[program2] : child process has abort error\n");
			printk("[program2] : The return signal is 6\n");
		} else if (signal == 7) {
			printk("[program2] : get SIGBUS signal\n");
			printk("[program2] : child process has bus error\n");
			printk("[program2] : The return signal is 7\n");
		} else if (signal == 8) {
			printk("[program2] : get SIGFPE signal\n");
			printk("[program2] : child process has floating error\n");
			printk("[program2] : The return signal is 8\n");
		} else if (signal == 9) {
			printk("[program2] : get SIGKILL signal\n");
			printk("[program2] : child process killed\n");
			printk("[program2] : The return signal is 9\n");
		} else if (signal == 11) {
			printk("[program2] : get SIGSEGV signal\n");
			printk("[program2] : child process has segmentation fault\n");
			printk("[program2] : The return signal is 11\n");
		} else if (signal == 13) {
			printk("[program2] : get SIGPIPE signal\n");
			printk("[program2] : child process has pipe error\n");
			printk("[program2] : The return signal is 13\n");
		} else if (signal == 14) {
			printk("[program2] : get SIGALRM signal\n");
			printk("[program2] : child process has alarm error\n");
			printk("[program2] : The return signal is 14\n");
		} else if (signal == 15) {
			printk("[program2] : get SIGTERM signal\n");
			printk("[program2] : child process terminated\n");
			printk("[program2] : The return signal is 15\n");
		} else {
			printk("[program2] : process get a signal which is not in the examples\n");
			printk("[program2] : process signal is %d \n", signal);
		}
	} else {
		printk("[program2] : child process continues\n");
	}
	do_exit(0);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Wang Shijie} {120090331}\n");

	/* write your code here */

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
