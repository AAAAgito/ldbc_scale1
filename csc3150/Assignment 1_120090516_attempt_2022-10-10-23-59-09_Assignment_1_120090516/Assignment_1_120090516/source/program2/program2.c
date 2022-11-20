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
#include <linux/sched/task.h>

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

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char __user *filename);

static struct task_struct *task;
// int status;

int my_exec(void)
{ //child process executes the test program
	const char path[] = "/tmp/test";
	// const char *const argv[] = {path, NULL, NULL};
	// const char *const envp[] = {"HOME=/root", "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin", NULL};
	struct filename *FN = getname_kernel(path);

	printk("[program2] : child process\n");
	int num = do_execve(FN, NULL, NULL);
	// printk("num=%d\n", num);

	if (!num) {
		return 0;
	} else {
		do_exit(num);
		// return 1;
	}
	// return 0;
}
void output(int status)
{
	switch (status) {
	case 0:
		printk("[program2] : child process exit normally\n");
		break;
	case 1:
		printk("[program2] : get SIGHUP signal\n");
		break;
	case 2:
		printk("[program2] : get SIGINT signal\n");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal\n");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal\n");
		break;
	case 14:
		printk("[program2] : get SIGALARM signal\n");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal\n");
		break;
	case 132:
		printk("[program2] : get SIGILL signal\n");
		break;
	case 134:
		printk("[program2] : get SIGABRT signal\n");
		break;
	case 135:
		printk("[program2] : get SIGBUS signal\n");
		break;
	case 136:
		printk("[program2] : get SIGFPE signal\n");
		break;

	default:
		break;
	}
	printk("[program2] : child process terminated\n");
	printk("[program2] : The return signal is %d\n", status);
}
void my_wait(pid_t pid)
{
	struct pid *wo_pid = find_get_pid(pid);
	struct wait_opts wo = {
		.wo_flags = WEXITED | WSTOPPED,
		.wo_rusage = NULL,
		.wo_stat = 0,
		.wo_pid = wo_pid,
		.wo_type = PIDTYPE_PID,
		.wo_info = NULL,
	};

	// wait for child process
	int a;
	a = do_wait(&wo);
	//output the signal from child process
	// printk("a=%d\n",&a);
	output(wo.wo_stat);
	put_pid(wo_pid);
	return;
}

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
	// /* fork a process using kernel_clone or kernel_thread */
	pid_t pid;
	struct kernel_clone_args kca = {
		.flags = SIGCHLD,
		.child_tid = NULL,
		.parent_tid = NULL,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};
	pid = kernel_clone(&kca);
	int ppid = (int)current->pid;
	// printk("test%d", pid);

	// /* execute a test program in child process */
	//Print out the process id for both parent and child process.(5)
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : The parent process has pid = %d\n", ppid);

	/* wait until child process terminates */
	my_wait(pid);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init {Yang Yin} {120090516}\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start");
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
