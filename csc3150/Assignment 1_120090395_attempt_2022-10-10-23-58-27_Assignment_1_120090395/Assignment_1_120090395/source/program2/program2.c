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
	enum pid_type		wo_type;
	int			wo_flags;
	struct pid		*wo_pid;

	struct waitid_info	*wo_info;
	int			wo_stat;
	struct rusage		*wo_rusage;

	wait_queue_entry_t		child_wait;
	int			notask_error;
};

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
	const char __user *const __user *__argv,
	const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename * getname_kernel(const char * filename);

static struct task_struct *task;

char *signame[] = {
		"",
		"SIGHUP",
		"SIGINT",
		"SIGQUIT",
		"SIGILL",
		"SIGTRAP",
		"SIGABRT",
		"SIGBUS",
		"SIGFPE",
		"SIGKILL",
		"SIGUSR1",
		"SIGSEGV",
		"SIGUSR2",
		"SIGPIPE",
		"SIGALRM",
		"SIGTERM",
		"SIGSTKFLT",
		"SIGCHLD",
		"SIGCONT",
		"SIGSTOP"
	};

void my_exec(void) {
	struct filename *fname;

	printk("[program2] : child process\n");
	
	fname = getname_kernel("/tmp/test");
	do_execve(fname, NULL, NULL);
}

void my_wait(pid_t pid) {
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

	do_wait(&wo);

	wo.wo_stat &= 7;

	printk("[program2] : get %s signal\n", signame[wo.wo_stat]);
	printk("[program2] : child process terminated\n");
	printk("[program2] : The return signal is %d\n", wo.wo_stat);

	put_pid(wo_pid);
}

//implement fork function
int my_fork(void *argc) {

	struct kernel_clone_args kca = {
		.flags = SIGCHLD,
		.stack = (unsigned long) &my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		.exit_signal = SIGCHLD
	};

	pid_t pid;

	//set default sigaction for current process

	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for(i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	pid = kernel_clone(&kca);
	
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n", current->pid);
	my_wait(pid);
	
	return 0;
}

static int __init program2_init(void) {

	printk("[program2] : module_init {Zeng Yihang} {120090395}\n");
	
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "mythread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	
	return 0;
}

static void __exit program2_exit(void) {
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);