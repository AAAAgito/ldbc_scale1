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

MODULE_LICENSE("GPL");

struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct siginfo __user *wo_info;
	int wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_entry_t child_wait;
	int notask_error;
};

// struct task_struct *kthread_create_on_node(int (*threadfn)(void *data),
//    void *data, int node,
//    const char namefmt[],
//    ...)
// {
// struct task_struct *task;
// va_list args;
//
// va_start(args, namefmt);
// task = __kthread_create_on_node(threadfn, data, node, namefmt, args);
// va_end(args);
//
// return task;
// }

static struct task_struct *task;

// extern pid_t kernel_clone(struct kernel_clone_args *args);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
// extern struct filename *getname(const char __user * filename);
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);

int my_WEXITSTATUS(int status)
{ // do 8-bit-right shifting
	return (((status)&0xff00) >> 8);
}

int my_WTERMSIG(int status)
{
	return (status & 0x7f);
}

int my_WSTOPSIG(int status)
{
	return (my_WEXITSTATUS(status));
}

int my_WIFEXITED(int status)
{
	return (my_WTERMSIG(status) == 0);
}

int my_WIFSTOPPED(int status)
{
	return ((status & 0xff) == 0x7f);
}

signed char my_WIFSIGNALED(int status)
{
	return (((signed char)(((status & 0x7f) + 1) >> 1)) > 0);
}

void my_display(int status)
{
	if (my_WIFEXITED(status)) {
		printk("[program2] : child process gets normal termination\n");
		printk("[program2] : The return signal is %d\n", status);
	} else if (my_WIFSTOPPED(status)) {
		int s_status = my_WSTOPSIG(status);
		printk("[program2] : Child process stops\n");
		if (s_status == 19) {
			printk("[program2] : child process gets SIGSTOP signal\n");
		} else {
			printk("[program2] : child process gets stop signal not 19\n");
		}
		printk("[program2] : The return signal is %d \n", s_status);
	} else if (my_WIFSIGNALED(status)) {
		int s_status = my_WTERMSIG(status);
		printk("[program2] : child process gets EXECUTION FAILED\n");
		printk("[program2] : The return signal is %d \n", s_status);
	} else {
		printk("[program2] : Child process continues.\n");
	}
	// return 0;
}

int my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	int i;
	int a;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(
		pid); //look up a pid from hash table and return with its count evlauted

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	i = do_wait(&wo); //address
	// printk("IN MY WAIT:");
	a = wo.wo_stat;
	// printk("[program2] : i of return val is %d",&i);

	put_pid(wo_pid);
	return a;
}

int my_work(void *argc)
{
	int status;
	const char path[] = "/tmp/test";
	// const char *const argv[] = {path, NULL, NULL};
	// const char *const envp[] = {"HOME=/", "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin", NULL};
	struct filename *my_file = getname_kernel(path);
	printk("[program2] : Child proces\n");
	status = do_execve(my_file, NULL, NULL);
	// printk("============in my work====:file:%s",my_file);
	// printk("============in my work====: do execve return:%d", status);
	if (!status) { //success

		return 0;
	}

	do_exit(status);
}

//implement fork function
int my_fork(void *argc)
{ //to execute the "test.c" program

	//set default sigaction for current process
	int i;
	// int SIGCHILD;
	pid_t child_pid;
	struct k_sigaction *k_action = &current->sighand->action[0];
	int status;
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	child_pid = kernel_thread(&my_work, NULL, 17);

	printk("[program2] : The child process has pid = %d \n", child_pid);
	printk("[program2] : The parent process has pid= %d\n",
	       (int)current->pid);

	status = my_wait(child_pid);
	my_display(status);
	/* wait until child process terminates */

	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : Module_init{Bao Rui} {120090738}\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */
	printk("[program2] : Module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");
	printk("[program2] : Module_init kthread start\n");
	//wake up if no error
	if (!IS_ERR(task)) {
		printk("[program2] : Kthrad starts\n");
		wake_up_process(task);
	}
	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
