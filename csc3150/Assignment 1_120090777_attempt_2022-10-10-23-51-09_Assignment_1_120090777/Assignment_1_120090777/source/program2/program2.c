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
	enum pid_type wo_type; // It is defined in ‘/include/linux/pid.h’.
	int wo_flags; // Wait options. (0, WNOHANG, WEXITED, etc.)
	struct pid *wo_pid; // Kernel's internal notion of a process identifier.
		// “Find_get_pid()”
	struct waited_info *wo_info; // Singal information.
	int wo_stat; // Child process’s termination status
	struct rusage *wo_rusage; // Resource usage
	wait_queue_entry_t child_wait; // Task wait queue
	int notask_error;
};

static struct task_struct *task;

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);
extern long do_wait(struct wait_opts *wo);
extern pid_t kernel_thread(int (*threadfn)(void *data), void *arg,
			   unsigned long flags);
extern void __noreturn do_exit(long code);

pid_t my_exec(void *argc)
{
	int result;
	const char *path =
		"/tmp/test"; /// home/vagrant/csc3150/HW1/source/program2/test
	struct filename *my_filename = getname_kernel(path);
	result = do_execve(my_filename, NULL, NULL);
	printk("[program2] : child process");
	if (!result) {
		return 0;
	}
	do_exit(result);
}

void my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED; // WUNTRACED to handle stop
	wo.wo_info = NULL;
	wo.wo_stat = (int __user *)&status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);
	// printk("do_wait return value is %d\n", &a);

	if (wo.wo_stat == 0) { // normal
		printk("child process exit normally\n");
		printk("[program2] : The return signal is 0\n");
	} else if (wo.wo_stat == 135) { // bus
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 7\n");
	} else if (wo.wo_stat == 1) { // hang up
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process is hung up\n");
		printk("[program2] : The return signal is 1\n");
	} else if (wo.wo_stat == 136) { // floating
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 8\n");
	} else if (wo.wo_stat == 2) { // interrupt
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : terminal interrupt\n");
		printk("[program2] : The return signal is 2\n");
	} else if (wo.wo_stat == 132) { // illegal instruction
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process has illegal instruction error\n");
		printk("[program2] : The return signal is 4\n");
	} else if (wo.wo_stat == 134) { // abort
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process has abort error\n");
		printk("[program2] : The return signal is 6\n");
	} else if (wo.wo_stat == 131) { // quit
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : terminal quit\n");
		printk("[program2] : The return signal is 3\n");
	} else if (wo.wo_stat == 139) { // segmentation
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process has segmentation fault error\n");
		printk("[program2] : The return signal is 11\n");
	} else if (wo.wo_stat == 13) { // pipe
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process has pipe error\n");
		printk("[program2] : The return signal is 13\n");
	} else if (wo.wo_stat == 133) { // trap
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process has trap error\n");
		printk("[program2] : The return signal is 5\n");
	} else if (wo.wo_stat == 9) { // kill
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process is killed\n");
		printk("[program2] : The return signal is 9\n");
	} else if (wo.wo_stat == 14) { // alarm
		printk("[program2] : get SIGALARM signal\n");
		printk("[program2] : child process has alarm error\n");
		printk("[program2] : The return signal is 14\n");
	} else if (wo.wo_stat == 15) { // terminate
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process is terminated\n");
		printk("[program2] : The return signal is 15\n");
	} else if (wo.wo_stat == 4991) {
		printk("[program2] : get SIGSTOP signal\n"); // STOP
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is 19\n");
	} else {
		printk("[program2] : process continues\n");
		printk("[program2] : process signal is %d \n", wo.wo_stat);
	}
	put_pid(wo_pid);
	return;
}
// implement fork function
int my_fork(void *argc)
{
	// set default sigaction for current process
	// the default state for the process
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
	// struct kernel_clone_args{
	// 	.flags = SIGCHILD,
	// 	.stack = &my_exec,
	// 	.stack_size = 0,
	// 	.parent_tid = NULL,
	// 	.child_tid = NULL,
	// 	.tls = 0,
	// 	.exit_signal = SIGCHLD,
	// };
	pid_t pid;
	pid = kernel_thread(&my_exec, NULL, SIGCHLD);
	/* execute a test program in child process */
	printk("[program2] : The child process has pid =%d\n", pid);
	printk("[program2] : This is the parent process, pid=%d\n",
	       current->pid);
	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Junzhi Chen} {120090777}\n");

	/* write your code here */
	printk("[program2] : Module_init create kthread start\n");
	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "MyThread");
	// wake up new thread if ok
	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread starts\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
