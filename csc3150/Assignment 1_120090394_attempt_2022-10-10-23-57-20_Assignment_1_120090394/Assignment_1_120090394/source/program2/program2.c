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
#include <linux/wait.h>

MODULE_LICENSE("GPL");

struct wait_opts {
	enum pid_type wo_type;
	int	wo_flags;
	struct pid	*wo_pid;

	struct siginfo __user *wo_info;
	int __user	*wo_stat;
	struct rusage __user*wo_rusage;

	wait_queue_t child_wait;
	
	int	notask_error;
};

static struct task_struct *task;
extern long _do_fork(unsigned long clone_flags,
      unsigned long stack_start,
      unsigned long stack_size,
      int __user *parent_tidptr,
      int __user *child_tidptr,
      unsigned long tls);

extern int do_execve(struct filename *filename,
		const char __user *const __user *__argv,
		const char __user *const __user *__envp);

extern struct filename *getname(const char __user * filename);

extern long do_wait(struct wait_opts *wo);

extern void __noreturn do_exit(long code);

void my_wait(pid_t pid){
	int status;
	struct wait_opts wo;
	struct pid *wo_pid=NULL;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo_pid=find_get_pid(pid);

	wo.wo_type=type;
	wo.wo_pid=wo_pid;
	wo.wo_flags=WEXITED|WUNTRACED;
	wo.wo_info = NULL;
	wo.wo_stat=(int __user*)&status;
	wo.wo_rusage=NULL;

	int a;
	a=do_wait(&wo);
	int sig = *wo.wo_stat;
	printk("child process");

	if(sig==4991){
		sig=19;
	}
	if(sig>=128){
		sig=sig-128;
	}
	if(sig==0){
		printk("get SIGCHID signal\n");
		printk("child process is normal\n");
	}
	
	if(sig==1){
		printk("get SIGHUP signal\n");
		printk("child process is hung up\n");
	}
	if(sig==2){
		printk("get SIGINT signal\n");
		printk("child process is interrupted\n");
	}
	if(sig==3){
		printk("get SIGQUIT signal\n");
		printk("child process is quited\n");
	}
	if(sig==4){
		printk("get SIGILL signal\n");
		printk("child process is illegal\n");
	}
	if(sig==5){
		printk("get SIGTRAP signal\n");
		printk("child process is trapped\n");
	}
	if(sig==6){
		printk("get SIGABRT signal\n");
		printk("child process is aborted\n");
	}
	if(sig==7){
		printk("get SIGBUS signal\n");
		printk("child process is bussed\n");
	}
	if(sig==8){
		printk("get SIGFPE signal\n");
		printk("child process is bussed\n");
	}
	if(sig==9){
		printk("get SIGKILL signal\n");
		printk("child process is killed\n");
	}
	if(sig==11){
		printk("get SIGSEGV signal\n");
		printk("child process is segment fault\n");
	}
	if(sig==13){
		printk("get SIGPIPE signal\n");
		printk("child process is piped\n");
	}
	if(sig==14){
		printk("get SIGALRM signal\n");
		printk("child process alarmed\n");
	}
	if(sig==15){
		printk("get SIGTERM signal\n");
		printk("child process is TERMINATED\n");
	}
	if(sig==19){
		printk("get SIGSTOP signal\n");
		printk("child process stopped\n");
	}
	printk("The return signal is %d\n",sig);
	put_pid(wo_pid);

	return;
}



int my_exec(void){
	int result;
	const char path[]= "/opt/test";
	const char *const  argv[]={path,NULL,NULL};
	const char *const  envp[]={"HOME=/","PATH=sbin:/user/sbin:/bin:/usr/bin",NULL};

	struct  filename * my_filename = getname(path);

	result = do_execve(my_filename,argv,envp);
	
	if(!result){
		return 0;
	}
	do_exit(result);
	
}
int my_fork(void *argc){
	long pid;
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for(i=0;i<_NSIG;i++){
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	int status;
	pid = _do_fork(SIGCHLD,(unsigned long)&my_exec,0,NULL,NULL,0);

    if(pid==-1){
    }
    else{
		
		printk("The Child Process has pid = %ld\n", pid);
		printk("This is the parent process, pid = %d\n", (int)current->pid);
		my_wait(pid);
		
		
	}

	
	return 0;
}

static int __init program2_init(void){

	printk("[program2] : Module_init\n");

	task=kthread_create(&my_fork,NULL,"MyThread");
	printk("[program2] : Module_init create kthread start\n");
	
	if(!IS_ERR(task)){
		printk("[program2] : Module_init Kthread starts\n");
		wake_up_process(task);
	}
	
	return 0;
}

static void __exit program2_exit(void){
	printk("[program2] : Module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
