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

/*function declaration*/
int my_fork(void *argc);
int my_wait(pid_t pid);
static int __init program2_init(void);
static void __exit program2_exit(void);
int my_exec(void);
struct wait_opts;

/* extern the functions I need */
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname(const char __user *filename);
extern struct filename *getname_kernel(const char *filename);
extern long do_wait(struct wait_opts *wo);
extern int kernel_wait(pid_t pid, int *stat);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

/* fork some useful macros in
 * "/usr/include/x86_64-linux-gnu/bits/waitstatus.h"*/
int my_WEXITSTATUS(int status)
{
	return (((status)&0xff00) >> 8);
}; // return exit status

int my_WTERMSIG(int status)
{
	return ((status)&0x7f);
}; // return the signal number that terminated the process

int my_WSTOPSIG(int status)
{
	return my_WEXITSTATUS(status);
}; // return the signal number that terminated the process (19)

int my_WIFEXITED(int status)
{
	return (my_WTERMSIG(status) == 0);
}; // return true when the program exits normally

signed char my_WIFSIGNALED(int status)
{
	return (((signed char)(((status)&0x7f) + 1) >> 1) > 0);
}; // return when the program terminates unnormally

int my_WIFSTOPPED(int status)
{
	return (((status)&0xff) == 0x7f);
}; /// return true when a child process returns because it has been paused by a
	/// SIGSTOP

struct wait_opts {
	enum pid_type wo_type; // It is defined in ‘/include/linux/pid.h’.
	int wo_flags; // Wait options.(0,WHOHANG,WEXITED,etc.)
	struct pid *wo_pid; // Kernel's internal notion of a process identifier.
		// “Find_get_pid()”
	struct siginfo __user *wo_info; // Singal information.
	int __user wo_stat; // Child process’s termination status
	struct rusage __user *wo_rusage; // Resource usage
	wait_queue_entry_t child_wait; // Task wait queue
	int notask_error;
};

// implement fork function
extern int my_fork(void *argc)
{
	// set default sigaction for current process
	int i;
	int status; // return value of my_wait()
	pid_t pid; // pid id for child process
	struct kernel_clone_args kca = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};

	struct k_sigaction *k_action =
		&current->sighand
			 ->action[0]; // indicates how the signal is handled
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler =
			SIG_DFL; // use the default handler function
		k_action->sa.sa_flags =
			0; // A flag specifying how the signal is to be handled
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_clone(&kca); // fork a process
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	/* execute a test program in child process */

	/* wait until child process terminates */
	status = my_wait(pid);
	/* Process the returned signal */
	// printk("%d\n", status);
	if (my_WIFEXITED(status)) {
		printk("[program2] : child process exit normally\n");
		printk("Normal termination with EXIT STATUS = %d\n",
		       my_WEXITSTATUS(status));
	} // exit normally
	else if (my_WIFSTOPPED(status)) {
		// int sStatus = my_WSTOPSIG(status); // it should be 19
		printk("[program2] : child process get SIGSTOP signal\n");
		printk("[program2] : The return signal is %d\n",
		       my_WEXITSTATUS(status));
	} // child process stop
	else if (my_WIFSIGNALED(status)) {
		int tStatus = my_WTERMSIG(status);
		switch (tStatus) {
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
		case 19:
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
			printk("[program2] : get SIGSYS signal\n");
			break;
		case 34:
			printk("[program2] : get SIGTMIN signal\n");
			break;
		};
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d\n",
		       my_WTERMSIG(status));
	} // exit unnormally
	else {
		printk("[program2] : child process continue\n");
	} // continue
	// printk("[program2] : The return signal is %d\n", my_WTERMSIG(status));
	do_exit(0);
	return 0;
}

int status;
// implement wait function
int my_wait(pid_t pid)
{
	int a, b;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(
		pid); // Look up a PID from hash table and return with
		// it's count evaluated

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WSTOPPED;
	wo.wo_info = NULL;
	wo.wo_stat = 0;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);
	b = (wo.wo_stat);
	if (b == 0) {
		return 0;
	}
	// output child process exit status
	put_pid(wo_pid); // Decrease the count and free memory

	return b;
}

static struct task_struct *task;

static int __init program2_init(void)
{
	printk("[program2] : module_init {Junxiao Liu} {120090809}\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");

	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "Mythread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

int my_exec(void)
{
	int result;
	// struct filename * myFilename =
	// getname_kernel("/home/seed/work/proj1/source/program2/test"); //get the
	// exec filename
	struct filename *myFilename =
		getname_kernel("/tmp/test"); // get the exec filename
	printk("[program2] : child process\n");
	result = do_execve(myFilename, NULL, NULL); // execute the task

	if (!result) // successfully execute
	{
		return 0;
	} else {
		printk("error code : %d\n", result); // print the error code
		do_exit(result);
	}
}

module_init(program2_init);
module_exit(program2_exit);
