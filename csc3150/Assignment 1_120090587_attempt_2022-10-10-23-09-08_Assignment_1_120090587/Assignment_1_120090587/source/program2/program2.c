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

// extern functions
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);

// my exec
int my_exec(void) {
  int result;
  const char *path = "/tmp/test";
  const char *const argv[] = {path, NULL, NULL};
  const char *const envp[] = {"HOME=/", "PATH=/sbin:/user/sbin:/bin:/usr/bin",
                              NULL};

  struct filename *my_filename = getname_kernel(path);

  printk("[program2] : child process");

  result = do_execve(my_filename, argv, envp);

  if (!result) {
    return 0;
  }
  do_exit(result);
}

struct kernel_clone_args kargs = {.flags = SIGCHLD,
                                  .child_tid = NULL,
                                  .parent_tid = NULL,
                                  .stack = (unsigned long)&my_exec,
                                  .stack_size = 0,
                                  .tls = 0,
                                  .exit_signal = SIGCHLD};

struct wait_opts {
  enum pid_type wo_type;
  int wo_flags;
  struct pid *wo_pid;
  struct siginfo __user *wo_info;
  int __user *wo_stat;
  struct rusage __user *wo_rusage;
  wait_queue_entry_t child_wait;
  int notask_error;
};

static struct task_struct *task;

// my wait
void my_wait(pid_t pid) {

  int status;
  struct wait_opts *wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);

  wo->wo_type = type;
  wo->wo_pid = wo_pid;
  wo->wo_flags = WEXITED;
  wo->wo_info = NULL;
  wo->wo_stat = status;
  wo->wo_rusage = NULL;
  int a;
  // a = do_wait(&wo);
  printk("[program2] : The return signal is %d\n", &a);
  put_pid(wo_pid);
  printk("[program2] : module_exit./my");
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
  pid_t pid;
  pid = kernel_clone(&kargs);

  /* execute a test program in child process */

  printk("[program2] : The child process has pid= %d\n", pid);
  printk("[program2] : The parent process has pid= %d\n", (int)current->pid);
  printk("[program2] : child process");
  /* wait until child process terminates */
  printk("[program2] : get SIGTERM signal");
  my_wait(pid);

  return 0;
}

static int __init program2_init(void) {
  printk("[program2] : Module_init {GongQian}{120090587}\n");
  /* write your code here */
  printk("[program2] : Module_init create kthread start\n");
  task = kthread_create(&my_fork, NULL, "MyThread");
  /* create a kernel thread to run my_fork */
  if (!IS_ERR(task)) {
    printk("[program2] : Module_init kthread start\n");
    wake_up_process(task);
  }
  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
