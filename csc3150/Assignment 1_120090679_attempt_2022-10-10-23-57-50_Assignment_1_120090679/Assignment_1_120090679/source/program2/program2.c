#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define __WIFSTOPPED(status) (((status)&0xff) == 0x7f)

MODULE_LICENSE("GPL");

/* function prototypes */

struct wait_opts {
  enum pid_type wo_type; // It is defined in ‘/include/linux/pid.h’.
  int wo_flags;          // Wait options. (0, WNOHANG, WEXITED, etc.)
  struct pid *wo_pid;    // Kernel's internal notion of a process
                      // identifier.“Find_get_pid()”
  struct siginfo __user *wo_info;  // Singal information.
  int __user wo_stat;              // Child process’s termination status
  struct rusage __user *wo_rusage; // Resource usage
  wait_queue_entry_t child_wait;   // Task wait queue
  int notask_error;
};

/* extern functions */
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
                     const char __user *const __user *argv,
                     const char __user *const __user *envp);
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern struct filename *getname_kernel(const char *filename);

static struct task_struct *my_thread;

void my_wait(pid_t pid) {
  int status;
  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);

  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WEXITED | WSTOPPED;
  wo.wo_info = NULL;
  wo.wo_stat = (int __user *)&status;
  wo.wo_rusage = NULL;

  int a;
  a = do_wait(&wo);

  switch (wo.wo_stat) {
  case 0: /* normal termination */
    printk("[program2] : child process exit normally\n");
    printk("[program2] : The return signal is 0\n");
    break;
  case 1: /* hangup */
    printk("[program2] : get SIGHUP signal\n");
    printk("[program2] : child process is hung up\n");
    printk("[program2] : The return signal is 1\n");
    break;
  case 2: /* interrupt */
    printk("[program2] : get SIGINT signal\n");
    printk("[program2] : terminal imterrupt\n");
    printk("[program2] : The return signal is 2\n");
    break;
  case 131: /* quit */
    printk("[program2] : get SIGQUIT signal\n");
    printk("[program2] : child process quit\n");
    printk("[program2] : The return signal is 3\n");
    break;
  case 132: /* illigal_instr */
    printk("[program2] : get SIGILL signal\n");
    printk("[program2] : child process has illegal instruction error\n");
    printk("[program2] : The return signal is 4\n");
    break;
  case 133: /* trap */
    printk("[program2] : get SIGTRAP signal\n");
    printk("[program2] : child process has trap error\n");
    printk("[program2] : The return signal is 5\n");

    break;
  case 134: /* abort */
    printk("[program2] : get SIGABRT signal\n");
    printk("[program2] : child process aborted\n");
    printk("[program2] : The return signal is 6\n");
    break;
  case 135: /* bus */
    printk("[program2] : get SIGBUS signal\n");
    printk("[program2] : child process has bus error\n");
    printk("[program2] : The return signal is 7\n");

    break;
  case 136: /* floating */
    printk("[program2] : get SIGFPE signal\n");
    printk("[program2] : child process has float error\n");
    printk("[program2] : The return signal is 8\n");

    break;
  case 9: /* kill */
    printk("[program2] : get SIGKILL signal\n");
    printk("[program2] : child process is killed\n");
    printk("[program2] : The return signal is 9\n");

    break;
  case 139: /* segment_fault */
    printk("[program2] : get SIGSEGV signal\n");
    printk("[program2] : child process has segmentation fault error\n");
    printk("[program2] : The return signal is 11\n");
    break;
  case 13: /* pipe */
    printk("[program2] : get SIGPIPE signal\n");
    printk("[program2] : child process has pipe error\n");
    printk("[program2] : The return signal is 13\n");
    break;
  case 14: /* alarm */
    printk("[program2] : get SIGALRM signal\n");
    printk("[program2] : child process has alarm error\n");
    printk("[program2] : The return signal is 14\n");
    break;
  case 15: /* terminate */
    printk("[program2] : get SIGTERM signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is 15\n");
    break;
  case 19: /* stop */
    printk("[program2] : get SIGSTOP signal\n");
  default:
    break;
  }

  if ((((wo.wo_stat) & 0xff) == 0x7f)) {
    printk("[program2] : get SIGSTOP signal\n");
    printk("[program2] : child process stopped\n");
    printk("[program2] : The return signal is 19\n");
  }

  put_pid(wo_pid);

  return;
}

int my_exec(void) {
  int result;
  const char path[] = "/tmp/test";
  const char *const argv[] = {path, NULL, NULL};
  const char *const envp[] = {"HOME=/", "PATH=/sbin:/user/sbin:/bin:/usr/bin",
                              NULL};
  struct filename *my_filename = getname_kernel(path);
  printk("[program2] : child process\n");
  result = do_execve(my_filename, NULL, NULL);
  if (!result) {
    return 0;
  } else {
    do_exit(result);
  }
  return 0;
}

// implement fork function
extern int my_fork(void *argc) {

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
  struct kernel_clone_args kca = {
      .flags = SIGCHLD,
      .child_tid = NULL,
      .parent_tid = NULL,
      .stack = (unsigned long)&my_exec,
      .stack_size = 0,
      .tls = 0,
      .exit_signal = SIGCHLD,
  };

  pid_t pid;
  pid = kernel_clone(&kca);

  /* execute a test program in child process */
  printk("[program2] : The child process has pid = %d\n", pid);
  printk("[program2] : This is the parent process, pid = %d\n",
         (int)current->pid);

  /* wait until child process terminates */
  my_wait(pid);

  return 0;
}

static int __init program2_init(void) {

  printk("[program2] : Module_init Cai_Zhao 120090679\n");

  /* create a kernel thread to run my_fork */

  printk("[program2] : module_init create kthread start\n");
  my_thread = kthread_create(&my_fork, NULL, "my_thread");
  if (!IS_ERR(my_thread)) {
    printk("[program2] : module_init kthread start\n");
    wake_up_process(my_thread);
  }
  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
