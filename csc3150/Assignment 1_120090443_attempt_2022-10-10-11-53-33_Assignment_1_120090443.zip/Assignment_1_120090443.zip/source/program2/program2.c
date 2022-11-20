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

static struct task_struct *task;

// extern long _do_fork(
// 	unsigned long clone_flags,
// 	unsigned long stack_start,
// 	unsigned long stack_size,
// 	int __user *parent_tidptr,
// 	int __user *child_tidptr,
// 	unsigned long tls);
// extern struct kernel_clone_args {
// 	u64 flags;
// 	int __user *pidfd;
// 	int __user *child_tid;
// 	int __user *parent_tid;
// 	int exit_signal;
// 	unsigned long stack;
// 	unsigned long stack_size;
// 	unsigned long tls;
// 	pid_t *set_tid;
// 	/* Number of elements in *set_tid */
// 	size_t set_tid_size;
// 	int cgroup;
// 	struct cgroup *cgrp;
// 	struct css_set *cset;
// };
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
// extern struct filename *getname(const char __user * filename);
extern struct filename *getname_kernel(const char *filename);
extern void __noreturn do_exit(long code);
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
extern long do_wait(struct wait_opts *wo);

void my_wait(pid_t pid); //wait for the child process to terminate
int my_exec(void); //execute the test
int my_fork(void *argc); //fork a process and wait

//implement exec function (tutorial)
int my_exec(void)
{
	int result;
	const char path[] = "/tmp/test";

	// struct filename * my_filename = getname(path);
	struct filename *my_filename = getname_kernel(path);
	result = do_execve(my_filename, NULL, NULL);

	//if exec success
	if (!result) {
		return 0;
	}
	do_exit(result);
}

//implement wait function (tutorial + print out child process's signal)
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
	wo.wo_stat = (int __user)status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	//output child process exit status
	if (wo.wo_stat == 4991) {
		printk("[Program2] : get SIGSTOP signal\n");
		printk("[Program2] : child process stopped\n");
		wo.wo_stat = 19;
	} else {
		if (wo.wo_stat > 128) {
			wo.wo_stat = wo.wo_stat - 128;
		}
		switch (wo.wo_stat) {
		case 0: //Normal termination
			printk("[Program2] : Normal termination\n");
			break;
		case 1: //SIGHUB
			printk("[Program2] : get SIGHUB signal\n");
			printk("[Program2] : child process hangup\n");
			break;
		case 2: //SIGINT
			printk("[Program2] : get SIGINT signal\n");
			printk("[Program2] : child process is interrupted\n");
			break;
		case 3: //SIGQUIT
			printk("[Program2] : get SIGQUIT signal\n");
			printk("[Program2] : child process quited\n");
			break;
		case 4: //SIGILL
			printk("[Program2] : get SIGILL signal\n");
			printk("[Program2] : child process has illegal instructions\n");
			break;
		case 5: //SIGTRAP
			printk("[Program2] : get SIGTRAP signal\n");
			printk("[Program2] : child process is trapped\n");
			break;
		case 6: //SIGABRT
			printk("[Program2] : get SIGABRT signal\n");
			printk("[Program2] : child process terminated\n");
			break;
		case 7: //SIGBUS
			printk("[Program2] : get SIGBUS signal\n");
			printk("[Program2] : child process has bus error\n");
			break;
		case 8: //SIGFPE
			printk("[Program2] : get SIGFPE signal\n");
			printk("[Program2] : child process has floating-point exception\n");
			break;
		case 9: //SIGKILL
			printk("[Program2] : get SIGKILL signal\n");
			printk("[Program2] : child process terminated\n");
			break;
		case 11: //SIGSEGV
			printk("[Program2] : get SIGSEGV signal\n");
			printk("[Program2] : child process has segmentation fault\n");
			break;
		case 13: //SIGPIPE
			printk("[Program2] : get SIGPIPE signal\n");
			printk("[Program2] : child process has broken pipe\n");
			break;
		case 14: //SIGALRM
			printk("[Program2] : get SIGALRM signal\n");
			printk("[Program2] : child process has timer signal from alarm clock\n");
			break;
		case 15: //SIGTERM
			printk("[Program2] : get SIGTERM signal\n");
			printk("[Program2] : child process terminated\n");
			break;
		case 16: //SIGSTKFLT
			printk("[Program2] : get SISTKFLT signal\n");
			printk("[Program2] : child process has stack error\n");
			break;
		}
	}

	printk("[Program2] : The return signal is %d\n", wo.wo_stat);
	put_pid(wo_pid);
	return;
}

//implement fork function
int my_fork(void *argc)
{
	//init pid
	// long pid;
	pid_t pid;

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

	/* fork a process using do_fork */
	struct kernel_clone_args kargs = { .flags = SIGCHLD,
					   .stack = (unsigned long)&my_exec,
					   .stack_size = 0,
					   .parent_tid = NULL,
					   .child_tid = NULL,
					   .tls = 0,
					   .exit_signal = SIGCHLD };

	pid = kernel_clone(&kargs);

	// pid = _do_fork(SIGCHLD, (unsigned long)&my_exe, 0, NULL, NULL, 0);

	printk("[Program2] : The child process has pid = %ld\n", pid);
	printk("[Program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	printk("[Program2] : child process\n");
	/* wait until child process terminates */
	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {ZhouYuxiao} {120090443}\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");

	/* create a kthread */
	task = kthread_create(&my_fork, NULL, "my_kernel_thread");

	//wake up new thread if ok
	if (!IS_ERR(task)) {
		printk("[Program2] : module_init kthread starts\n");
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
