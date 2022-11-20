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

struct wait_opts
{
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;

	struct waitid_info *wo_info;
	int wo_stat;
	struct rusage *wo_rusage;

	wait_queue_entry_t child_wait;
	int notask_error;
};


extern int do_execve(struct filename *filename,
					 const char __user *const __user *__argv,
					 const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);
extern pid_t kernel_clone(struct kernel_clone_args *args);

extern struct filename *getname_kernel(const char *filename);
static struct task_struct *thread_task;

int my_exec(void) {
    int result;
    //const char path[] = "/home/vagrant/csc3150/assignment1/code/program2/test";
	const char path[] = "/tmp/test";
	
    result = do_execve(getname_kernel(path), NULL, NULL);
	//printk("after do_execve, result = %d", result);

    if (!result) {
        return 0;
    }

    //do_exit(result);
	return 0;
}

int my_wait(pid_t pid)
{
	int status;
	int return_status;
	int a;

	// int terminatedStatus;
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	type = PIDTYPE_PID;

	// look up PID from hash table
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED | WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);
	return_status = wo.wo_stat;
	//printk("my wait: return_status is %d", return_status);
	//printk("do_wait return valus is %d", a);

	// decrease the count and free memory
	put_pid(wo_pid);

	return return_status;
}

// implement fork function
int my_fork(void *argc)
{

	// set default sigaction for current process
	int i;
	pid_t pid;
	struct kernel_clone_args kca;

	kca.flags = SIGCHLD;
	kca.exit_signal = SIGCHLD;
	kca.stack = (unsigned long)&my_exec;
	kca.stack_size = 0;
	kca.parent_tid = NULL;
	kca.child_tid = NULL;
	kca.tls = 0;

	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++)
	{
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	

	/* execute a test program in child process */

	pid = kernel_clone(&kca);

	printk("[program2] : The Child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n", (int)current->pid);
	printk("[program2] : child process");

	/* wait until child process terminates */
	// wait and determine return status
	int status;
	int terminate_status;
	//printk("[program2] : Read to wait");
	status = my_wait(pid);
	//printk("[program2] : status value = %d", status);

	if (status == 25600)
	{
		printk("[program2] : child process gets normal termination\n");
		printk("[program2] : The return signal is %d", status);
	}
	else if (status!=25600)
	{
		terminate_status = status & 0x7f;
		switch (status)
		{
		case 1:
			printk("[program2] : Execution failed with hang up signal\n");
			break;

		case 2:
			printk("[program2] : Execution failed with interrupt signal\n");
			break;

		case 131:
			printk("[program2] : Execution failed with quit signal\n");
			break;

		case 132:
			printk("[program2] : Execution failed with illegal signal\n");
			break;

		case 133:
			printk("[program2] : Execution failed with trap signal\n");
			break;

		case 134:
			printk("[program2] : Execution failed with abort signal\n");
			break;

		case 135:
			printk("[program2] : Execution failed with bus signal\n");
			break;

		case 136:
			printk("[program2] : Execution failed with floating point exception signal\n");
			break;

		case 9:
			printk("[program2] : Execution failed with kill signal\n");
			break;


		case 139:
			printk("[program2] : Execution failed with invaild memory reference signal\n");
			break;


		case 13:
			printk("[program2] : Execution failed with pipeline error signal\n");
			break;

		case 14:
			printk("[program2] : Execution failed with alarm signal\n");
			break;

		case 15:
			printk("[program2] : Execution failed with termaniation signal\n");
			break;

		case 4991:
			printk("[program2] : CHILD PROCESS STOPPED\n");
			break;

		default:
			printk("[program2] : CHILD PROCESS CONTINUED\n");
			break;
		}

		printk("[program2] : The return signal is %d", terminate_status);
		printk("[program2] : The return signal is %d", status);
	}

	//something wrong here: if i delete the following if else statment, the kernel_clone
	// will fail for no reason
	else if(status == -5656)
	{
		
	}
	else
	{
		//printk("[program2] : CHILD PROCESS CONTINUED\n");
	}

	do_exit(0);
	return 0;
}

static int __init program2_init(void)
{

	printk("[program2] : Module_init Ting Wang 120090862\n");

	/* write your code here */
	printk("[program2] : module_init create kthread start\n");

	/* create a kernel thread to run my_fork */
	thread_task = kthread_create(&my_fork, NULL, "Mythread");

	// determined whether the task is created sommthly
	if (IS_ERR(thread_task))
	{
		long err = PTR_ERR(thread_task);
		printk("[program2] : module_init errno is %d\n", (int)err);
	}
	else
	{
		printk("[program2] : module_init Kthread starts\n");
		wake_up_process(thread_task);		
	}

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);

// #define	EPERM		 1	/* Operation not permitted */
// #define	ENOENT		 2	/* No such file or directory */
// #define	ESRCH		 3	/* No such process */
// #define	EINTR		 4	/* Interrupted system call */
// #define	EIO		 5	/* I/O error */
// #define	ENXIO		 6	/* No such device or address */
// 。。。。。。。

// #define	EFBIG		27	/* File too large */
// #define	ENOSPC		28	/* No space left on device */
// #define	ESPIPE		29	/* Illegal seek */
// #define	EROFS		30	/* Read-only file system */
// #define	EMLINK		31	/* Too many links */
// #define	EPIPE		32	/* Broken pipe */
// #define	EDOM		33	/* Math argument out of domain of func */
// #define	ERANGE		34	/* Math result not representable */

// # define __user		__attribute__((noderef, address_space(1)))
// # define __kernel	__attribute__((address_space(0)))
// # define __safe		__attribute__((safe))
// # define __force	__attribute__((force))
// # define __nocast	__attribute__((nocast))
// # define __iomem	__attribute__((noderef, address_space(2)))
// # define __must_hold(x)	__attribute__((context(x,1,1)))
// # define __acquires(x)	__attribute__((context(x,0,1)))
// # define __releases(x)	__attribute__((context(x,1,0)))
// # define __acquire(x)	__context__(x,1)
// # define __release(x)	__context__(x,-1)
// # define __cond_lock(x,c)	((c) ? ({ __acquire(x); 1; }) : 0)
// # define __percpu	__attribute__((noderef, address_space(3)))
// # define __rcu		__attribute__((noderef, address_space(4)))
// # define __private	__attribute__((noderef))