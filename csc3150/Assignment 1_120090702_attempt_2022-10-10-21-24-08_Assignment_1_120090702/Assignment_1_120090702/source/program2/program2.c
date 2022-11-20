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
static struct task_struct *start;
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

extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern struct filename *getname_kernel(const char *filename);

void my_wait(pid_t pid);
void my_exec(void);

void my_wait(pid_t pid) {
  int status;
  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid); // check hash to get pid*

  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WUNTRACED | WEXITED;
  wo.wo_info = NULL;
  wo.wo_stat = status;
  wo.wo_rusage = NULL;

  int a;
  a = do_wait(&wo);
  // printk("do_wait is %d\n",&a)
  printk("[program2] : get SIGTERM signal\n");
  printk("[program2] : child process terminated\n");
  int signal_int = wo.wo_stat & 0x7f;
  if (signal_int == 127)
    printk("[program2] : CHILD PROCESS STOPPED\n"); // stop signal is different
  else if (signal_int == 0)
    printk("[program2] : Normal termination with EXIT STATUS = %d\n",
           signal_int);
  else
    printk("[program2] : The return signal is %d\n", signal_int);

  put_pid(wo_pid);
}

void my_exec(void) {
  printk("[program2] : child process\n");
  do_execve(getname_kernel("/tmp/test"), NULL,
            NULL); // need change to /tmp/test
}

// implement fork function
int my_fork(void *argc) {
  printk("[program2] : module_init kthread start\n");

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
  struct kernel_clone_args child_args = {
      .flags = SIGCHLD,
      .child_tid = NULL,
      .parent_tid = NULL,
      .stack = (unsigned long)&my_exec,
      .exit_signal = SIGCHLD, // to terminate the child trough signal
      .stack_size = 0,
      .tls = 0,
  };
  pid_t pid_2 = kernel_clone(&child_args);
  printk("[program2] : The child process has pid = %d\n", pid_2);

  if (pid_2 == -1)
    printk("error in kernel_clone");
  if (pid_2 == 0) {
    /* execute a test program in child process */
    my_exec();
    do_exit(1);
  } else {
    printk("[program2] : This is the parent process, pid = %d\n", current->pid);
    /* wait until child process terminates */
    my_wait(pid_2); // need parent's pid
  }

  return 0;
}

static int __init program2_init(void) {

  printk("[program2] : module_init {Li Yiqian} {120090702}\n");

  /* write your code here */

  /* create a kernel thread to run my_fork */

  start = kthread_create(&my_fork, NULL, "start");
  if (!IS_ERR(start)) {
    printk("[program2] : module_init create kthread start\n");
    wake_up_process(start);
  }
  return 0;
}

static void __exit program2_exit(void) {
  printk("[program2] : module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
