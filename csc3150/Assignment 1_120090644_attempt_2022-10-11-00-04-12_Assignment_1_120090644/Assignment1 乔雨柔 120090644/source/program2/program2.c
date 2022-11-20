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
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
extern int do_execve(struct filename *filename,
					 const char __user *const __user *__argv,
					 const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern getname_kernel();
MODULE_LICENSE("GPL");
void my_exec(struct filename *filename)
{
	printk("[program2] : module_init kthread start");
	do_execve(filename, NULL, NULL);
}
void my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = (int _user *)&status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	printk("[program2] : get %d\n signal", *wo.wo_stat);
	printk("[program2] : child process terminated");
	printk("[program2] : The return signal is %d\n", &a);
	put_pid(wo_pid);

	return;
}

// implement fork function
int my_fork(void *argc)
{

	// set default sigaction for current process
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++)
	{
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.stack = &my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
	} pid_t cp = kernel_clone(args);
	printk("[program2] : The child process has pid = %d\n", cp);
	/* execute a test program in child process */

	my_exec(getname_kernel("/tmp/test"));
	printk("[program2] : child process");

	/* wait until child process terminates */
	my_wait(cp);
	return 0;
}

static int __init program2_init(void)
{
	/* write your code here */
	printk("[program2] : Module_init {乔雨柔} {120090644}\n");
	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	printk("[program2] : module_init kthread start\n");
	pid_t p = kernel_thread(my_fork, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGHAND);
	printk("[program2] : This is the parent process, pid = %d\n", p);

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
