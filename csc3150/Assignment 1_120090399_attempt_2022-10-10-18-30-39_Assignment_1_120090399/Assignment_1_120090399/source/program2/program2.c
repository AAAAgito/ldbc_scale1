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
#include <linux/signal.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;

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

extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
extern struct filename *getname_kernel(const char *filename);
extern long do_wait(struct wait_opts *wo);

int my_exec(void) {
  int execStatus;
  char *path = "/tmp/test";
  struct filename *my_filename = getname_kernel(path);

  execStatus = do_execve(my_filename, NULL, NULL);
  if (execStatus == 0) {
    return 0;
  }
  do_exit(execStatus);
}

int my_wait(pid_t pid) {
  int a;
  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);

  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WEXITED | WSTOPPED;
  wo.wo_info = NULL;
  wo.wo_rusage = NULL;

  a = do_wait(&wo);
  put_pid(wo_pid);
  return wo.wo_stat;
}

// implement fork function
int my_fork(void *argc) {

  // set default sigaction for current process
  int i;
  pid_t pid;
  int status;
  struct kernel_clone_args args = {
      .flags = SIGCHLD,
      .stack = (unsigned long)&my_exec,
      .stack_size = 0,
      .parent_tid = NULL,
      .child_tid = NULL,
      .tls = 0,
      .exit_signal = SIGCHLD,
  };
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
  /* execute a test program in child process */

  printk("[program2] : The child process has pid = %d\n", pid);
  printk("[program2] : The parent process has pid= %d\n", (int)current->pid);
  printk("[program2] : child process");

  /* wait until child process terminates */
  status = my_wait(pid) & 0x7f;
  if (status == 0) {
    printk("[program2] : child process normally exited\n");
    printk("[program2] : The return signal is %d\n", status);
  }
  if (status == 1) {
    printk("[program2] : get SIGHUP signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 2) {
    printk("[program2] : get SIGINT signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 3) {
    printk("[program2] : get SIGQUIT signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 4) {
    printk("[program2] : get SIGILL signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 5) {
    printk("[program2] : get SIGTRAP signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 6) {
    printk("[program2] : get SIGABRT signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 7) {
    printk("[program2] : get SIGBUS signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 8) {
    printk("[program2] : get SIGFPE signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 9) {
    printk("[program2] : get SIGKILL signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 11) {
    printk("[program2] : get SIGSEGV signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 13) {
    printk("[program2] : get SIGPIPE signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 14) {
    printk("[program2] : get SIGALRM signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 15) {
    printk("[program2] : get SIGTERM signal\n");
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", status);
  } else if (status == 127) {
    printk("[program2] : get SIGSTOP signal\n");
    printk("[program2] : child process stopped\n");
    printk("[program2] : The return signal is 19\n");
  }
  return 0;
}

static int __init program2_init(void) {

  printk("[program2] : Module_init Peng Yihao 120090399\n");
  printk("[program2] : Module_init create kthread start");

  /* write your code here */
  task = kthread_create(&my_fork, NULL, "MyThread");
  /* create a kernel thread to run my_fork */
  if (!IS_ERR(task)) {
    printk("[program2] : Module_init kthread start");
    wake_up_process(task);
  }
  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
