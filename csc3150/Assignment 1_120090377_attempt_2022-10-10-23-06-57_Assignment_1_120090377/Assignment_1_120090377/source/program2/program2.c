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

//	STRUCT
struct wait_opts {
  enum pid_type wo_type;
  int wo_flags;        //(0, WNOHANG, WEXITED, etc.)
  struct pid *wo_pid;  // kernel's internal notion of a process identifier.
  struct siginfo __user *wo_info;
  int wo_stat;  // Child process's termination status. 5.10 changed it to int
                // type ? it used to be int __user *
  struct rusage __user *wo_rusage;  // Resource usage
  wait_queue_entry_t child_wait;
  int notask_error;
};
/*
struct kernel_clone_args{
        u64 flags;
        int __user *pidfd;
        int __user *child_tid;
        int __user *parent_tid;
        int exit_signal;
        unsigned long stack;
        unsigned long stack_size;
        unsigned long tls;
        pid_t *set_tid;
        size_t set_tid_size;
        int cgroup;
        int io_thread;
        struct cgroup *cgrp;
        struct css_set *cset;

};
*/
static struct task_struct *my_task;

//	EXPORT SYMBOL
extern struct filename *getname_kernel(const char *filename);
extern int do_execve(struct filename *filename,  //	Already done
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
extern long do_wait(
    struct wait_opts *wo);  //	Already changed the kernel source code.
extern pid_t kernel_clone(
    struct kernel_clone_args *arg);  //	Already done with kernel_thread too.

/* My implementation of detecting signal according to c Lib's implementation*/

/*
 *--------------------
 *	if WIFSIGNALED(status), return the termination signal.
 *--------------------
 */
int WTERMSIG(int status) { return ((status)&0x7f); }

/*
 *--------------------
 *	if WIFEXITED(status), return the low-order 8 bits of the status
 *--------------------
 */
int WEXITSTATUS(int status) { return (((status)&0xff00) >> 8); }

/*
 *--------------------
 *	if WIFSTOPPED(status), return the signal that stopped the child.
 *--------------------
 */
int WSTOPSIG(int status) { return (WEXITSTATUS(status)); }

/*
 *--------------------
 *	Nonzero if STATUS indicates the child is stopped.
 *--------------------
 */
int WIFSTOPPED(int status) { return (((status)&0xff) == 0x7f); }

/*
 *--------------------
 *	Nonzero if STATUS indicates termination by a signal.
 *--------------------
 */
signed char WIFSIGNALED(int status) {
  return (((signed char)(((status)&0x7f) + 1) >> 1) > 0);
}

/*
 *--------------------
 *	Nonzero if STATUS indicates normal termination.
 *--------------------
 */
int WIFEXITED(int status) { return (WTERMSIG(status) == 0); }

char *WGETSIGNAL(int status) {
  char *INT_TO_SIGNALS[] = {"SIGHUP",  "SIGINT",  "SIGQUIT", "SIGILL",
                            "SIGTRAP", "SIGABRT", "SIGBUS",  "SIGFPE",
                            "SIGKILL", NULL,      "SIGSEGV", NULL,
                            "SIGPIPE", "SIGALRM", "SIGTERM"};

  return INT_TO_SIGNALS[status - 1];
}

void my_wait(pid_t pid) {
  int status;
  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;

  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);  // ? return with its count evaluated.

  //	Init wo
  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WEXITED | WUNTRACED;  //	WUNTRACED maybe
  wo.wo_info = NULL;
  wo.wo_stat = status;
  wo.wo_rusage = NULL;

  int a;
  a = do_wait(&wo);  //	wait for the pid to terminate or what

  int STATUS;
  STATUS = wo.wo_stat;

  put_pid(wo_pid);  //	Decrease the count and free the memory.

  //	Output the return signal of child process

  if (WIFEXITED(STATUS)) {
    printk("[program2] : Normal termination with EXIT STATUS = %d \n",
           WEXITSTATUS(STATUS));
    printk("[program2] : Child process terminated\n");
    printk("[program2] : The return signal is %d\n", WEXITSTATUS(STATUS));
  } else if (WIFSIGNALED(STATUS)) {
    printk("[program2] : get %s signal \n", WGETSIGNAL(WTERMSIG(STATUS)));
    printk("[program2] : Child process terminated\n");
    printk("[program2] : The return signal is %d\n", WTERMSIG(STATUS));

  } else if (WIFSTOPPED(STATUS)) {
    printk("[program2] : child process STOPPED\n");
    if (WSTOPSIG(STATUS) == 19) {
      printk("[program2] : get SIGSTOP signal\n");
    }
    printk("[program2] : Child process terminated\n");
    printk("[program2] : The return signal is %d\n", WSTOPSIG(STATUS));
  }

  //	The exit status is stored in wo.wo_stat

  return;
}

/*
 *--------------------
 *	The Child process's stack ptr will be set to this func
 *--------------------
 */
int my_child_stack(void) {
  const char path[] = "/tmp/test";

  // struct filename * my_filename = getname_kernel(path);

  printk("[program2] : child process");
  int return_of_execve;
  return_of_execve = do_execve(getname_kernel(path), NULL, NULL);
  // printk("[program2] : return of do_execve = %d\n", return_of_execve);
  return 0;
}

// implement fork function
int my_fork(void *argc) {
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
  struct kernel_clone_args args = {
      .flags = SIGCHLD,
      .exit_signal = SIGCHLD,
      .stack = &my_child_stack,
      .stack_size = 0,
      .child_tid = NULL,
      .parent_tid = NULL,
  };

  /* execute a test program in child process */
  pid_t pid = kernel_clone(&args);

  if (pid < 0) {
    printk("fork ERROR\n");
  } else {
    //	Parent process
    if (pid > 0) {
      printk("[program2] : The child process has pid = %d", pid);
      printk("[program2] : This is the parent process, pid = %d",
             task_pid_nr(current));
    }
  }

  /* wait until child process terminates */
  my_wait(pid);

  return 0;
}

static int __init program2_init(void) {
  printk("[program2] : Module_init YuyangLIN 120090377 \n");
  /* write your code here */

  /* create a kernel thread to run my_fork */
  printk("[program2] : Module_init create kthread start \n");
  my_task = kthread_create(&my_fork, NULL, "MyThread");

  /* Wake Up the Kthread */
  if (!IS_ERR(my_task)) {
    printk("[program2] : Module_init kthread start \n");
    wake_up_process(my_task);
  }
  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
