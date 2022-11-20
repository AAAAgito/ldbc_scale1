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
	enum pid_type	wo_type;
	int				wo_flags;
	struct pid		*wo_pid;
	struct waitid_info	__user*wo_info;
	int				wo_stat;
	struct rusage	__user*wo_rusage;
	wait_queue_entry_t	child_wait;
	int				notask_error;
};

extern long do_wait(struct wait_opts*wo);

extern struct filename *getname_kenel(const char __user*filename);

extern int do_execve(struct filename *filename,
				const char __user *const __user *__argv,
				const char __user *const __user *__envp);

extern pid_t kernel_clone(struct kernel_clone_args *args);

static struct task_struct* task;



int my_exec(void)
{
	printk("[program2] : child process\n");
	do_execve(getname_kernel("/tmp/test"),NULL,NULL);
	return 0 ;
}



void my_wait(pid_t pid)
{
	int status;
	struct wait_opts wo;
	struct pid *wo_pid =NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_flags = WEXITED|WUNTRACED;
	wo.wo_pid = wo_pid;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	if(wo.wo_stat == 4991)
	{
		printk("[program2] : get SIGSTOP signal");
	}
	else if(wo.wo_stat == 131)
	{
		printk("[program2] : get SIGQUIT signal");
	}
	else if(wo.wo_stat == 135)
	{
		printk("[program2] : get SIGBUS signal");
	}
	else if(wo.wo_stat == 132)
	{
		printk("[program2] : get SIGILL signal");
	}
	else if(wo.wo_stat == 133)
	{
		printk("[program2] : get SIGTRAP signal");
	}
	else if(wo.wo_stat == 136)
	{
		printk("[program2] : get SIGFPE signal");
	}
	else if(wo.wo_stat == 139)
	{
		printk("[program2] : get SIGSEGV signal");
	}
	else if(wo.wo_stat == 134)
	{
		printk("[program2] : get SIGABRT signal");
	}
	else if(wo.wo_stat == 1)
	{
		printk("[program2] : get SIGHUP signal");
	}
	else if(wo.wo_stat == 2)
	{
		printk("[program2] : get SIGINT signal");
	}
	else if(wo.wo_stat == 9)
	{
		printk("[program2] : get SIGKILL signal");
	}
	else if(wo.wo_stat == 10)
	{
		printk("[program2] : get SIGUSR1 signal");
	}
	else if(wo.wo_stat == 12)
	{
		printk("[program2] : get SIGUSR2 signal");
	}
	else if(wo.wo_stat == 13)
	{
		printk("[program2] : get SIGPIPE signal");
	}
	else if(wo.wo_stat == 14)
	{
		printk("[program2] : get SIGALRM signal");
	}
	else if(wo.wo_stat == 15)
	{
		printk("[program2] : get SIGTERM signal");
	}
	else if(wo.wo_stat == 16)
	{
		printk("[program2] : get SIGSTKFLT signal");
	}
	else if(wo.wo_stat == 17)
	{
		printk("[program2] : get SIGCHLD signal");
	}
	else if(wo.wo_stat == 18)
	{
		printk("[program2] : get SIGCONT signal");
	}
	else if(wo.wo_stat == 20)
	{
		printk("[program2] : get SIGTSTP signal");
	}
	printk("[program2] : child process terminated");
	printk("[program2] : The return signal is %d\n", wo.wo_stat);



	put_pid(wo_pid);
	return ;
}

//implement fork function
int my_fork(void *argc)
{
	pid_t pid;
	int status;
	//set default sigaction for current process
	int i;
	struct kernel_clone_args my_args = 
	{
		.flags = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};
	

	struct k_sigaction *k_action = &current->sighand->action[0];
	for(i=0;i<_NSIG;i++)
	{
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	
	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_clone(&my_args);

	/* wait until child process terminates */
	printk("[program2] : The Child Process has pid = %d", pid);
	printk("[program2] : This is the parent process, pid = %d", current->pid);
	my_wait(pid);

	// do_exit(0);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init {Tan Jinzhen} {120090543}\n");
	
	/* write your code here */
	
	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init creat kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");
	if (!IS_ERR(task))
	{
		printk("[program2] : module_init kthread start\n");
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
