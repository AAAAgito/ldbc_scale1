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

struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct siginfo __user *wo_info;
	int __user wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};

const char __user *path = "/tmp/test";
struct task_struct *task;

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname_kernel(const char __user *filename);

extern pid_t kernel_clone(struct kernel_clone_args *kargs);

static unsigned long my_exec(void)
{
	do_execve(getname_kernel(path), NULL, NULL);
	return 0;
}

//implement fork function
int my_fork(void *argc)
{
	//set default sigaction for current process

	int signal;
	int sta;
	int i;
	struct wait_opts wo;
	pid_t pid;
	char str[10];
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	struct kernel_clone_args kca = {.flags = SIGCHLD,
					.child_tid = NULL,
					.parent_tid = NULL,
					.stack = (unsigned long)&my_exec,
					.stack_size = 0,
					.tls = 0,
					.exit_signal = SIGCHLD };

	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_clone(&kca);
	printk("[program2] : The child process has pid = %d", pid);
	printk("[program2] : This is the parent process, pid = %d",
	       (int)current->pid);
	/* execute a test program in child process */

	/* wait until child process terminates */
	wo.wo_pid = find_get_pid(pid);
	wo.wo_info = NULL;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_type = PIDTYPE_PID;
	wo.wo_rusage = NULL;
	wo.wo_stat = (int __user) & sta;

	//printk("hello,%d",a);
	do_wait(&wo);
	//printk("hello world1");
	if (((wo.wo_stat) & 0x7f) == 0) {
		//printk("hello, world2");
		printk("[program2] : child process\n");
		printk("[program2] : Normal termination with EXIT STATUS = %d\n",
		       (((wo.wo_stat) & 0xff00) >> 8));
	} else if ((((signed char)(((wo.wo_stat) & 0x7f) + 1) >> 1) > 0)) {
		signal = ((wo.wo_stat) & 0x7f);
		if (signal == 1) {
			strcpy(str, "SIGHUP");
		} else if (signal == 3) {
			strcpy(str, "SIGQUIT");
		} else if (signal == 2) {
			strcpy(str, "SIGINT");
		} else if (signal == 5) {
			strcpy(str, "SIGTRAP");
		} else if (signal == 7) {
			strcpy(str, "SIGBUS");
		} else if (signal == 4) {
			strcpy(str, "SIGILL");
		} else if (signal == 6) {
			strcpy(str, "SIGABRT");
		} else if (signal == 8) {
			strcpy(str, "SIGFPE");
		} else if (signal == 13) {
			strcpy(str, "SIGPIPE");
		} else if (signal == 9) {
			strcpy(str, "SIGKILL");
		} else if (signal == 14) {
			strcpy(str, "SIGALRM");
		} else if (signal == 11) {
			strcpy(str, "SIGSEGV");
		} else if (signal == 15) {
			strcpy(str, "SIGTERM");
		}
		printk("[program2] : child process\n");
		printk("[program2] : get %s signal\n", str);
		printk("[program2] : child process terminated\n");
	}

	else if (((wo.wo_stat) & 0x7f) == 0x7f) {
		printk("[program2] : child process\n");
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : CHILD EXECUTION STOPPED\n");
	}

	else {
		printk("[program2] : child process\n");
		printk("[program2] : CHILD PROCESS CONTINUED\n");
	}
	int a = wo.wo_stat & 0x7f;
	if (a != 0x7f && a != 0)
		printk("[program2] : The return signal is %d\n", a);
	else if (a == 0x7f)
		printk("[program2] : The return signal is 19\n");
	put_pid(wo.wo_pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init Jingyu Huang 120090890\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");
	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "MyThread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start");
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
