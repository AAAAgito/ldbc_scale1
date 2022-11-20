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

#define KWIFSIGNALED(status) (((signed char)(((status)&0x7f) + 1) >> 1) > 0)
#define KWIFSTOPPED(status) (((status)&0xff) == 0x7f)
#define KWSTOPSIG(status) (((status)&0xff00) >> 8)
#define KWTERMSIG(status) ((status)&0x7f)
#define KWIFEXITED(status) (KWTERMSIG(status) == 0)
#define KWEXITSTATUS(status) (((status)&0xff00) >> 8)

MODULE_LICENSE("GPL");

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

// kernel non-static funcs:
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *file_name);

void sigprint(char *buffer, int signal)
{
	if (signal == SIGHUP)
		snprintf(buffer, 10, "SIGHUP");
	if (signal == SIGINT)
		snprintf(buffer, 10, "SIGINT");
	if (signal == SIGQUIT)
		snprintf(buffer, 10, "SIGQUIT");
	if (signal == SIGILL)
		snprintf(buffer, 10, "SIGILL");
	if (signal == SIGTRAP)
		snprintf(buffer, 10, "SIGTRAP");
	if (signal == SIGABRT)
		snprintf(buffer, 10, "SIGABRT");
	if (signal == SIGBUS)
		snprintf(buffer, 10, "SIGBUS");
	if (signal == SIGFPE)
		snprintf(buffer, 10, "SIGFPE");
	if (signal == SIGKILL)
		snprintf(buffer, 10, "SIGKILL");
	if (signal == SIGUSR1)
		snprintf(buffer, 10, "SIGUSR1");
	if (signal == SIGSEGV)
		snprintf(buffer, 10, "SIGSEGV");
	if (signal == SIGUSR2)
		snprintf(buffer, 10, "SIGUSR2");
	if (signal == SIGPIPE)
		snprintf(buffer, 10, "SIGPIPE");
	if (signal == SIGALRM)
		snprintf(buffer, 10, "SIGALRM");
	if (signal == SIGTERM)
		snprintf(buffer, 10, "SIGTERM");
	if (signal == SIGCHLD)
		snprintf(buffer, 10, "SIGCHLD");
	if (signal == SIGCONT)
		snprintf(buffer, 10, "SIGCONT");
	if (signal == SIGSTOP)
		snprintf(buffer, 10, "SIGSTOP");
	if (signal == SIGTSTP)
		snprintf(buffer, 10, "SIGTSTP");
	if (signal == SIGTTIN)
		snprintf(buffer, 10, "SIGTTIN");
	if (signal == SIGTTOU)
		snprintf(buffer, 10, "SIGTTOU");
	if (signal == SIGURG)
		snprintf(buffer, 10, "SIGURG");
	if (signal == SIGXCPU)
		snprintf(buffer, 10, "SIGXCPU");
	if (signal == SIGXFSZ)
		snprintf(buffer, 10, "SIGXFSZ");
	if (signal == SIGVTALRM)
		snprintf(buffer, 10, "SIGVTALRM");
	if (signal == SIGPROF)
		snprintf(buffer, 10, "SIGPROF");
	if (signal == SIGWINCH)
		snprintf(buffer, 10, "SIGWINCH");
	if (signal == SIGIO)
		snprintf(buffer, 10, "SIGIO");
	if (signal == SIGPWR)
		snprintf(buffer, 10, "SIGPWR");
	if (signal == SIGSYS)
		snprintf(buffer, 10, "SIGSYS");
	if (signal == 0)
		snprintf(buffer, 10, "0");
	if (signal == 16)
		snprintf(buffer, 10, "16");
}

int ex_kernel_wait(pid_t pid, int *stat, int flags)
{
	struct wait_opts wo = {
		.wo_type = PIDTYPE_PID,
		.wo_pid = find_get_pid(pid),
		.wo_flags = flags,
	};
	int ret;

	ret = do_wait(&wo);
	// if( ret > 0 && wo.wo_stat )
	*stat = wo.wo_stat;
	put_pid(wo.wo_pid);
	return ret;
}

int my_exec(void *args)
{
	/* execute a test program in child process */
	int ret_value = 0;
	const char *path = "/tmp/test";
	printk("[program2] : child process");
	// printk("[program2] : path = %s", path);

	ret_value = do_execve(getname_kernel(path), NULL, NULL);
	// printk("[program2] : ret_val = %d", ret_value);
	// if (ret_value)
	// 	do_exit(ret_value);
	return ret_value;
}

//implement fork function
int my_fork(void *argc)
{
	//set default sigaction for current process
	int i, status = 0;
	pid_t pid;
	char buffer[12];
	struct k_sigaction *k_action = &current->sighand->action[0];
	// struct kernel_clone_args fork_args = { exit_signal: SIGCHLD };

	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_thread(&my_exec, NULL, SIGCHLD);
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       *(&current->pid));

	/* wait until child process terminates */

	if (ex_kernel_wait(pid, &status, WUNTRACED | WEXITED) < 0) {
		printk("[program2] : wait error");
		do_exit(1);
	}

	// printk("[program2] : signal info = %d", status);
	if (KWIFSIGNALED(status)) {
		sigprint(buffer, KWTERMSIG(status));
		printk("[program2] : get %s signal", buffer);
		printk("[program2] : child process terminated");
		printk("[program2] : The return signal is %d",
		       KWTERMSIG(status));
	}

	if (KWIFSTOPPED(status)) {
		sigprint(buffer, KWSTOPSIG(status));
		printk("[program2] : get %s signal", buffer);
		printk("[program2] : child process STOPPED");
	}

	if (KWIFEXITED(status))
		printk("[program2] : child process ended normally with exit status %d\n",
		       KWEXITSTATUS(status));
	return 0;
}

static int __init program2_init(void)
{
	struct task_struct *kthr;
	// print info
	printk("[program2] : module_init Derong Jin 120090562\n");
	printk("[program2] : module_init create kthread start\n");

	kthr = kthread_create(&my_fork, NULL, "null");

	if (!IS_ERR(kthr)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(kthr);
	} else {
		printk("[program2] : module_init creat kthread FAILED!\n");
		return PTR_ERR(kthr);
	}

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
