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
#include <linux/init.h>
#include <linux/signal.h>
#include <linux/wait.h>

MODULE_LICENSE("GPL");
// #define __WEXITSTATUS(status) ((  (status) & 0xff00 )  >> 8 )
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

static struct task_struct *test_task;

extern pid_t kernel_thread(int (*threadfn)(void *data), void *arg,
			   unsigned long flags);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *_argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);
extern void __noreturn do_exit(long code);
extern long do_wait(struct wait_opts *wo);

void my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(
		pid); // look up a PID from hash table and return with it's count evaluated

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo); // pid
	// printk("status : %d, stat : %d\n",status , wo.wo_stat);
	if (wo.wo_stat == 4991) {
		printk("[program2] : get SIGSTOP signal\n"); // STOP
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is 19\n");
	} else if (wo.wo_stat == 0) { // normal
		printk("[program2] : child process exit normally\n");
		printk("[program2] : The return signal is 0\n");
	} else if (wo.wo_stat == 136) { // floating
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 8\n");
	} else if (wo.wo_stat == 135) { // bus
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 7\n");
	} else if (wo.wo_stat == 1) { // hang up
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process is hung up\n");
		printk("[program2] : The return signal is 1\n");
	}

	else if (wo.wo_stat == 2) { // interrupt
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : terminal interrupt\n");
		printk("[program2] : The return signal is 2\n");
	}

	else if (wo.wo_stat == 131) { // quit
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : terminal quit\n");
		printk("[program2] : The return signal is 3\n");
	}

	else if (wo.wo_stat == 132) { // illegal instruction
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process has illegal instruction error\n");
		printk("[program2] : The return signal is 4\n");
	}

	else if (wo.wo_stat == 133) { // trap
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process has trap error\n");
		printk("[program2] : The return signal is 5\n");
	}

	else if (wo.wo_stat == 134) { // abort
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process has abort error\n");
		printk("[program2] : The return signal is 6\n");
	} else if (wo.wo_stat == 9) { // kill
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process is killed\n");
		printk("[program2] : The return signal is 9\n");
	}

	else if (wo.wo_stat == 139) { // segmentation
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process has segmentation fault error\n");
		printk("[program2] : The return signal is 11\n");
	}

	else if (wo.wo_stat == 13) { // pipe
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process has pipe error\n");
		printk("[program2] : The return signal is 13\n");
	}

	else if (wo.wo_stat == 14) { // alarm
		printk("[program2] : get SIGALARM signal\n");
		printk("[program2] : child process has alarm error\n");
		printk("[program2] : The return signal is 14\n");
	} else if (wo.wo_stat == 15) { // terminate
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process is terminated\n");
		printk("[program2] : The return signal is 15\n");
	} else {
		printk("[program2] : process continues\n");
		printk("[program2] : process signal is %d \n", wo.wo_stat);
	}

	put_pid(wo_pid);
	return;
}

//implement fork function
int my_exec(void *argc)
{
	int status;
	struct filename *filename = getname_kernel(
		"/home/vagrant/csc3150/ASS2/test"); // original path:/tmp/test
	status = do_execve(filename, NULL, NULL);
	printk("[program2] : child process");
	// printk("status: %d\n",status);
	if (!status) { //check whether there is error
		return 0;
	}
	do_exit(status);
}
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

	/* fork a process using kernel_clone or kernel_thread */
	pid_t pid;
	pid = kernel_thread(&my_exec, NULL, 17);
	/* execute a test program in child process */
	printk("[program2] : The child process has pid =%d\n", pid);
	printk("[program2] : This is the parent process, pid=%d\n",
	       current->pid);
	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}

static int __init program2_init(void)
{
	int err;
	printk("[program2] : module_init {Huang Lei} {120090481}\n");

	/* write your code here */
	printk("[program2] : module_init create kthread starts\n");
	/* create a kernel thread to run my_fork */
	test_task = kthread_create(&my_fork, NULL, "HL'sThread");
	/* check whether the thread is ok*/
	if (IS_ERR(test_task)) {
		printk("Unable to start kthread\n ");
		err = PTR_ERR(test_task);
		test_task = NULL;
		return err;
	}
	printk("[program2] : module_init kthread starts\n");
	wake_up_process(test_task);

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
