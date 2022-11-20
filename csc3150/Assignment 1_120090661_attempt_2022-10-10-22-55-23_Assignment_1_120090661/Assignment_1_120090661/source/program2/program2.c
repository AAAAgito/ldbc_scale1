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
#include <linux/signal.h>
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

static struct task_struct *task;
extern long do_wait(struct wait_opts *wo);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve ( struct filename *filename,
const char __user *const __user *__argv,
const char __user *const __user *__envp);

extern struct filename *
getname_kernel(const char * filename);

int my_exec(void)
{
    int flag;
    printk("[program2] : child process");
    flag = do_execve(getname_kernel("/tmp/test"), NULL, NULL);
    if (!flag) return 0;
      else do_exit(flag);
}

void my_wait(pid_t pid) {
    int status;
    int a;
    struct wait_opts wo;
    struct pid *wo_pid = NULL;
    enum pid_type type;
    type = PIDTYPE_PID;
    wo_pid = find_get_pid(pid);

    wo.wo_type = type;
    wo.wo_pid = wo_pid;
    wo.wo_flags = WEXITED;
    wo.wo_info = NULL;
    wo.wo_stat = status;
    wo.wo_rusage = NULL;

	a = do_wait(&wo);
	if ( wo.wo_stat == 0){
		printk("[program2] : child process exit normally\n");
		printk("[program2] : The return signal is 0\n");
	}
	else if (wo.wo_stat == 1){
		printk("[program2] : get SIGHUP signal\n");
		printk("[program2] : The return signal is 1\n");
	}

	else if (wo.wo_stat == 2){
		printk("[program2] : get SIGINT signal\n");
		printk("[program2] : The return signal is 2\n");
	}

	else if (wo.wo_stat == 131){
		printk("[program2] : get SIGQUIT signal\n");
		printk("[program2] : The return signal is 3\n");
	}

	else if (wo.wo_stat == 132){
		printk("[program2] : get SIGILL signal\n");
		printk("[program2] : The return signal is 4\n");
	}

	else if (wo.wo_stat == 133){
		printk("[program2] : get SIGTRAP signal\n");
		printk("[program2] : The return signal is 5\n");
	}

	else if (wo.wo_stat == 134){
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : The return signal is 6\n");
	}

	else if (wo.wo_stat == 135){
		printk("[program2] : get SIGBUS signal\n");
		printk("[program2] : The return signal is 7\n");
	}

	else if (wo.wo_stat == 136){
		printk("[program2] : get SIGFPE signal\n");
		printk("[program2] : The return signal is 8\n");
	}

	else if (wo.wo_stat == 9){
		printk("[program2] : get SIGKILL signal\n");
		printk("[program2] : The return signal is 9\n");
	}

	else if (wo.wo_stat == 139){
		printk("[program2] : get SIGSEGV signal\n");
		printk("[program2] : The return signal is 11\n");
	}

	else if (wo.wo_stat == 13){
		printk("[program2] : get SIGPIPE signal\n");
		printk("[program2] : The return signal is 13\n");
	}

	else if (wo.wo_stat == 14){
		printk("[program2] : get SIGALARM signal\n");
		printk("[program2] : The return signal is 14\n");
	}
	else if (wo.wo_stat == 15){
		printk("[program2] : get SIGTERM signal\n");
		printk("[program2] : The return signal is 15\n");
	}
	else{
		printk("[program2] : process continues\n");
		printk("[program2] : process signal is %d \n",wo.wo_stat);
	}
}

//implement fork function
extern int my_fork(void *argc){
	
	//set default sigaction for current process
	int i;
	pid_t pid;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for(i=0;i<_NSIG;i++){
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}
	
	/* fork a process using kernel_clone or kernel_thread */
	struct kernel_clone_args ker_args = {
		.flags = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};
    pid = kernel_clone(&ker_args);
	
	/* execute a test program in child process */
	printk("[program2] : The child process has pid= %d\n", pid);
	printk("[program2] : The parent process has pid= %d\n", (int) current->pid);
	
	/* wait until child process terminates */
    my_wait(pid);
	
	return 0;
}

static int __init program2_init(void){

	printk("[program2] : module_init {邓毅轩} {120090661}\n");
	
	/* write your code here */
    printk("[program2] : module_init create kthread start\n");
	
	/* create a kernel thread to run my_fork */
    task = kthread_create(&my_fork, NULL, "test_thread");
    if (!IS_ERR(task))
    {
        printk("[program2] : module_init kthread start\n");
        wake_up_process(task);
    }
	
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
