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
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);
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
struct task_struct *Thread;
struct filename *filename;

// implement fork function
void my_exec(void *args)
{
	printk("[program2] : child process\n");
	int return_value = do_execve(getname_kernel("/tmp/test"), NULL, NULL);
}

#define my_WAIT_INT(status) (*(__const int *)&(status))
#define my_WEXITSTATUS(status) (((status)&0xff00) >> 8)
#define my_WTERMSIG(status) ((status)&0x7f)
#define my_WSTOPSIG(status) my_WEXITSTATUS(status)
#define my_WIFEXITED(status) (my_WTERMSIG(status) == 0)
#define my_WIFSIGNALED(status) (((signed char)(((status)&0x7f) + 1) >> 1) > 0)
#define my_WIFSTOPPED(status) (((status)&0xff) == 0x7f)
#define WEXITSTATUS(status) my_WEXITSTATUS(my_WAIT_INT(status))
#define WTERMSIG(status) my_WTERMSIG(my_WAIT_INT(status))
#define WSTOPSIG(status) my_WSTOPSIG(my_WAIT_INT(status))
#define WIFEXITED(status) my_WIFEXITED(my_WAIT_INT(status))
#define WIFSIGNALED(status) my_WIFSIGNALED(my_WAIT_INT(status))
#define WIFSTOPPED(status) my_WIFSTOPPED(my_WAIT_INT(status))

void my_wait(pid_t pid)
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
	wo.wo_stat = 0;
	wo.wo_rusage = NULL;
	int RTV = do_wait(&wo);
	int return_value = -1;
	if (WIFEXITED(wo.wo_stat))
		return_value = WEXITSTATUS(wo.wo_stat);
	else if (WIFSIGNALED(wo.wo_stat))
		return_value = WTERMSIG(wo.wo_stat);
	else if (WIFSTOPPED(wo.wo_stat))
		return_value = WSTOPSIG(wo.wo_stat);
	if (return_value > 0) {
		if (return_value == 6)
			printk("[program2] : get SIGABRT signal\n");
		if (return_value == 14)
			printk("[program2] : get SIGALRM signal\n");
		if (return_value == 7)
			printk("[program2] : get SIGBUS signal\n");
		if (return_value == 8)
			printk("[program2] : get SIGFPE signal\n");
		if (return_value == 1)
			printk("[program2] : get SIGHUP signal\n");
		if (return_value == 4)
			printk("[program2] : get SIGILL signal\n");
		if (return_value == 2)
			printk("[program2] : get SIGINT signal\n");
		if (return_value == 9)
			printk("[program2] : get SIGKILL signal\n");
		if (return_value == 13)
			printk("[program2] : get SIGPIPE signal\n");
		if (return_value == 3)
			printk("[program2] : get SIGQUIT signal\n");
		if (return_value == 11)
			printk("[program2] : get SIGSEGV signal\n");
		if (return_value == 15)
			printk("[program2] : get SIGTERM signal\n");
		if (return_value == 5)
			printk("[program2] : get SIGTRAP signal\n");
		if (return_value == 19)
			printk("[program2] : get SIGSTOP signal\n");
	}
	printk("[program2] : child process terminated\n");
	if (WIFEXITED(wo.wo_stat))
		printk("[program2] : The return signal is %d\n",
		       WEXITSTATUS(wo.wo_stat));
	else if (WIFSIGNALED(wo.wo_stat))
		printk("[program2] : The return signal is %d\n",
		       WTERMSIG(wo.wo_stat));
	else if (WIFSTOPPED(wo.wo_stat))
		printk("[program2] : The return signal is %d\n",
		       WSTOPSIG(wo.wo_stat));
	put_pid(wo_pid);
	return;
}
int my_fork(void *argc)
{
	printk("[program2] : module_init kthread start\n");
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
	struct kernel_clone_args clone_args = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.exit_signal = SIGCHLD,
		.stack_size = 0,
		.tls = 0,
		.stack = (unsigned long)&my_exec,
	};
	pid_t pid = kernel_clone(&clone_args);
	printk("[program2] : The child process has pid = %d", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       task_pid_nr(current));
	/* execute a test program in child process */
	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Mang Qiuyang} {121020150}\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */
	Thread = kthread_create(&my_fork, NULL, "my thread");
	if (!IS_ERR(Thread)) {
		printk("[program2] : module_init create kthread start\n");
		wake_up_process(Thread);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
