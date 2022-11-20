#define WEXITSTATUS(s) (((s)&0xff00) >> 8)
#define WCOREDUMP(s) ((s)&0x80)
#define WTERMSIG(s) ((s)&0x7f)
#define WSTOPSIG(s) WEXITSTATUS(s)
#define WIFEXITED(s) (WTERMSIG(s) == 0)
#define WIFSTOPPED(s) (WTERMSIG(s) == 0x7f)
#define WIFSIGNALED(s) (WTERMSIG((s) + 1) >= 2)

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

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

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
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	// output child process exit status
	int s = wo.wo_stat;
	if (WIFEXITED(s)) {
		printk("[program2] : Normal termination");
	} else {
		if (WIFSIGNALED(s)) {
			if (WTERMSIG(s) == 14)
				printk("[program2] : get SIGALRM signal\n");
			else if (WTERMSIG(s) == 6)
				printk("[program2] : get SIGABRT signal\n");
			else if (WTERMSIG(s) == 7)
				printk("[program2] : get SIGBUS signal\n");
			else if (WTERMSIG(s) == 8)
				printk("[program2] : get SIGFPE signal\n");
			else if (WTERMSIG(s) == 1)
				printk("[program2] : get SIGHUP signal\n");
			else if (WTERMSIG(s) == 4)
				printk("[program2] : get SIGILL signal\n");
			else if (WTERMSIG(s) == 2)
				printk("[program2] : get SIGINT signal\n");
			else if (WTERMSIG(s) == 9)
				printk("[program2] : get SIGKILL signal\n");
			else if (WTERMSIG(s) == 13)
				printk("[program2] : get SIGPIPE signal\n");
			else if (WTERMSIG(s) == 3)
				printk("[program2] : get SIGQUIT signal\n");
			else if (WTERMSIG(s) == 11)
				printk("[program2] : get SIGSEGV signal\n");
			else if (WTERMSIG(s) == 15)
				printk("[program2] : get SIGTERM signal\n");
			else if (WTERMSIG(s) == 5)
				printk("[program2] : get SIGTRAP signal\n");
		} else if (WIFSTOPPED(s)) {
			printk("[program2] : get SIGSTOP signal\n");
			printk("[program2] : child process terminated");
			printk("[program2] : The return signal is %d",
			       WSTOPSIG(s));
			return;
		} else
			printk("[program2] : unsupported signal");
	}

	printk("[program2] : child process terminated");
	printk("[program2] : The return signal is %d", WTERMSIG(s));
	put_pid(wo_pid);

	return;
}

// implement exec function
void my_exec(void *argc)
{
	const char path[] = "/tmp/test";

	struct filename *test_file = getname_kernel(path);
	int do_execve_result;
	do_execve_result = do_execve(test_file, NULL, NULL);

	return 0;
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

	struct kernel_clone_args new_process_args = {
		.flags = SIGCHLD,
		.stack = &my_exec,
		.stack_size = 0,
		.child_tid = NULL,
		.exit_signal = SIGCHLD,
	};

	pid_t child_pid;
	/* fork a process using kernel_clone or kernel_thread */
	child_pid = kernel_clone(&new_process_args);
	printk("[program2] : The child process has pid = %d\n", child_pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       task_pid_nr(current));

	/* wait until child process terminates */
	printk("[program2] : child process");
	my_wait(child_pid);

	do_exit(0);
}

static struct task_struct *thread;

static int __init program2_init(void)
{
	printk("[program2] : Module_init Yinggan Xu 120090375\n");

	/* create a kernel thread to run my_fork */
	thread = kthread_create(&my_fork, NULL, "thread program2");

	printk("[program2] : module_init create kthread start\n");
	if ((thread)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(thread);
	}

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
