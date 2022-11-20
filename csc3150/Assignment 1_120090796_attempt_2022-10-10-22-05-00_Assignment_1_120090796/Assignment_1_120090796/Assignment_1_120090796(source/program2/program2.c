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

static struct task_struct *task;
struct wait_opts{
	enum pid_type wo_type;
	int	wo_flags;
	struct pid *wo_pid;
	struct waitid_info *wo_info;
	int	wo_stat;
	struct rusage *wo_rusage;
	wait_queue_entry_t child_wait;
	int	notask_error;
};

//extern
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
const char __user *const __user *__argv,
const char __user *const __user *__envp);
extern struct filename*
getname_kernel(const char * filename);


int my_exec(void) {
    int result;
	printk("start executing/n");

    result = do_execve(getname_kernel("/home/vagrant/csc3150/Assignment1/source/program2/test.c"),NULL,NULL);

    if (!result) {
        return 0;
    } else {
        do_exit(result);
    }
}

void output_info(int exit){
    switch (exit) {
        case 0:
            printk("[program2] : child process exit normally\n");
            printk("[program2] : The return signal is 0\n");
            break;

        case 1:
            printk("[program2] : get SIGHUP signal\n");
            printk("[program2] : child process is hung up\n");
            printk("[program2] : The return signal is 1\n");
            break;
        case 2:
            printk("[program2] : get SIGINT signal\n");
            printk("[program2] : terminal interrupt\n");
            printk("[program2] : The return signal is 2\n");
            break;
        case 131:
            printk("[program2] : get SIGQUIT signal\n");
            printk("[program2] : terminal quit\n");
            printk("[program2] : The return signal is 3\n");
            break;
        case 132:
            printk("[program2] : get SIGILL signal\n");
            printk("[program2] : child process has illegal instruction error\n");
            printk("[program2] : The return signal is 4\n");
            break;
        case 133:
            printk("[program2] : get SIGTRAP signal\n");
            printk("[program2] : child process has trap error\n");
            printk("[program2] : The return signal is 5\n");
            break;
        case 134:
            printk("[program2] : get SIGABRT signal\n");
            printk("[program2] : child process has abort error\n");
            printk("[program2] : The return signal is 6\n");
            break;
        case 135:
            printk("[program2] : get SIGBUS signal\n");
            printk("[program2] : child process has bus error\n");
            printk("[program2] : The return signal is 7\n");
            break;
        case 136:
            printk("[program2] : get SIGFPE signal\n");
            printk("[program2] : child process has float error\n");
            printk("[program2] : The return signal is 8\n");
            break;
        case 9:
            printk("[program2] : get SIGKILL signal\n");
            printk("[program2] : child process killed\n");
            printk("[program2] : The return signal is 9\n");
            break;
        case 139:
            printk("[program2] : get SIGSEGV signal\n");
            printk("[program2] : child process has segmentation fault error\n");
            printk("[program2] : The return signal is 11\n");
            break;
        case 13:
            printk("[program2] : get SIGPIPE signal\n");
            printk("[program2] : child process has pipe error\n");
            printk("[program2] : The return signal is 13\n");
            break;
        case 14:
            printk("[program2] : get SIGALARM signal\n");
            printk("[program2] : child process has alarm error\n");
            printk("[program2] : The return signal is 14\n");
            break;
        case 15:
            printk("[program2] : get SIGTERM signal\n");
            printk("[program2] : child process terminated\n");
            printk("[program2] : The return signal is 15\n");
            break;

    }
    return;
}

void my_wait(pid_t pid) {
	int status;
	struct wait_opts wo;
    struct pid *wo_pid = NULL;
    enum pid_type type;
    type = PIDTYPE_PID;
    wo_pid = find_get_pid(pid);

    wo.wo_type = type;
    wo.wo_pid = wo_pid;
    wo.wo_flags = WEXITED;
    wo.wo_info = NULL;
    wo.wo_stat = (int __user*)&status;
    wo.wo_rusage = NULL;

	int a;
	a = do_wait(&wo);

	output_info(status);

	put_pid(wo_pid);
	return;
}

//implement fork function
int my_fork(void *argc){
	
	
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
	struct kernel_clone_args args = {
		.flags = SIGCHLD,
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.parent_tid = NULL,
		.child_tid = NULL,
		.tls = 0,
		.exit_signal = SIGCHLD,
	};
	pid = kernel_clone(&args);

	/* execute a test program in child process */
	printk("[program2] : The child process has pid= %d\n", pid);
    printk("[program2] : The parent process has pid= %d\n", (int) current->pid);

	/* wait until child process terminates */
	my_wait(pid);
	return 0;
}

static int __init program2_init(void){

	printk("[program2] : Module_init\n");
	
	/* write your code here */
	/* create a kernel thread to run my_fork */
	printk("[program2] : Module_init create kthread start\n");

	task = kthread_create(&my_fork, NULL, "MyThread");
    if (!IS_ERR(task)) {
        printk("[program2] : Module_init kthread starts\n");
        wake_up_process(task);
    }

	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
