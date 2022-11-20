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

#define __WEXITSTATUS(status) (((status)&0xff00) >> 8)

/* If WIFSIGNALED(STATUS), the terminating signal.  */
#define __WTERMSIG(status) ((status)&0x7f)

/* If WIFSTOPPED(STATUS), the signal that stopped the child.  */
#define __WSTOPSIG(status) __WEXITSTATUS(status)

/* Nonzero if STATUS indicates normal termination.  */
#define __WIFEXITED(status) (__WTERMSIG(status) == 0)

/* Nonzero if STATUS indicates termination by a signal.  */
#define __WIFSIGNALED(status) (((signed char)(((status)&0x7f) + 1) >> 1) > 0)

/* Nonzero if STATUS indicates the child is stopped.  */
#define __WIFSTOPPED(status) (((status)&0xff) == 0x7f)

/* Nonzero if STATUS indicates the child continued after a stop.  We only
   define this if <bits/waitflags.h> provides the WCONTINUED flag bit.  */

MODULE_LICENSE("GPL");

static struct task_struct *task;

/* Declear struct */
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

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

int my_exec(void *argc)
{
	do_execve(getname_kernel(
			  "/tmp/test"),
		  NULL, NULL);

	return 0;
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
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_stat = &status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	if (__WIFEXITED(wo.wo_stat)) {
		printk("[program2] : normal termination with EXIT STATUS = %d\n",
		       __WEXITSTATUS(wo.wo_stat));
	} else if (__WIFSIGNALED(wo.wo_stat)) {
		if (__WTERMSIG(wo.wo_stat) == 6) {
			printk("[program2] : get SIGABRT signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 14) {
			printk("[program2] : get SIGALRM signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 7) {
			printk("[program2] : get SIGBUS signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 8) {
			printk("[program2] : get SIGFPE signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 1) {
			printk("[program2] : get SIGHUP signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 4) {
			printk("[program2] : get SIGILL signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 2) {
			printk("[program2] : get SIGINT signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 9) {
			printk("[program2] : get SIGKILL signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 13) {
			printk("[program2] : get SIGPIPE signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 3) {
			printk("[program2] : get SIGQUIT signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 11) {
			printk("[program2] : get SIGSEVG signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 15) {
			printk("[program2] : get SIGTERM signal\n");
		} else if (__WTERMSIG(wo.wo_stat) == 5) {
			printk("[program2] : get SIGTRAP signal\n");
		} else {
			printk("[program2] : Child Process terminated with status = %d\n",
			       __WTERMSIG(status));
		}
		/*printk("Child Process terminated with STATUS = %d\n", WTERMSIG(status));*/
	} else if (__WIFSTOPPED(wo.wo_stat)) {
		printk("[program2] : get SIGSTOP signal\n");
		/*printk("Child Process stopped with STATUS = %d\n", WSTOPSIG(status));*/
	} else {
		printk("[program2] : Child Process continued\n");
	}

	printk("[program2] : child process terminated\n");

	if (__WIFEXITED(wo.wo_stat)) {
		printk("[program2] : The return signal is %d\n",
		       __WEXITSTATUS(wo.wo_stat));
	} else if (__WIFSIGNALED(wo.wo_stat)) {
		printk("[program2] : The return signal is %d\n",
		       __WTERMSIG(wo.wo_stat));
	} else if (__WIFSTOPPED(wo.wo_stat)) {
		printk("[program2] : The return signal is %d\n",
		       __WSTOPSIG(wo.wo_stat));
	} else {
		printk("[program2] : Child Process continued\n");
	}

	put_pid(wo_pid);

	return;
}

//implement fork function
int my_fork(void *argc)
{
	struct kernel_clone_args kca = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.exit_signal = SIGCHLD,
		.stack = &my_exec,
		.stack_size = 0,
		.tls = 0,
	};

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

	/* fork a process using kernel_clone or kernel_thread */
	pid_t pid = kernel_clone(&kca);

	if (pid < 0) {
		printk("[program2] : Fork error\n");
		do_exit(0);
	} else {
		printk("[program2] : The child process has pid = %d\n", pid);
	}

	/* execute a test program in child process */

	/* wait until child process terminates */

	struct pid *a = find_get_pid(current->pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       a->numbers[a->level].nr);
	printk("[program2] : child process\n");

	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Shanjun Xie} {120090745}\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "NewThread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
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
