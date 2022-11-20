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
#include <linux/signal.h>

MODULE_LICENSE("GPL");

struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct siginfo __user *wo_info;
	int wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};

static struct task_struct *task;

//external functions
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);
extern pid_t kernel_clone(struct kernel_clone_args *args);

//implement wait function
void my_wait(pid_t pid)
{
	int status = 0;

	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(
		pid); //look up a PID from hash table and retunr with it's count evaluated

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = status; // exit code
	wo.wo_rusage = NULL;

	int a;

	a = do_wait(&wo);

	int stat = wo.wo_stat & 0x7f;
	if (stat == 0x7f) {
		stat = (wo.wo_stat & 0xff00) >> 8;
	}

	if (stat == 0) {
		printk("[program2] : child process exit normally\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 1) {
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process hung up\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 2) {
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : terminal interrupt\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 3) {
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : terminal quit\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 4) {
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : illegal instruction\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 5) {
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : trap error\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 6) {
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process abort\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 7) {
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : bus error\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 8) {
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : floating point exception\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 9) {
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : kill process\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 11) {
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : invalid memory reference\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 13) {
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : broken pipe\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 14) {
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process timer error\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 15) {
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else if (stat == 19) {
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is %d \n", stat);
	} else {
		printk("[program2] : process got an unkown signal");
		printk("[program2] : process continues\n");
	}
	put_pid(wo_pid); // decrease the count and free memory
}

//execute test.c
int my_execve(void)
{
	int result;
	// const char path[] =
	// 	"/home/vagrant/csc3150/Assignment_1_120090391/source/program2/test";
	const char path[] = "/tmp/test";
	struct filename *my_filename = getname_kernel(path);
	printk("[program2] : child process");
	result = do_execve(my_filename, NULL, NULL);

	if (!result) {
		return 0;
	} else {
		do_exit(result);
	}
}

struct kernel_clone_args args = {
	.flags = SIGCHLD,
	.stack = &my_execve,
	.stack_size = 0,
	.parent_tid = NULL,
	.child_tid = NULL,
	.tls = 0,
	.exit_signal = SIGCHLD,
};

//implement fork function
int my_fork(void *argc)
{
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
	pid_t pid;

	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_clone(&args);

	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	/* execute a test program in child process */

	/* wait until child process terminates */

	my_wait(pid);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Zhang Qiyue} {120090391}\n");
	printk("[program2] : Module_init create kthread start\n");

	/* write your code here */
	task = kthread_create(&my_fork, NULL, "MyThread");

	if (!IS_ERR(task)) {
		printk("[program2] : Module_init kthread start\n");
		wake_up_process(task);
	}
	/* create a kernel thread to run my_fork */

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
