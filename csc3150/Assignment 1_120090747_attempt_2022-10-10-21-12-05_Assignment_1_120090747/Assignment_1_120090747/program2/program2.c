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

extern struct kernel_clone_args;
extern struct wait_opts;

// implement fork function
static int my_fork(void *argc)
{
	// set default sigaction for current process
	int i;
	struct kernel_clone_args args = { .flags = SIGCHLD,
					  .exit_signal = SIGCHLD };
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
	if (pid == 0) {
		printk("[program2] : The child process has pid = %d\n",
		       getpid());
		/* execute a test program in child process */
		do_execve(getname_kernel("/tmp/test"));
	}
	/* wait until child process terminates */
	else {
		enum pid_type type;
		int status, child_sig;
		struct wait_opts wo = { .wo_type = type,
					.wo_flags = WEXITED,
					.wo_pid = find_get_pid(pid),
					.wo_info = NULL,
					.wo_stat = (int __user)status,
					.wo_rusage = NULL };
		printk("[program2] : This is the parent process, pid = %d\n",
		       getpid());
		child_sig = do_wait(&wo);
		printk("Child process terminated\n");
		printk("The return signal is %d\n", child_sig);
	}
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init Qingyuan Chen 120090747\n");

	/* write your code here */
	printk("[program2] : Module_init create kthread start\n");
	/* create a kernel thread to run my_fork */
	kthread_run(my_fork, NULL, "[program2] : ");
	printk("[program2] : Module_init kthread start\n");
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
