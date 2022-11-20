#include <linux/init.h>
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
#include <linux/wait.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;

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
extern struct filename *getname_kernel(const char *filename);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);

int my_exec(void)
{
	//struct filename *fname = getname_kernel("/home/vagrant/csc3150/source/program2/test");
	//const char *const argv[] = {"/home/vagrant/csc3150/source/program2/test", NULL, NULL};
	int exe;
	struct filename *fname;
	printk("[program2] : child process");
	fname = getname_kernel("/tmp/test");
	exe = do_execve(fname, NULL, NULL);
	return 0;
}

void my_wait(pid_t pid)
{
	struct wait_opts wo;
	int status;
	int a;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;

	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WUNTRACED | WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = &status;
	wo.wo_rusage = NULL;
	a = do_wait(&wo);
	switch (wo.wo_stat) {
	case 0:
		printk("[program2] : child process terminated normally\n");
		printk("[program2] : The return signal is 0\n");
		break;
	case 1:
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : child process is hung up\n");
		printk("[program2] : The return signal is 1\n");
		break;
	case 2:
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : child process is interrupted\n");
		printk("[program2] : The return signal is 2\n");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : child process is killed\n");
		printk("[program2] : The return signal is 9\n");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : child process terminated by pipe signal\n");
		printk("[program2] : The return signal is 13\n");
		break;
	case 14:
		printk("[program2] : get SIGALRM signal\n");
		printk("[program2] : child process terminated by alarm signal (wake up call)\n");
		printk("[program2] : The return signal is 14\n");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is 15\n");
		break;
	case 131:
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : child process quited\n");
		printk("[program2] : The return signal is 3\n");
		break;
	case 132:
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : child process terminated because it has illegal instruction\n");
		printk("[program2] : The return signal is 4\n");
		break;
	case 133:
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : child process terminated by trap signal\n");
		printk("[program2] : The return signal is 5\n");
		break;
	case 134:
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process aborted\n");
		printk("[program2] : The return signal is 6\n");
		break;
	case 135:
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : child process terminated because of bus error\n");
		printk("[program2] : The return signal is 7\n");
		break;
	case 136:
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : child process terminated because of floating point error\n");
		printk("[program2] : The return signal is 8\n");
		break;
	case 139:
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : child process terminated because it has segment fault\n");
		printk("[program2] : The return signal is 11\n");
		break;
	case 4991:
		printk("[program2] : get SIGSTOP signal\n");
		printk("[program2] : child process stopped\n");
		printk("[program2] : The return signal is 19\n");
	}
	put_pid(wo_pid);
	return;
}

//implement fork function
int my_fork(void *argc)
{
	//set default sigaction for current process
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
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.exit_signal = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
	};

	/* execute a test program in child process */
	pid = kernel_clone(&args);
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* wait until child process terminates */

	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Zhong Jiale} {121040084}\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");
	/* create a kernel thread to run my_fork */
	task = kthread_create(&my_fork, NULL, "myThread");
	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
