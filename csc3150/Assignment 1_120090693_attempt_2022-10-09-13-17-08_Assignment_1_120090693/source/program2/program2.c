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

struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct siginfo __user *wo_info;
	int __user wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};

extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname_kernel(const char *filename);

extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

int my_exec(void) {
	int result;
	const char path[] = "/tmp/test";
	struct filename *file_name = getname_kernel(path);
	result = do_execve(file_name, NULL, NULL);
	if (result)
		do_exit(result);
	return 0;
}

void my_wait(pid_t pid) {

	int status, a;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = (int __user)status;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);
	switch (wo.wo_stat) {
	case 0:
		printk("[program2] : get NORMAL signal");
		printk("[program2] : child process normally exited");
		break;
	case 1:
		printk("[program2] : get SIGHUP signal");
		printk("[program2] : child process was hanged up");
		break;
	case 2:
		printk("[program2] : get SIGINT signal");
		printk("[program2] : child process was interrupted");
		break;
	case 9:
		printk("[program2] : get SIGKILL signal");
		printk("[program2] : child process was killed");
		break;
	case 13:
		printk("[program2] : get SIGPIPE signal");
		printk("[program2] : child process got pipe error");
		break;
	case 14:
		printk("[program2] : get SIGALARM signal");
		printk("[program2] : child process got alarm error");
		break;
	case 15:
		printk("[program2] : get SIGTERM signal");
		printk("[program2] : child process terminated");
		break;
	case 131:
		printk("[program2] : get SIGQUIT signal");
		printk("[program2] : child process quit");
		break;
	case 132:
		printk("[program2] : get SIGILL signal");
		printk("[program2] : child process got illegal instruction");
		break;
	case 133:
		printk("[program2] : get SIGTRAP signal");
		printk("[program2] : child process was trapped");
		break;
	case 134:
		printk("[program2] : get SIGABRT signal");
		printk("[program2] : child process got abort error");
		break;
	case 135:
		printk("[program2] : get SIGBUS signal");
		printk("[program2] : child process got bus error");
		break;
	case 136:
		printk("[program2] : get SIGFPE signal");
		printk("[program2] : child process got float error");
		break;
	case 139:
		printk("[program2] : get SIGSEGV signal");
		printk("[program2] : child process got segmentation fault error");
		break;
	case 4991:
		printk("[program2] : get SIGSTOP signal");
		printk("[program2] : child process stoped");
		break;
	default:
		printk("[program2] : get ???? signal");
		printk("[program2] : child process ????");
		break;
	}

	if (wo.wo_stat < 128) {
		printk("[program2] : The return signal is %d", wo.wo_stat);
	} else if (wo.wo_stat != 4991) {
		printk("[program2] : The return signal is %d", wo.wo_stat - 128);
	} else {
		printk("[program2] : The return signal is %d", 19);
	}
	put_pid(wo_pid);
	return;
}

// implement fork function
int my_fork(void *argc) {
	pid_t pid;

	// set default sigaction for current process
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
	/* execute a test program in child process */
	pid = kernel_thread((void *)&my_exec, 0, SIGCHLD);

	printk("[program2] : The child process has pid = %d", pid);
	printk("[program2] : This is the parent process, pid = %d", current->pid);
	printk("[program2] : child process");
	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}

static int __init program2_init(void) {
	struct task_struct *task;
	printk("[program2] : module_init {Zhang Lingran} {120090693}");

	/* write your code here */
	printk("[program2] : module_init create kthread start");

	/* create a kernel thread to run my_fork */

	task = kthread_create(&my_fork, NULL, "MyThread");
	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void) {
	printk("[program2] : module_exit./my");
	return;
}

module_init(program2_init);
module_exit(program2_exit);
