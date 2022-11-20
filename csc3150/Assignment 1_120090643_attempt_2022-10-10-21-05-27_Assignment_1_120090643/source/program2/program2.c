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
/* If WIFSIGNALED(STATUS), the terminating signal.  */
#define __WTERMSIG(status) ((status)&0x7f)
/* Nonzero if STATUS indicates normal termination.  */
#define __WIFEXITED(status) (__WTERMSIG(status) == 0)
/* Nonzero if STATUS indicates the child is stopped.  */
#define __WIFSTOPPED(status) (((status)&0xff) == 0x7f)
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

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname_kernel(const char *filename);
int my_exec(void)
{
	int exec_result;
	const char *path = "/tmp/test";

	struct filename *my_filename = getname_kernel(path);
	printk("[program2] : child process\n");
	exec_result = do_execve(my_filename, NULL, NULL);
	if (!exec_result) {
		return exec_result;
	} else {
		do_exit(exec_result);
	}
}

int my_wait(pid_t pid)
{
	struct wait_opts wo;
	struct pid *wo_pid;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);
	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = 0;
	wo.wo_rusage = NULL;
	do_wait(&wo);
	put_pid(wo_pid);
	return wo.wo_stat;
}
struct kernel_clone_args kargs = {
	.flags = SIGCHLD,
	.exit_signal = SIGCHLD,
	.stack = (unsigned long)&my_exec,
	.stack_size = 0,
};

// implement fork function
int my_fork(void *argc)
{
	// set default sigaction for current process
	int status;
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

	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_clone(&kargs);
	printk("[program2] : The Child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	/* execute a test program in child process */
	/* wait until child process terminates */
	status = my_wait(pid);
	if (__WIFEXITED(status)) {
		printk("[program2] : child process normally terminated\n");
		printk("[program2] : The return signal is 0\n");
	} else if (__WIFSTOPPED(status)) {
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is 19\n");
	} else {
		switch (status) {
		case 6:
			printk("[program2] : get SIGABRT signal\n");
			printk("[program2] : child process get aborted\n");
			printk("[program2] : The return signal is 6\n");
			break;
		case 134:
			printk("[program2] : get SIGABRT signal\n");
			printk("[program2] : child process get aborted\n");
			printk("[program2] : The return signal is 6\n");
			break;
		case 14:
			printk("[program2] : get SIGALRM signal\n");
			printk("[program2] : child process get alarmed\n");
			printk("[program2] : The return signal is 14\n");
			break;
		case 142:
			printk("[program2] : get SIGALRM signal\n");
			printk("[program2] : child process get alarmed\n");
			printk("[program2] : The return signal is 14\n");
			break;
		case 7:
			printk("[program2] : get SIGBUS signal\n");
			printk("[program2] : child process get bused\n");
			printk("[program2] : The return signal is 7\n");
			break;
		case 135:
			printk("[program2] : get SIGBUS signal\n");
			printk("[program2] : child process get bused\n");
			printk("[program2] : The return signal is 7\n");
			break;
		case 8:
			printk("[program2] : get SIGFPE signal\n");
			printk("[program2] : child process: floating\n");
			printk("[program2] : The return signal is 8\n");
			break;
		case 136:
			printk("[program2] : get SIGFPE signal\n");
			printk("[program2] : child process: floating\n");
			printk("[program2] : The return signal is 8\n");
			break;
		case 1:
			printk("[program2] : get SIGHUP signal\n");
			printk("[program2] : child process get hung up\n");
			printk("[program2] : The return signal is 1\n");
			break;
		case 129:
			printk("[program2] : get SIGHUP signal\n");
			printk("[program2] : child process get hung up\n");
			printk("[program2] : The return signal is 1\n");
			break;
		case 4:
			printk("[program2] : get SIGILL signal\n");
			printk("[program2] : child process get illegal instruction\n");
			printk("[program2] : The return signal is 4\n");
			break;
		case 132:
			printk("[program2] : get SIGILL signal\n");
			printk("[program2] : child process get illegal instruction\n");
			printk("[program2] : The return signal is 4\n");
			break;
		case 2:
			printk("[program2] : get SIGINT signal\n");
			printk("[program2] : child process get interrupted\n");
			printk("[program2] : The return signal is 2\n");
			break;
		case 130:
			printk("[program2] : get SIGINT signal\n");
			printk("[program2] : child process get interrupted\n");
			printk("[program2] : The return signal is 2\n");
			break;
		case 9:
			printk("[program2] : get SIGKILL signal\n");
			printk("[program2] : child process get killed\n");
			printk("[program2] : The return signal is 9\n");
			break;
		case 137:
			printk("[program2] : get SIGKILL signal\n");
			printk("[program2] : child process get killed\n");
			printk("[program2] : The return signal is 9\n");
			break;
		case 13:
			printk("[program2] : get SIGPIPE signal\n");
			printk("[program2] : child process get piped\n");
			printk("[program2] : The return signal is 13\n");
			break;
		case 141:
			printk("[program2] : get SIGPIPE signal\n");
			printk("[program2] : child process get piped\n");
			printk("[program2] : The return signal is 13\n");
			break;
		case 3:
			printk("[program2] : get SIGQUIT signal\n");
			printk("[program2] : child process get quited\n");
			printk("[program2] : The return signal is 3\n");
			break;
		case 131:
			printk("[program2] : get SIGQUIT signal\n");
			printk("[program2] : child process get quited\n");
			printk("[program2] : The return signal is 3\n");
			break;
		case 11:
			printk("[program2] : get SIGSEGV signal\n");
			printk("[program2] : child process get quited\n");
			printk("[program2] : The return signal is 11\n");
			break;
		case 139:
			printk("[program2] : get SIGSEGV signal\n");
			printk("[program2] : child process get quited\n");
			printk("[program2] : The return signal is 11\n");
			break;
		case 15:
			printk("[program2] : get SIGTERM signal\n");
			printk("[program2] : child process get terminated\n");
			printk("[program2] : The return signal is 15\n");
			break;
		case 143:
			printk("[program2] : get SIGTERM signal\n");
			printk("[program2] : child process get terminated\n");
			printk("[program2] : The return signal is 15\n");
			break;
		case 5:
			printk("[program2] : get SIGTRAP signal\n");
			printk("[program2] : child process get trapped\n");
			printk("[program2] : The return signal is 5\n");
			break;
		case 133:
			printk("[program2] : get SIGTRAP signal\n");
			printk("[program2] : child process get trapped\n");
			printk("[program2] : The return signal is 5\n");
			break;
		default:
			printk("[program2] : The return signal is %d\n",
			       status);
			break;
		}
	}
	return 0;
}
static struct task_struct *tsk;
static int __init program2_init(void)
{
	printk("[program2] : Module_init {chenqixu} {120090643}\n");
	printk("[program2] : module_init create kthread start\n");
	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init Kthread start\n");
	tsk = kthread_create(&my_fork, NULL, "My");
	wake_up_process(tsk);
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
