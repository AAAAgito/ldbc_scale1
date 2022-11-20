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

extern struct filename *getname_kernel(const char *);
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);

int my_exec(void);
void my_wait(pid_t pid);
int my_fork(void *argc);
void printout_result(int i);

struct kernel_clone_args task = {
	.flags = SIGCHLD,
	.stack = (unsigned long)&my_exec,
	.stack_size = 0,
	.parent_tid = NULL,
	.child_tid = NULL,
	.tls = 0,
	.exit_signal = SIGCHLD,
};

pid_t taskid;
struct task_struct *thread;

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
	/* fork a process using kernel_clone or kernel_thread */
	/* execute a test program in child process */
	taskid = kernel_clone(&task);

	/* wait until child process terminates */
	printk("[program2] : The Child pid has pid = %d\n", taskid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       current->pid);
	my_wait(taskid);

	return (0);
}

int my_exec(void)
{
	int res;
	printk("[program2] : child process");
	res = do_execve(getname_kernel("/tmp/test"), NULL, NULL);
	if (!res) {
		return 0;
	}
	do_exit(res);
}

int signal;

int status;
void my_wait(pid_t pid)
{
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
	do_wait(&wo);
	signal = (wo.wo_stat & 0x7f);
	if (signal == 127) {
		signal = 19;
	}
	printout_result(signal);
	printk("[program2] : child process terminated\n");
	printk("[program2] : The return signal is %d\n", signal);
	put_pid(wo_pid);
	return;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Luohao} {120090712}\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");
	/* create a kernel thread to run my_fork */
	thread = kthread_create(&my_fork, NULL, "NewThread");
	if (!IS_ERR(thread)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(thread);
	}
	return 0;
}

void printout_result(int i)
{
	switch (i) {
	case 0:
		printk("[program2] : get SIGCHLD signal\n");
		break;
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
		printk("[program2] : get SIGQILL signal\n");
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
	case 19:
		printk("[program2] : get SIGSTOP signal\n");
		break;
	case 126:
		printk("[program2] : Child process cannot open the file\n");
		break;
	default:
		printk("[program2] : The child process terminates in an unknown way, siganl is: %d\n",
		       i);
		break;
	}
	return;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
