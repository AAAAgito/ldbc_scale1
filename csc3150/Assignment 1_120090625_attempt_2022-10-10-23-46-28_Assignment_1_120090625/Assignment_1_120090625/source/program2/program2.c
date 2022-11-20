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

#define __WIFSTOPPED(status) (((status)&0xff) == 0x7f)

// wait_opts structure
struct wait_opts {
  enum pid_type wo_type;
  int wo_flags;
  struct pid *wo_pid;
  struct waitid_info *wo_info;
  // struct siginfo *wo_info;
  int wo_stat;
  struct rusage *wo_rusage;
  wait_queue_entry_t child_wait;
  int notask_error;
};
static struct task_struct *task;
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname(const char __user *filename);
extern struct filename *getname_kernel(const char __user *filename);
extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);

int my_exec(void) {
  int return_value;
  const char path[] = "/tmp/test";

  return_value = do_execve(getname_kernel(path), NULL, NULL);
  if (!return_value) {
    return 0;
  } else {
    printk("[program2] : The result of my_exec() has errors! %d", return_value);
    do_exit(return_value);
  }
}

// print the related information to the log
void my_print(int status) {
  if (__WIFSTOPPED(status)) {
    // case 127:
    printk("[program2] : get SIGSTOP signal\n");
    printk("[program2] : child process stop\n");
    printk("[program2] : The return signal is 19\n");
    // break;
  }
  switch (status) {
  case 134:
    printk("[program2] : get SIGABRT signal\n");
    printk("[program2] : child process has abort error\n");
    printk("[program2] : The return signal is 6\n");
    break;
  case 14:
    printk("[program2] : get SIGALARM signal\n");
    printk("[program2] : child process has alarm error\n");
    printk("[program2] : The return signal is 14\n");
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
  case 1:
    printk("[program2] : get SIGHUP signal\n");
    printk("[program2] : child process is hung up\n");
    printk("[program2] : The return signal is 1\n");
    break;
  case 132:
    printk("[program2] : get SIGILL signal\n");
    printk("[program2] : child process has illegal instruction error\n");
    printk("[program2] : The return signal is 4\n");
    break;
  case 2:
    printk("[program2] : get SIGINT signal\n");
    printk("[program2] : terminal interrupt\n");
    printk("[program2] : The return signal is 2\n");
    break;
  case 9:
    printk("[program2] : get SIGKILL signal\n");
    printk("[program2] : child process killed\n");
    printk("[program2] : The return signal is 9\n");
    break;
  case 0:
    printk("[program2] : child process exit normally\n");
    printk("[program2] : The return signal is 0\n");
    break;
  case 13:
    printk("[program2] : get SIGPIPE signal\n");
    printk("[program2] : child process has pipe error\n");
    printk("[program2] : The return signal is 13\n");
    break;
  case 131:
    printk("[program2] : get SIGQUIT signal\n");
    printk("[program2] : terminal quit\n");
    printk("[program2] : The return signal is 3\n");
    break;
  case 139:
    printk("[program2] : get SIGSEGV signal\n");
    printk("[program2] : child process has segmentation fault error\n");
    printk("[program2] : The return signal is 11\n");
    break;
  case 15:
    printk("[program2] : get SIGTERM signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 15\n");
    break;
  case 133:
    printk("[program2] : get SIGTRAP signal\n");
    printk("[program2] : child process has trap error\n");
    printk("[program2] : The return signal is 5\n");
    break;
  }
  return;
}

// implement wait function
int my_wait(pid_t pid) {
  int return_signal;
  struct wait_opts wo;
  enum pid_type type;
  struct pid *wo_pid = NULL;
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);

  wo.wo_type = type;
  wo.wo_flags = WEXITED | WUNTRACED;
  wo.wo_pid = wo_pid;
  wo.wo_info = NULL;
  wo.wo_stat = 10;
  wo.wo_rusage = NULL;
  do_wait(&wo);
  put_pid(wo_pid);
  return_signal = (unsigned long)wo.wo_stat;
  return return_signal;
}

// implement fork function
int my_fork(void *argc) {
  int i;
  int status;

  // set default sigaction for current process
  struct k_sigaction *k_action = &current->sighand->action[0];
  for (i = 0; i < _NSIG; i++) {
    k_action->sa.sa_handler = SIG_DFL;
    k_action->sa.sa_flags = 0;
    k_action->sa.sa_restorer = NULL;
    sigemptyset(&k_action->sa.sa_mask);
    k_action++;
  }

  // struct kernel_clone_args arg = {
  //     .flags =
  //         ((lower_32_bits(SIGCHLD) | CLONE_VM | CLONE_UNTRACED) & ~CSIGNAL),
  //     .exit_signal = (lower_32_bits(SIGCHLD) & CSIGNAL),
  //     .stack = (unsigned long)my_exec,
  //     .stack_size = (unsigned long)NULL,
  // };
  struct kernel_clone_args arg = {
      .flags = SIGCHLD,
      .exit_signal = SIGCHLD,
      .stack = (unsigned long)my_exec,
      .stack_size = 0,
      .parent_tid = NULL,
      .child_tid = NULL,
  };

  /* fork a process using kernel_clone or kernel_thread */
  /* execute a test program in child process */
  pid_t pid = kernel_clone(&arg);
  printk("[program2] : The child process has pid = %d\n", pid);
  printk("[program2] : This is the parent process, pid = %d\n",
         (int)current->pid);
  printk("[program2] : child process\n");
  status = my_wait(pid);

  // checking return signal
  my_print(status);

  return 0;
}

static int __init program2_init(void) {
  printk("[program2] : module_init {Song Wenxin} {120090625}\n");
  printk("[program2] : module_init create kthread start");

  /* create a kernel thread to run my_fork */
  task = kthread_create(&my_fork, NULL, "MyThread");

  if (!IS_ERR(task)) {
    printk("[program2] : module_init kthread start");
    wake_up_process(task);
  }
  return 0;
}

static void __exit program2_exit(void) {
  printk("[program2] : module_exit./6\n");
}

module_init(program2_init);
module_exit(program2_exit);
