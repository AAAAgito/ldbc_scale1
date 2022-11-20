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

// struct kernel_clone_args {
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

static struct task_struct *task;
extern long do_wait(struct wait_opts *wo);
//  first define wait_opts, then da_wait
// extern long do_wait(struct wait_opts *wo); //this is static
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
// extern struct filename { // the code of the linux
// 	const char	*name;
// 	const __user char	*uptr;
// 	int			refcnt;
// 	struct audit_names	*aname;
// 	const char		iname[];
// };

extern struct filename *getname_kernel(const char *);

// extern static int do_exit(void);
extern signed char my_WIFSIGNALED(int status);

static char *signals[];

// void my_wait(pid_t pid);
// int my_fork(void *argc);

// #define	__WEXITSTATUS(status)	(((status) & 0xff00) >> 8)
// #define	__WTERMSIG(status)	((status) & 0x7f)
// #define	__WSTOPSIG(status)	__WEXITSTATUS(status)
// #define	__WIFEXITED(status)	(__WTERMSIG(status) == 0)
//   (((signed char) (((status) & 0x7f) + 1) >> 1) > 0)
// #define	__WIFSTOPPED(status)	(((status) & 0xff) == 0x7f)
// # define __WIFCONTINUED(status)	((status) == __W_CONTINUED)
// #define __W_CONTINUED		0xffff

// int my_WEXITSTATUS(int status){
// 	return (((status) & 0xff00) >> 8);
// }

// int my_WTERMSIG(int status){
// 	return ((status) & 0x7f);
// }

// int my_WSTOPSIG(int status){
// 	return (my_WEXITSTATUS(status));
// }

// int my_WIFEXITED(int status){
// 	return (my_WTERMSIG(status)==0);
// }

// signed char my_WIFSIGNALED(int status){
// 	return (((signed char) (((status) & 0x7f) + 1) >> 1) > 0);
// }

// int my_WIFSTOPPED(int status){
// 	return (((status) & 0xff) == 0x7f);
// }
// int my_WCOREDUMP(int status){
// 	return ((status) & 0xffff);
// }
void check(int status)
{
	if (status == 134) {
		printk("[program2] : get SIGABRT signal\n");
	} else if (status == 14) {
		printk("[program2] : get SIGALRM signal\n");
	} else if (status == 135) {
		printk("[program2] : get SIGBUS signal\n");
	} else if (status == 136) {
		printk("[program2] : get SIGFPE signal\n");
	} else if (status == 1) {
		printk("[program2] : get SIGHUP signal\n");
	} else if (status == 132) {
		printk("[program2] : get SIGILL signal\n");
	} else if (status == 2) {
		printk("[program2] : get SIGINT signal\n");
	} else if (status == 9) {
		printk("[program2] : get SIGKILL signal\n");
	} else if (status == 0) {
		printk("[program2] : This is the normal process\n");
	} else if (status == 13) {
		printk("[program2] : get SIGPIPE signal\n");
	} else if (status == 131) {
		printk("[program2] : get SIGQUIT signal\n");
	} else if (status == 139) {
		printk("[program2] : get SIGSEGV signal\n");
	} else if (status == 15) {
		printk("[program2] : get SIGTERM signal\n");
	} else if (status == 133) {
		printk("[program2] : get SIGTRAP signal\n");
	} else if (status == 4991) {
		printk("[program2] : get SIGSTOP signal\n");
	}
	return;
}

int my_execve(void)
{
	int flag;
	int a = 0;
	flag = do_execve(getname_kernel("/tmp/test"), NULL, NULL);

	if (a == 0) {
		// printk("[program2] : child process\n");
	}

	if (!flag) {
		return 0;
	}

	do_exit(flag);
}

// //implement wait function
int my_wait(pid_t pid)
{
	int status;
	int wo_stat;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);
	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WUNTRACED | WEXITED;
	wo.wo_info = NULL;
	// wo.wo_stat = status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);
	// printk("do_wait return value is %d\n",&a);
	status = wo.wo_stat;
	// output child process exit status
	int print_value = status & 0x7f;
	// printk("[program2] : The return signal is %d\n", print_value);
	put_pid(wo_pid);

	return status;
}

struct kernel_clone_args para = {
	// This the parameter settings
	.flags = SIGCHLD,
	.child_tid = NULL,
	.parent_tid = NULL,
	.stack =
		(unsigned long)&my_execve, // when finish my_execve, it will return
	// a int
	.stack_size = 0,
	.exit_signal = SIGCHLD,
};

// implement fork function
int my_fork(void *argc)
{
	// set default sigaction for current process
	int i;
	int a = 0;
	pid_t pid;
	int status;
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
	pid = kernel_clone(
		&para); // new one process, test program has been execute at there
	printk("[program2] : module_init kthread start\n");
	/* wait until child process terminates */
	status = my_wait(pid);
	// printk("before ppid status and after my_wait\n");
	int ppid = (int)current->pid;
	printk("[program2] : The child process pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n", ppid);
	if (a == 0) {
		printk("[program2] : child process\n");
	}
	//////////////////////////////

	check(status);

	//////////////////////////////

	printk("[program2] : child process terminated\n");

	int print_value = status & 0x7f;
	printk("[program2] : The return signal is %d\n", print_value);

	// NORMAL
	// if (my_WIFEXITED(status)){
	// 	printk("[program2] : Child process terminated\n");
	// 	printk("[program2] : The return signal is %d\n",
	// my_WEXITSTATUS(status));

	// }

	// //signaled abort
	// else if (my_WIFSIGNALED(status)){
	// 	printk("[program2] : CHILD PROCESS FAILED \n");
	// 	printk("[program2] : The return signal is %d\n", my_WTERMSIG(status));
	// }

	// //stopped
	// else if (my_WIFSTOPPED(status)){
	// 	printk("[program2] : CHILD PROCESS STOPPED \n");
	// 	printk("[program2] : The return signal is %d\n", my_WSTOPSIG(status));
	// }
	// else{
	// 	printk("[program2] : CHILD PROCESS CONTINUED\n");
	// }

	do_exit(0);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Xu Zijian} {120090620}\n"); // can't use printf

	/* write your code here */

	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "program2_thread");

	// wake up new thread if ok
	if (!IS_ERR(task)) {
		printk("[program2] : module_init create kthread start\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

// special macro, tell the kernel what the init and exit function that we define

module_init(program2_init);
module_exit(program2_exit);
