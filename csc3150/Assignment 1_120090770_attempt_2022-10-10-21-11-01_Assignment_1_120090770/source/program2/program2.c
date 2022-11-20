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

const char __user *path = "/tmp/test";
struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct waitid_info __user *wo_info;
	int wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};

static struct task_struct *test_thread;
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern struct filename *getname_kernel(const char __user *filename);
extern long do_wait(struct wait_opts *wo);

int my_exec(void *argc)
{
	int result;

	printk("[program2] : Child process\n");
	result = do_execve(getname_kernel(path), NULL, NULL);
	// printk("result: %d",result); //debug
	if (!result) {
		return 0;
	}
	return 0;
}

void my_wait(pid_t pid)
{
	int status;
	int b;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	long tmp;

	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;
	tmp = do_wait(&wo);
	b = wo.wo_stat & 127;
	switch (b) {
	case 1:
		printk("[program2] : get SIGHUP signal\n");
		break;
	case 2:
		printk("[program2] : get SIGINT signal\n");
		break;
	case 3:
		printk("[program2] : get SIGQUIT signal\n");
		break;
	case 4:
		printk("[program2] : get SIGILL signal\n");
		break;
	case 5:
		printk("[program2] : get SIGTRAP signal\n");
		break;
	case 6:
		printk("[program2] : get SIGABRT signal\n");
		break;
	case 7:
		printk("[program2] : get SIGBUS signal\n");
		break;
	case 8:
		printk("[program2] : get SIGFPE signal\n");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal\n");
		break;
	case 11:
		printk("[program2] : get SIGSEGV signal\n");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal\n");
		break;
	case 14:
		printk("[program2] : get SIGALRM signal\n");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal\n");
		break;
	case 127:
		printk("[program2] : get SIGSTOP signal\n");
		break;
	default:
		printk("[program2] : Not receive signal\n");
		break;
	}
	printk("[program2] : Child process terminated");
	if (b == 127) {
		printk("[program2] : The return signal is 19\n");
	} else {
		printk("[program2] : The return signal is %d\n", b);
	}
	put_pid(wo.wo_pid);
	return;
}

int my_fork(void *argc)
{
	// set default sigaction for current process
	int i;
	pid_t pid;
	int status;
	struct k_sigaction *k_action = &current->sighand->action[0];
	struct kernel_clone_args ker = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};

	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	};
	/* fork a process using kernel_clone or kernel_thread */

	pid = kernel_clone(&ker);

	printk("[program2] : The child process has pid= %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	my_wait(pid);
	// /* execute a test program in child process */

	/* wait until child process terminates */

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {LuoKaicheng} {120090770}\n");
	printk("[program2] : Module_init create kthread start\n");
	/* write your code here */
	test_thread = kthread_create(&my_fork, NULL, "KernelThread");

	if (!IS_ERR(test_thread)) {
		printk("[program2] : Module_init kthread starts\n");
		wake_up_process(test_thread);
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
