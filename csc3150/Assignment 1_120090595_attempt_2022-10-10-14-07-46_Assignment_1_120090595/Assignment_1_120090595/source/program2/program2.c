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

struct wait_opts;
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);

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

void my_wait(pid_t pid)
{
	int a;
	int status = 0;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type wo_type;
	wo_type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = wo_type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);

	printk("[program2] : child process\n");
	if ((wo.wo_stat & 0x7F) == 1) {
		printk("[program2] : get SIGHUP signal\n");
	} else if ((wo.wo_stat & 0x7F) == 2) {
		printk("[program2] : get SIGINT signal\n");
	} else if ((wo.wo_stat & 0x7F) == 3) {
		printk("[program2] : get SIGQUIT signal\n");
	} else if ((wo.wo_stat & 0x7F) == 4) {
		printk("[program2] : get SIGILL signal\n");
	} else if ((wo.wo_stat & 0x7F) == 5) {
		printk("[program2] : get SIGTRAP signal\n");
	} else if ((wo.wo_stat & 0x7F) == 6) {
		printk("[program2] : get SIGABRT signal\n");
	} else if ((wo.wo_stat & 0x7F) == 7) {
		printk("[program2] : get SIGBUS signal\n");
	} else if ((wo.wo_stat & 0x7F) == 8) {
		printk("[program2] : get SIGFPE signal\n");
	} else if ((wo.wo_stat & 0x7F) == 9) {
		printk("[program2] : get SIGKILL signal\n");
	} else if ((wo.wo_stat & 0x7F) == 11) {
		printk("[program2] : get SIGSEGV signal\n");
	} else if ((wo.wo_stat & 0x7F) == 13) {
		printk("[program2] : get SIGPIPE signal\n");
	} else if ((wo.wo_stat & 0x7F) == 14) {
		printk("[program2] : get SIGALRM signal\n");
	} else if ((wo.wo_stat & 0x7F) == 15) {
		printk("[program2] : get SIGTERM signal\n");
	} else if ((wo.wo_stat & 0x7F) == 19) {
		printk("[program2] : get SIGSTOP signal\n");
	}
	printk("[program2] : child process terminated\n");
	if ((wo.wo_stat & 0x7F) == 127) { //SIGSTOP
		printk("[program2] : The return signal is 19");
	} else {
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7F);
	}
	put_pid(wo_pid);
	return;
}

int my_exec(void)
{
	int i;
        i = do_execve(getname_kernel("/tmp/test"), NULL, NULL);
	return 0;
}

// implement fork function
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
	pid_t pid;
	// set default sigaction for current process
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	pid = kernel_clone(&kargs);
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       current->pid);

	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	struct task_struct *task;

        printk("[program2] : Module_init {Chi Xu} {120090595}\n");
	printk("[program2] : module_init create kthread start");

	task = kthread_create(&my_fork, NULL, "vagrant");
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
