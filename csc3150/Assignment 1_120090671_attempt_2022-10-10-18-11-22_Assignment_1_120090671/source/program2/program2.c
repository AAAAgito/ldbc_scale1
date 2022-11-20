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

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern struct filename *getname(const char __user *filename);

extern struct filename *getname_kernel(const char __user *filename);
static struct task_struct *task;

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

extern long do_wait(struct wait_opts *wo);

//implement wait function
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
	wo.wo_flags = WUNTRACED | WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	if ((wo.wo_stat & 0x7f) == 1) {
		printk("[program2] : get SIGHUP signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 2) {
		printk("[program2] : get SIGINT signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 3) {
		printk("[program2] : get SIGQUIT signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 4) {
		printk("[program2] : get SIGILL signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 6) {
		printk("[program2] : get SIGABRT signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 14) {
		printk("[program2] : get SIGALRM signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 7) {
		printk("[program2] : get SIGBUS signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 8) {
		printk("[program2] : get SIGFPE signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 9) {
		printk("[program2] : get SIGKILL signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 13) {
		printk("[program2] : get SIGPIPE signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 11) {
		printk("[program2] : get SIGSEGV signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 127) {
		printk("[program2] : get SIGSTOP signal");
		printk("[program2] : The return signal is 19\n");
	} else if ((wo.wo_stat & 0x7f) == 15) {
		printk("[program2] : get SIGTERM signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 5) {
		printk("[program2] : get SIGTRAP signal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	} else if ((wo.wo_stat & 0x7f) == 0) {
		printk("[program2] : This is the normal");
		printk("[program2] : The return signal is %d\n",
		       wo.wo_stat & 0x7f);
	}

	put_pid(wo_pid);

	return;
}

int my_exec(void)
{
	int exec;
	const char path[] =
		"/home/vagrant/csc3150/template_source/source/program2/test";

	struct filename *my_filename = getname_kernel(path);

	exec = do_execve(my_filename, NULL, NULL);
	//printk("[program2] : child process\n");

	if (!exec) {
		// printk("doexec success");
		// printk("%d", exec);
		return 0;
	} else {
		printk("doexec fail");

		printk("%d", exec);

		do_exit(exec);
	}
}

//implement fork function
int my_fork(void *argc)
{
	//set default sigaction for current process
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};

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
	pid_t pid = kernel_clone(&args);
	/* execute a test program in child process */
	printk("[program2] : The child process has pid = %d", pid);
	printk("[program2] : This is the parent process, pid = %d",
	       current->pid);
	printk("[program2] : child process\n");
	my_exec();
	/* wait until child process terminates */
	my_wait(pid);

	return 0;
}

// int func(void* data) {

// 	int time_count = 0;
// 	  do {
// 		printk(KERN_INFO "thread_function: %d times", ++time_count);

// 	  }while(!kthread_should_stop() && time_count<=30);

// 	  return time_count;
// }

static int __init program2_init(void)
{
	printk("[program2] : module_init {Cong Yuzheng} {120090671}\n");

	printk("[program2] : module_init create kthread start\n");
	/* write your code here */
	task = kthread_create(&my_fork, NULL, "MyThread");
	/* create a kernel thread to run my_fork */
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

module_init(program2_init);
module_exit(program2_exit);