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

struct wait_opts { // get from kernel
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct waitid_info *wo_info;
	int wo_stat;
	struct rusage *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};

static struct task_struct *task;

// external functions
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
struct filename *getname_kernel(const char *filename);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

// wait fucntion
void my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;

	int a;

	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WUNTRACED | WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	// int a;
	a = do_wait(&wo);

	if ((wo.wo_stat & 0x7f) == 0) {
		printk("[program2] : child process exit normally \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 1) {
		printk("[program2] : get SIGHUP signal \n");
		printk("[program2] : child process hung up \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 2) {
		printk("[program2] : get SIGINT signal \n");
		printk("[program2] : child process terminal interrupt \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 3) {
		printk("[program2] : get SIGQUIT signal \n");
		printk("[program2] : child process terminal quit \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 4) {
		printk("[program2] : get SIGILL signal \n");
		printk("[program2] : child process illegal instruction error \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 5) {
		printk("[program2] : get SIGTRAP signal \n");
		printk("[program2] : child process trap error \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));
		// printk("[program2] : The return signal is 5 \n");
	} else if ((wo.wo_stat & 0x7f) == 6) {
		printk("[program2] : get SIGABRT signal \n");
		printk("[program2] : child process abort error \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

		// printk("[program2] : The return signal is 6 \n");
	} else if ((wo.wo_stat & 0x7f) == 7) {
		printk("[program2] : get SIGBUS signal \n");
		printk("[program2] : child process bus error \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 8) {
		printk("[program2] : get SIGFPE signal \n");
		printk("[program2] : child process float error \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 9) {
		printk("[program2] : get SIGKILL signal \n");
		printk("[program2] : child process killed \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 11) {
		printk("[program2] : get SIGSEGV signal \n");
		printk("[program2] : child process segmentation fault error \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 13) {
		printk("[program2] : get SIGPIPE signal \n");
		printk("[program2] : child process pipe error \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 14) {
		printk("[program2] : get SIGALARM signal \n");
		printk("[program2] : child process alarm error \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if ((wo.wo_stat & 0x7f) == 15) {
		printk("[program2] : get SIGTERM signal \n");
		printk("[program2] : child process terminated \n");
		printk("[program2] : The return signal is %d \n",
		       (wo.wo_stat & 0x7f));

	} else if (((wo.wo_stat >> 8) & 0x7f) == 0x7f) {
		printk("[program2] : get SIGSTOP signal \n");
		printk("[program2] : child process stopped \n");
		// printk("[program2] : The return signal is %d \n",(wo.wo_stat & 0x7f));

	} else {
		printk("[program2] : child process continued \n");
		// printk("[program2] : The return signal is %d \n",(wo.wo_stat & 0x7f));
	}
	// printk("[program2] : The return signal is %d \n",(wo.wo_stat & 0x7f));

	put_pid(wo_pid);
}

int my_exec(void *data)
{
	int exec_result;
	const char *path = "/tmp/test";
	// "/home/vagrant/csc3150/assignment1/src/program2/test"; // need to change
	// this path
	struct filename *my_filename = getname_kernel(path);

	printk("[program2] : child process");

	exec_result = do_execve(my_filename, NULL, NULL);

	if (exec_result == 0) {
		return 0;
	} // successfully execute
	do_exit(exec_result); // error situation
}

// implement fork function
int my_fork(void *argc)
{
	// set default sigaction for current process
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
	pid = kernel_thread(&my_exec, 0, SIGCHLD);

	/* execute a test program in child process */
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : The parent process has pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */
	my_wait(pid);

	// do_exit(0);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Lyu Minen} {120090828} \n"); // modified

	/* write your code here */

	/* create a kernel thread to run my_fork */
	printk("[program2] : Module_init create kthread start \n");
	task = kthread_create(&my_fork, NULL, "MyThread");

	// wake up
	if (!IS_ERR(task)) {
		printk("[program2] : Module_init kthread start \n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit \n");
}

module_init(program2_init);
module_exit(program2_exit);
