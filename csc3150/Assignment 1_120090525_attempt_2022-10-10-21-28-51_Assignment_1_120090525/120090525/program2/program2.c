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

extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);

struct wait_opts {enum pid_type wo_type;
			    int wo_flags;
		    	struct pid *wo_pid;
			    struct waitid_info *wo_info;
			    int wo_stat;
			    struct rusage *wo_rusage;
			    wait_queue_entry_t child_wait;
		   	  int notask_error;};

int my_wait(pid_t pid) {
  
  int status = 0;
  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);

  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WEXITED|WUNTRACED;
  wo.wo_info = NULL;
  wo.wo_stat = status;
  wo.wo_rusage = NULL;
  int a;
  a = do_wait(&wo);

  put_pid(wo_pid);
  return wo.wo_stat;
}
void my_exec(void){
  int i;
	char path[] = "/tmp/test";
  struct filename* my_filename = getname_kernel(path);
	i = do_execve(my_filename, NULL, NULL);
	return ;
}

// implement fork function
int my_fork(void *argc) {
  struct kernel_clone_args kernal_args = {
      .flags = SIGCHLD,
      .stack = (unsigned long)&my_exec,
      .stack_size = 0,
      .child_tid = NULL,
      .parent_tid = NULL,
      .exit_signal = SIGCHLD,
  }; 
  // set default sigaction for current process
  int i;
  struct k_sigaction *k_action = &current->sighand->action[0];
  for (i=0;i<_NSIG;i++) {
    k_action->sa.sa_handler = SIG_DFL;
    k_action->sa.sa_flags = 0;
    k_action->sa.sa_restorer = NULL;
    sigemptyset(&k_action->sa.sa_mask);
    k_action++;
  }
  

  pid_t pid;
  pid = kernel_clone(&kernal_args);
  printk("[program2] : The child process has pid = %d\n", pid);
  printk("[program2] : This is the parent process, pid = %d\n", current->pid);
  printk("[program2] : child process\n");
  int status = my_wait(pid);
	status &= 0x7f;
  switch(status){

    case 0
      printk("[program2] : get SIGCHLD signal\n");
      printk("[program2] : this is a normal termination\n");
      break;
    case 6
      printk("[program2] : get SIGABRT signal\n");
      printk("[program2] : child process is aborted\n");
      break;
    case 14
      printk("[program2] : get SIGALRM signal\n");
      printk("[program2] : child process is alarmed\n");
      break;
    case 7
      printk("[program2] : get SIGBUS signal\n");
      printk("[program2] : child process bus errored\n");
      break;
    case 8
      printk("[program2] : get SIGFPE signal\n");
      printk("[program2] : child process is floating-point excepted\n");
      break;
    case 1
      printk("[program2] : get SIGHUP signal\n");
      printk("[program2] : child process is hunguped\n");
      break;
    case 4
      printk("[program2] : get SIGILL signal\n");
      printk("[program2] : child process id illegally instructed\n");
      break;
    case 2
      printk("[program2] : get SIGINT signal\n");
      printk("[program2] : child process is interrupted\n");
      break;
    case 9
      printk("[program2] : get SIGKILL signal\n");
      printk("[program2] : child process is killed\n");
      break;
    case 13
      printk("[program2] : get SIGPIPE signal\n");
      printk("[program2] : child process is broken\n");
      break;
    case 3
      printk("[program2] : get SIGQUIT signal\n");
      printk("[program2] : child process is quitted\n");
      break;
    case 11
      printk("[program2] : get SIGSEGV signal\n");
      printk("[program2] : child process is segmentated\n");
      break;
    case 15
      printk("[program2] : get SIGTERM signal\n");
      printk("[program2] : child process terminated\n");
      break;
    case 5
      printk("[program2] : get SIGTRAP signal\n");
      printk("[program2] : child process is traped\n");
      break;
    case 19
      printk("[program2] : get SIGSTOP signal\n");
      printk("[program2] : child process stopped\n");
      break;
    case 127
      printk("[program2] : get SIGSTOP signal\n");
      printk("[program2] : child process stopped\n");
      break;
  }
  if (status == 127) {
    printk("[program2] : The return signal is 19");
  }
  else {
    printk("[program2] : The return signal is %d\n", status & 0x7F);
  }
  return 0;
}

static int __init program2_init(void) {
  struct task_struct *task;
  printk("[program2] : Module_init {Xingjian Li} {120090525}\n");
  
  /* write your code here */
  printk("[program2] : module_init create kthread start");
  
  /* create a kernel thread to run my_fork */
  task = kthread_create(&my_fork, NULL, "vagrant");
  if (!IS_ERR(task)) {
    printk("[program2] : module_init kthread start\n");
    wake_up_process(task);
  }
  return 0;
}

static void __exit program2_exit(void) {
  printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);