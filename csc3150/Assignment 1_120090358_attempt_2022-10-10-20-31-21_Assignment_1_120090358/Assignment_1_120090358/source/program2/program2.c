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

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);

/* execute a test program in child process */
int my_exec(void) {
  struct filename *fname;
  const char *path = "/tmp/test";

  fname = getname_kernel(path);

  printk("[program2] : child process\n");
  do_execve(fname, NULL, NULL);

  return 0;
}

// implement fork function
int my_fork(void *argc) {
  int status = 0;
  pid_t pid;
  enum pid_type type;
  struct wait_opts wo;
  struct pid *wo_pid;
  struct kernel_clone_args args = {.flags = SIGCHLD,
                                   .stack = (unsigned long)&my_exec,
                                   .stack_size = 0,
                                   .parent_tid = NULL,
                                   .child_tid = NULL,
                                   .exit_signal = SIGCHLD,
                                   .tls = 0};

  // set default sigaction for current process
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
  pid = kernel_clone(&args);
  printk("[program2] : The child process has pid = %d\n", pid);
  printk("[program2] : This is the parent process, pid = %d\n", (*current).pid);

  /* wait until child process terminates */
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);

  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = (WEXITED | WUNTRACED);
  wo.wo_info = NULL;
  wo.wo_stat = status;
  wo.wo_rusage = NULL;

  do_wait(&wo);

  switch (wo.wo_stat) {
  case 1: {
    printk("[program2] : get SIGUP signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 1\n");
    break;
  }
  case 2: {
    printk("[program2] : get SIGINT signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 2\n");
    break;
  }
  case 9: {
    printk("[program2] : get SIGKILL signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 9\n");
    break;
  }
  case 13: {
    printk("[program2] : get SIGPIPE signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 13\n");
    break;
  }
  case 14: {
    printk("[program2] : get SIGALRM signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 14\n");
    break;
  }
  case 15: {
    printk("[program2] : get SIGTERM signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 15\n");
    break;
  }
  case 131: {
    printk("[program2] : get SIGQUIT signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 3\n");
    break;
  }
  case 132: {
    printk("[program2] : get SIGILL signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 4\n");
    break;
  }
  case 133: {
    printk("[program2] : get SIGTRAP signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 5\n");
    break;
  }
  case 134: {
    printk("[program2] : get SIGABRT signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 6\n");
    break;
  }
  case 135: {
    printk("[program2] : get SIGBUS signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 7\n");
    break;
  }
  case 136: {
    printk("[program2] : get SIGFPE signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 8\n");
    break;
  }
  case 139: {
    printk("[program2] : get SIGSEGV signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 11\n");
    break;
  }
  case 4991: {
    printk("[program2] : get SIGSTOP signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 19\n");
    break;
  }
  default: {
    printk("[program2] : child process normally terminated\n");
    printk("[program2] : The return signal is 0\n");
    break;
  }
  }

  put_pid(wo_pid);
  return 0;
}

static int __init program2_init(void) {
  struct task_struct *task;
  printk("[program2] : Module_init 张元涛 120090358\n");

  /* write your code here */

  /* create a kernel thread to run my_fork */
  printk("[program2] : module_init create kthread start");

  task = kthread_create(&my_fork, NULL, "MyThread");
  if (!IS_ERR(task)) {
    printk("[program2] : module_init kthread start\n");
    wake_up_process(task);
  }

  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
