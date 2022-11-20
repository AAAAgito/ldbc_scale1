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

/* create a task_struct for thread */
static struct task_struct *task;

/* struct wait_opts prototype */
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

/* export functions from kernel */
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname(const char __user *filename);
extern struct filename *getname_kernel(const char *filename);

/* simulation functions */
int my_fork(void *argc);
int my_exec(void);
int my_wait(pid_t pid);

MODULE_LICENSE("GPL");

/* implement fork function */
int my_fork(void *argc)
{
	/* set default sigaction for current process */
	int i;
	pid_t pid;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	/* fork a process using kernel_clone */
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = &my_exec,
		.stack_size = 0,
		.exit_signal = SIGCHLD,
	};
	pid = kernel_clone(
		&args); // the test program will be executed in my_exec() function

	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	printk("[program2] : child process\n");
	/* wait until child process terminates */
	int parent_wait = my_wait(pid);
	/* get the signal type according to the signal number */
	switch (parent_wait) {
	case 0:
		printk("[program2] : get NORMAL signal\n");
		break;
	case 1:
		printk("[program2] : get SIGHUP signal\n");
		break;
	case 2:
		printk("[program2] : get SIGINT signal\n");
		break;
	case 3:
		printk("[program2] : get SIGQUIT signal\n");
		break;
	case 4:
		printk("[program2] : get SIGILL signal\n");
		break;
	case 5:
		printk("[program2] : get SIGTRAP signal\n");
		break;
	case 6:
		printk("[program2] : get SIGABRT signal\n");
		break;
	case 7:
		printk("[program2] : get SIGBUS signal\n");
		break;
	case 8:
		printk("[program2] : get SIGFPE signal\n");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal\n");
		break;
	case 10:
		printk("[program2] : get SIGUSR1 signal\n");
		break;
	case 11:
		printk("[program2] : get SIGSEGV signal\n");
		break;
	case 12:
		printk("[program2] : get SIGUSR2 signal\n");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal\n");
		break;
	case 14:
		printk("[program2] : get SIGALRM signal\n");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal\n");
		break;
	case 16:
		printk("[program2] : get SIGSTKFLT signal\n");
		break;
	case 17:
		printk("[program2] : get SIGCHLD signal\n");
		break;
	case 18:
		printk("[program2] : get SIGCONT signal\n");
		break;
	case 127:
		printk("[program2] : get SIGSTOP signal\n");
		break;
	case 20:
		printk("[program2] : get SIGTSTP signal\n");
		break;
	case 21:
		printk("[program2] : get SIGTTIN signal\n");
		break;
	case 22:
		printk("[program2] : get SIGTTOU signal\n");
		break;
	case 23:
		printk("[program2] : get SIGURG signal\n");
		break;
	case 24:
		printk("[program2] : get SIGXCPU signal\n");
		break;
	case 25:
		printk("[program2] : get SIGXFSZ signal\n");
		break;
	case 26:
		printk("[program2] : get SIGVTALRM signal\n");
		break;
	case 27:
		printk("[program2] : get SIGPROF signal\n");
		break;
	case 28:
		printk("[program2] : get SIGWINCH signal\n");
		break;
	case 29:
		printk("[program2] : get SIGIO signal\n");
		break;
	case 30:
		printk("[program2] : get SIGPWR signal\n");
		break;
	case 31:
		printk("[program2] : get SIGSYS signal");
		break;
	default:
		printk("[program2] : get signal\n");
		break;
	}
	printk("[program2] : child process terminated\n");
	if (parent_wait == 127) {
		printk("[program2] : The return signal is 19\n",
		       parent_wait); // print the corresponding signal
	} else if (parent_wait == 0) {
		printk("[program2] : The return signal is 0\n",
		       parent_wait); // print the corresponding signal
	} else {
		printk("[program2] : The return signal is %d\n",
		       parent_wait); // print the corresponding signal
	}

	return 0;
}

/* implement exec function */
int my_exec(void)
{
	/* execute a test program in child process */
	// const char path[] =
	// 	"/home/vagrant/csc3150/source/program2/test"; // describe the path
	const char path[] = "/tmp/test";
	struct filename *testfile = getname_kernel(path);
	int exec = do_execve(testfile, NULL, NULL);
	return 0;
}

/* implement wait function */
int my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	struct pid *wo_pid = find_get_pid(pid);
	enum pid_type type;
	type = PIDTYPE_PID;

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = (int __user)status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	put_pid(wo_pid);
	return wo.wo_stat & 0x7f;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Luozhexu} {120090427}\n");
	printk("[program2] : module_init create kthread start\n");
	printk("[program2] : module_init kthread start\n");
	/* create a kernel thread to run my_fork() */
	task = kthread_create(&my_fork, NULL, "MyThread");
	if (!IS_ERR(task)) {
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