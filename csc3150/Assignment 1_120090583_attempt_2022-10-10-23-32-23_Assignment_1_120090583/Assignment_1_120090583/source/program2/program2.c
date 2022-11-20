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

struct kernel_clone_args;

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

static struct task_struct *task;

extern struct filename *getname_kernel(const char *filename);
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern int do_execve(struct filename *fname,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern void __noreturn do_exit(long code);

MODULE_LICENSE("GPL");

// my_wait
int my_wait(pid_t pid) {
  printk("begin to run my_wait");
  int status;

  struct wait_opts wo;
  struct pid *mypid = NULL;
  mypid = find_get_pid(pid);

  enum pid_type type;
  type = PIDTYPE_PID;
  wo.wo_type = type;

  wo.wo_flags = WEXITED | WUNTRACED;
  wo.wo_pid = mypid;
  wo.wo_info = NULL;
  wo.wo_stat = status;
  wo.wo_rusage = NULL;

  do_wait(&wo);

  return status;
}

// my_exec
int my_exec(void) {
  printk("begin to run my_exec");
  int result;
  const char *path = "/tmp/test";
  const char *const argv[] = {path, NULL, NULL};

  struct filename *my_filename = getname_kernel(path);

  printk("[program2] : child process");

  result = do_execve(my_filename, argv, NULL);

  if (!result) {
    return 0;
  }
  do_exit(result);
}

// my_fork
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
  struct kernel_clone_args arg;
  arg.flags = ((SIGCHLD | CLONE_VM | CLONE_UNTRACED) & ~CSIGNAL),
  arg.exit_signal = (SIGCHLD & CSIGNAL), arg.stack = (unsigned long)&my_exec;
  arg.stack_size = 0;

  pid_t pid;
  pid = kernel_clone(&arg);

  /* execute a test program in child process */
  printk("[program2] : The child process has pid= %d\n", pid);
  printk("[program2] : The parent process has pid= %d\n", (int)current->pid);

  /* wait until child process terminates */

  my_wait(pid);
  return 0;
}

static int __init program2_init(void) {

  printk("[program2] : Module_init {Yang Qingyuan} {120090583}\n");
  printk("[program2] : Module_init create kthread start\n");

  /* create a kernel thread to run my_fork */
  task = kthread_create(&my_fork, NULL, "MyThread");
  if (!IS_ERR(task)) {
    printk("[program2] : Module_init kthread start\n");
    wake_up_process(task);
  }
  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
