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

//Macro
#ifdef _POSIX_SOURCE
#define _W_INT(i) (i)
#else
#define _W_INT(w) (*(int *)&(w))
#define WCOREFLAG 0200
#endif

#define _WSTATUS(x)                                                            \
	(_W_INT(x) &                                                           \
	 0177) // If WIFEXITED(STATUS), the low-order 8 bits of the status.
#define _WSTOPPED 0177 //_WSTATUS if process is stopped
#define WIFSTOPPED(x) (_WSTATUS(x) == _WSTOPPED)
#define WSTOPSIG(x) (_W_INT(x) >> 8)
#define WIFSIGNALED(x) (_WSTATUS(x) != _WSTOPPED && _WSTATUS(x) != 0)
#define WTERMSIG(x) (_WSTATUS(x))
#define WIFEXITED(x) (_WSTATUS(x) == 0)
#define WEXITSTATUS(x) (_W_INT(x) >> 8)

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

extern struct filename *getname_kernel(const char __user *filename);

extern pid_t kernel_clone(struct kernel_clone_args *kargs);

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

static struct task_struct *task;

int my_exec(void)
{
	//printk("run my exec\n");

	int result;

	printk("[program2] : child process\n");

	//execute testfile
	result = do_execve(getname_kernel("/tmp/test"), NULL, NULL);
	//result = do_execve(getname_kernel("/home/xxl/桌面/Assignment_1_120090484/source/program2/test"), NULL, NULL);

	printk("[program2] : do_execve return %d\n", result);

	//if my_exec success
	if (!result) {
		return 0;
	}

	//if my_exec failed
	do_exit(result);
}

void my_wait(pid_t pid)
{
	int status;
	int sig_value;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WUNTRACED | WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = (int __user) & status;
	wo.wo_rusage = NULL;

	do_wait(&wo);

	//analyze signal

	//printk("[program2] : analyze signal\n");

	//printk("[program2] : the return signal is %d\n", status);

	if (WIFEXITED(wo.wo_stat)) {
		printk("[program2] : normal exit with status : %d\n",
		       WEXITSTATUS(wo.wo_stat));
	}

	else if (WIFSIGNALED(wo.wo_stat)) {
		sig_value = WTERMSIG(wo.wo_stat);

		if (sig_value == 1) {
			printk("[program2] : get SIGUP signal\n");
		} else if (sig_value == 2) {
			printk("[program2] : get SIGINT signal\n");
		} else if (sig_value == 3) {
			printk("[program2] : get SIGQUIT signal\n");
		} else if (sig_value == 4) {
			printk("[program2] : get SIGILL signal\n");
		} else if (sig_value == 5) {
			printk("[program2] : get SIGTRAP signal\n");
		} else if (sig_value == 6) {
			printk("[program2] : get SIGABRT signal\n");
		} else if (sig_value == 7) {
			printk("[program2] : get SIGBUS signal\n");
		} else if (sig_value == 8) {
			printk("[program2] : get SIGFPE signal\n");
		} else if (sig_value == 9) {
			printk("[program2] : get SIGKILL signal\n");
		} else if (sig_value == 11) {
			printk("[program2] : get SIGSEGV signal\n");
		} else if (sig_value == 13) {
			printk("[program2] : get SIGPIPE signal\n");
		} else if (sig_value == 14) {
			printk("[program2] : get SIGALRM signal\n");
		} else if (sig_value == 15) {
			printk("[program2] : get SIGTERM signal\n");
		}
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n", sig_value);
	}

	else if (WIFSTOPPED(wo.wo_stat)) {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is %d\n",
		       WSTOPSIG(wo.wo_stat));
	}

	put_pid(wo_pid);

	return;
}

//implement fork function
int my_fork(void *argc)
{
	struct kernel_clone_args kargs = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};

	int i;
	pid_t pid;

	//set default sigaction for current process
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

	//printk("run my fork\n");

	pid = kernel_clone(&kargs);

	if (pid < 0) {
		printk("[program2] : kernel_clone error!\n");
		return (10);
	} else {
		printk("[program2] : The child process has pid = %d\n", pid);
		printk("[program2] : This is the parent process,pid = %d\n",
		       current->pid);

		/* wait until child process terminates */

		my_wait(pid);
	}

	//printk("finish my fork\n");

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init Xiang Xinli 120090484\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */

	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "my_fork");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread starts\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
