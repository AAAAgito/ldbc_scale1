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
static struct task_struct *task;
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);

//struct wait_opts
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
extern long do_wait(struct wait_opts *wo);

// implement wait function
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
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_stat = &status;
	wo.wo_rusage = NULL;

	int signal_result;
	signal_result = do_wait(&wo);

	printk("[program2] : child process\n");
	if (wo.wo_stat == 1) {
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process is hung up\n");
		printk("[program2] : the return signal is %d\n", wo.wo_stat);
	}
	if (wo.wo_stat == 2) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process is interrupted\n");
		printk("[program2] : the return signal is %d\n", wo.wo_stat);
	}
	if ((wo.wo_stat - 128) == 3) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process quit\n");
		printk("[program2] : the return signal is %d\n",
		       (wo.wo_stat - 128));
	}
	if ((wo.wo_stat - 128) == 4) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process has illegal instruction\n");
		printk("[program2] : the return signal is %d\n",
		       (wo.wo_stat - 128));
	}
	if ((wo.wo_stat - 128) == 5) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process causes trace trap\n");
		printk("[program2] : the return signal is %d\n",
		       (wo.wo_stat - 128));
	}
	if ((wo.wo_stat - 128) == 6) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process is aborted\n");
		printk("[program2] : the return signal is %d\n",
		       (wo.wo_stat - 128));
	}
	if ((wo.wo_stat - 128) == 7) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process causes bus error\n");
		printk("[program2] : the return signal is %d\n",
		       (wo.wo_stat - 128));
	}
	if ((wo.wo_stat - 128) == 8) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process has floating point exception\n");
		printk("[program2] : the return signal is %d\n",
		       (wo.wo_stat - 128));
	}
	if (wo.wo_stat == 9) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process is killed\n");
		printk("[program2] : the return signal is %d\n", wo.wo_stat);
	}
	if ((wo.wo_stat - 128) == 11) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process has invalid memory segment access\n");
		printk("[program2] : the return signal is %d\n",
		       (wo.wo_stat - 128));
	}
	if (wo.wo_stat == 13) {
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process has broken pipe\n");
		printk("[program2] : the return signal is %d\n", wo.wo_stat);
	}
	if (wo.wo_stat == 14) {
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process has expired alarm clock\n");
		printk("[program2] : the return signal is %d\n", wo.wo_stat);
	}
	if (wo.wo_stat == 15) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : the return signal is %d\n", wo.wo_stat);
	}
	if (wo.wo_stat == 4991) {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		printk("[program2] : the return signal is 19\n");
	}
	if (wo.wo_stat == 25600) {
		printk("[program2] : get SIGCHLD signal\n");
		printk("[program2] : Normal termination with return signal 17\n");
	}
	put_pid(wo_pid);

	return;
}

// implement my_execve function
int my_execve(void)
{
	int result;
	//const char path[] = "C:/Users/yang/Desktop/template_source/source/program2/test";
	//const char path[] = "/home/vagrant/csc3150/Assignment1/source/program2/test";
	const char path[] = "/tmp/test";
	struct filename *my_filename = getname_kernel(path);
	result = do_execve(my_filename, NULL, NULL);
	if (!result) {
		return 0;
	}
	//if failed
	else {
		do_exit(result);
	}
}

//implement fork function
int my_fork(void *argc)
{
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
	// struct kernel_clone_args
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.stack = (unsigned long)&my_execve,
		.stack_size = 0,
		.tls = 0,
		.exit_signal = SIGCHLD,
		.parent_tid = NULL,
		.child_tid = NULL,
	};

	/* fork a process using kernel_clone or kernel_thread */

	pid_t pid;
	printk("[program2] : Module_init kthread start\n");
	/* execute a test program in child process */
	pid = kernel_clone(&args);
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */
	if (pid > 0) {
		my_wait(pid);
	}
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {%s} {%d}\n", "Yang Nan", 120090602);

	/* write your code here */
	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "MyThread");
	if (!IS_ERR(task)) {
		printk("[program2] : Module_init create kthread start\n");
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
