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
extern struct filename *getname_kernel(const char *fliename);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
void my_exec(void *args)
{
	printk("[program2] : child process\n");
	int value = do_execve(
		getname_kernel(
			"/tmp/test"),
		NULL, NULL);
}
int my_WAIT_INT(int status)
{
	return (*(__const int *)&(status));
}
int my_WEXITSTATUS(int status)
{
	return (((status)&0xff00) >> 8);
}
int my_WTERMSIG(int status)
{
	return ((status)&0x7f);
}
int my_WSTOPSIG(int status)
{
	return my_WEXITSTATUS(status);
}
int my_WIFEXITED(int status)
{
	return (my_WTERMSIG(status) == 0);
}
int my_WIFSIGNALED(int status)
{
	return (((signed char)(((status)&0x7f) + 1) >> 1) > 0);
}
int my_WIFSTOPPED(int status)
{
	return (((status)&0xff) == 0x7f);
}
void printk_sig(char *tmp)
{
	printk("[program2] : get %s singal\n", tmp);
}
void my_wait(pid_t pid)
{
	struct wait_opts wo;
	enum pid_type type;
	type = PIDTYPE_PID;
	struct pid *wo_pid = NULL;
	wo_pid = find_get_pid(pid);
	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = 0;
	wo.wo_rusage = NULL;
	do_wait(&wo);
	int stat_value = my_WAIT_INT(wo.wo_stat);
	if (my_WIFEXITED(stat_value))
		stat_value = my_WEXITSTATUS(stat_value);
	else if (my_WIFSIGNALED(stat_value))
		stat_value = my_WTERMSIG(stat_value);
	else if (my_WIFSTOPPED(stat_value))
		stat_value = my_WSTOPSIG(stat_value);
	switch (stat_value) {
	case 0:
		printk("[program2] : child process exit normally\n");
		break;
	case 1:
		printk_sig("SIGHUP");
		break;
	case 2:
		printk_sig("SIGINT");
		break;
	case 3:
		printk_sig("SIGQUIT");
		break;
	case 4:
		printk_sig("SIGILL");
		break;
	case 5:
		printk_sig("SIGTRAP");
		break;
	case 6:
		printk_sig("SIGABRT");
		break;
	case 7:
		printk_sig("SIGBUS");
		break;
	case 8:
		printk_sig("SIGFPE");
		break;
	case 9:
		printk_sig("SIGKILL");
		break;
	case 11:
		printk_sig("SIGSEGV");
		break;
	case 13:
		printk_sig("SIGPIPE");
		break;
	case 14:
		printk_sig("SIGALRM");
		break;
	case 15:
		printk_sig("SIGTERM");
		break;
	case 19:
		printk_sig("SIGSTOP");
		break;
	}
	if (stat_value)
		printk("[program2] : child process terminated\n");
	printk("[program2] : The return signal is %d\n", stat_value);
	put_pid(wo_pid);
	return;
}
// implement fork function
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
	/* execute a test program in child process */
	pid_t pid = kernel_clone(&clone_args);
	printk("[program2] : The child process has pid = %d", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       task_pid_nr(current));
	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}
struct task_struct *task;
static int __init program2_init(void)
{
	printk("[program2] : Module_init {Guo Qingshuo} {121090151}\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "my_thread");
	if (!IS_ERR(task)) {
		printk("[program2] : module_init create kthread start\n");
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
