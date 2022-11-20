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
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;

	struct waitid_info *wo_info;
	int wo_stat;
	struct rusage *wo_rusage;

	wait_queue_entry_t child_wait;
	int notask_error;
};

extern pid_t kernel_clone(struct kernel_clone_args *kargs);

extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname_kernel(const char *filename);

void my_exec(pid_t cpid)
{
	int temp;
	printk("[program2] : child process\n");

	temp = do_execve(getname_kernel("/home/vagrant/csc3150/program2/test"),
			 NULL, NULL);
	printk("[program2] : child process terminated\n");
}

void my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	struct pid *wo_pid = NULL; //名叫wo_pid的指针指向struct pid类型
	enum pid_type type;
	int a;

	type = PIDTYPE_PID;
	status = 0;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);
	printk("[program2] : do_wait return value is %d\n", a);

	printk("[program2] : Do Fork. The return signal is %d\n", wo.wo_stat);

	put_pid(wo.wo_pid);
	return;
}

//implement fork function
int my_fork(void *argc)
{
	extern pid_t kernel_clone(struct kernel_clone_args * kargs);

	//set default sigaction for current process
	int i;
	struct k_sigaction *k_action =
		&current->sighand->action[0]; // 指向k_sigaction结构体的指针
	pid_t cpid;
	struct kernel_clone_args args = { .flags = SIGCHLD,
					  .child_tid = NULL,
					  .parent_tid = NULL,
					  .exit_signal = SIGCHLD,
					  .stack = (unsigned long)&my_exec,
					  .stack_size = 0,
					  .tls = 0 };

	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */

	cpid = kernel_clone(&args); // return the pid of child process

	printk("[program2] : child process has pid = %d\n", cpid);
	if (cpid == -1) {
		printk("failed kernel clone!");
	} else {
		// int output;
		printk("[program2] : parent process, pid is:%d\n",
		       (*current).pid);
		my_wait(/* wait until child process terminates */
				 cpid); // return the status of child
		// printk("[program2] : child process terminated\n");
		// printk("[program2] : get signal is %d\n", output);
	}
	return 0;
}

static int __init program2_init(void)
{
	static struct task_struct *thread;
	printk("[program2] : Module_init student name:陈飞飏 ID:120090590\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");
	thread = kthread_create(&my_fork, NULL, "NewThread");
	/* create a kernel thread to run my_fork */

	if (!IS_ERR(thread)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(thread);
	}
	// my_fork();
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
