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

struct wait_opts {  enum pid_type wo_type; 
                    int wo_flags;
                    struct pid *wo_pid;
                    struct waitid_info *wo_info;
					int wo_stat;
                    struct rusage *wo_rusage;
                    wait_queue_entry_t child_wait;
                    int notask_error;};

extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

extern struct filename *getname_kernel(const char * filename);

extern int do_execve(  struct filename *filename,
                       const char __user *const __user *__argv,
                       const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);


char sigtrans[15][8]={"SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP","SIGABRT","SIGBUS","SIGFPE","SIGKILL","SIGUSR1","SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM","SIGTERM"};

void my_wait(pid_t pid){
	struct wait_opts wo;
	struct pid *wo_pid = NULL;
	enum pid_type type;
	int a;
	int b;
	int status;
	type = PIDTYPE_PID;
	wo_pid = find_get_pid(pid);

	wo.wo_type = type;
	wo.wo_pid = wo_pid;
	wo.wo_flags = WEXITED;
	wo.wo_info = NULL;
	wo.wo_stat = status;
	wo.wo_rusage = NULL;

	a = do_wait(&wo);
	// printk("do_wait return value is %d\n", a);
	status = wo.wo_stat & 0x7f;
	// status = status % 128;
	// // b = wo.wo_stat & 0x7f;
	if ((status > 16) || (status < 1)){
	    printk("[program2] : normal termination\n");
	} 
	else{
	    printk("[program2] : get %s signal\n", sigtrans[status-1]);
	}
	printk("[program2] : child process terminated\n");
	printk("[program2] : The return signal is %d\n",status);
	put_pid(wo_pid); //Decrease the count and free memory

	return;
}

int my_exec(void *argc){
	int result;
	const char path[] = "/home/vagrant/source/program2/test";

	struct filename * my_filename = getname_kernel(path);
        // result = do_execve(my_filename,argv,envp);
	result = do_execve(my_filename,NULL,NULL);
	if(!result){
	    return 0;
	}
	do_exit(result);
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

	int (*p)(void *) = my_exec;
	pid = kernel_thread(p,NULL,SIGCHLD);
	printk("[program2] : The child process has pid = %d\n",pid);
	printk("[program2] : This is parent process, pid = %d\n",(int)current->pid);
	printk("[program2] : child process\n");
	/* execute a test program in child process */
	my_wait(pid);
	/* wait until child process terminates */
	return 0;
}

static int __init program2_init(void){
	static struct task_struct *task;
	// printk("[program2] : Module_init\n");
	printk("[program2] : module_init LIU_hengrui_120090609\n");
	
	/* write your code here */
	task = kthread_create(&my_fork, NULL,"MyThread");
	printk("[program2] : module_init create kthread start\n");
	if(!IS_ERR(task)){
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}
	// my_fork();
	/* create a kernel thread to run my_fork */
	
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
