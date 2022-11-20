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
#include <linux/signal.h>
#include <linux/slab.h>
#include <linux/wait.h>

MODULE_LICENSE("GPL");

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);
extern struct wait_opts {
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
extern void __noreturn do_exit(long code);

// signals information
char *program_signals[] = { "SIGHUP",  "SIGINT",   "SIGQUIT", "SIGILL",
			    "SIGTRAP", "SIGABRT",  "SIGBUS",  "SIGFPE",
			    "SIGKILL", " ",	"SIGSEGV", " ",
			    "SIGPIPE", "SIGALARM", "SIGTERM" };

static struct task_struct *p; // parant process

int program_exit_status(int signal)
{ // WEXITSTATUS
	int res = (signal & 0xff00) >> 8;
	return res;
}

int program_term_by_signal(int signal)
{ // WTERMSIG
	return (signal & 0x7f);
}

int normal_terminate(int signal)
{ // WIFEXITED
	return (program_term_by_signal(signal) == 0);
}

signed char is_term_by_signal(int signal)
{ // WIFSIGNALED
	int tmp = program_term_by_signal(signal);
	tmp += 1;
	tmp = tmp >> 1;
	return ((signed char)tmp > 0);
}

int program_stop_signal(int signal)
{ // WSTOPSIG
	return program_exit_status(signal);
}

int program_stop(int signal)
{ // WIFSTOPPED
	int res = signal & 0xff;
	return (res == 0x7f);
}

int my_exec(void)
{
	printk("[program2] : child process\n");
	int indicator; // indicating whether the test program executes successfully
	const char path_name[] = "/tmp/test";
	struct filename *file_name_struct;
	file_name_struct = getname_kernel(path_name);
	indicator = do_execve(file_name_struct, NULL, NULL);
	return 0;
}

// implement fork function
int my_fork(void *argc)
{
	int a; // return value of do_wait
	pid_t child_id;
	struct kernel_clone_args arg = {
		.flags = SIGCHLD,
		.exit_signal = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
	};

	int i;

	// set default sigaction for current process
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */

	child_id = kernel_clone(&arg);
	/* execute a test program in child process */
	printk("[program2] : The child process has pid = %d\n", (int)child_id);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);
	/* wait until child process terminates */

	int status; // status of child process
	struct pid *wo_pid = NULL;
	enum pid_type type;

	type = PIDTYPE_PID;
	wo_pid = find_get_pid(child_id);
	struct wait_opts wo = {
		.wo_type = type,
		.wo_pid = wo_pid,
		.wo_flags = WEXITED | WUNTRACED,
		.wo_info = NULL,
		.wo_stat = status,
		.wo_rusage = NULL,
	};
	a = do_wait(&wo);

	int signal_number;
	int exit_status = wo.wo_stat;
	if (program_stop(exit_status)) {
		signal_number = program_stop_signal(exit_status);
		printk("[program2] : CHILD PROCESS STOPPED\n");
		if (signal_number == 19) {
			printk("[program2] : get SIGSTOP signal\n");
		}
		printk("[program2] : the return signal is %d\n", signal_number);
	} else if (normal_terminate(exit_status)) {
		printk("[program2] : child process terminated\n");
		signal_number = program_exit_status(exit_status);
		printk("[program2] : normal termination with EXIT STATUS = %d\n",
		       signal_number);
	} else if (is_term_by_signal(exit_status)) {
		signal_number = program_term_by_signal(exit_status);
		if (signal_number <= 15 &&
		    program_signals[signal_number] != " ") {
			printk("[program2] : get %s signal\n",
			       program_signals[signal_number - 1]);
		} else {
			printk("[program2] : child process receives a signal that is not in the "
			       "signal list\n");
		}
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d.\n",
		       signal_number);
	} else {
		printk("[program2] : child process continues\n");
	}

	do_exit(0);

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init Qiu Weilun 120090771\n");
	printk("[program2] : Module_init create kthread start\n");

	/* write your code here */
	/* create a kernel thread to run my_fork */
	p = kthread_create(&my_fork, NULL, "mythread");

	if (!IS_ERR(p)) {
		printk("[program2] : Module_init kthread starts\n");
		wake_up_process(p);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
