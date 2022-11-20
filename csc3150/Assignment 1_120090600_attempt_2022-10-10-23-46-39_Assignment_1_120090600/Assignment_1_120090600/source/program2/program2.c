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

  /**struct kernel_clone_axgs {
    u64 flags : int __user *pidfd;
    int __user *child_tid;
    int __user *parent_tid;
    int exit signal;
    unsigned long stack;
    unsigned long stack size;
    unsigned long tls;
    pid_t *set_tid;
    size_t set_tid_size;
    int cgroup;
    struct cgroup *cgrpi;
    struct css_set *cset;
  }; 

  return 0;
}*/
void ny_wait(pid_t pid) {

  int status;
  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTVPE_PTD;
  wo_pid = find_get_pid(pid);
  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WEXITED;
  wo.wo_info = NULL;
  wo.wo_stat = (int __user *)&status;
  wo.wo_rusage = NULL;
  int a;
  a = do_wait(&wo);
  printk("do_wait return value is %din", &a);
  // output child process exit status
  printk("[Do_Fork]:The return signal is %d|n", *wo.wo_stat);

  put_pid(wo_pid);

  return;
}

struct wait_opts {
    enum pid_type       wo_type;
    int         wo_flags;
    struct pid      *wo_pid;

    struct siginfo __user   *wo_info;
    int         wo_stat;
    struct rusage __user    *wo_rusage;

    wait_queue_entry_t      child_wait;
    int         notask_error;
};

extern pid_t kernel_clone(struct kernel_clone_args *kargs);

extern long do_wait(struct wait_opts *wo);

extern int do_execve(struct filename *filename,
    const char __user *const __user *__argv,
    const char __user *const __user *__envp);
extern struct filename* getname_kernel(const char* filename);


static int __init program2_init(void) {
  printk("KT module create kthread start\n");
  // create a kthread
  task = kthread_create(&func, NULL, "MyThread");
  // wake up new thread if ok
  if (!IS_ERR(task)) {
    printk("Kthread starts\n");
    wake_up_process(task);
    return 0;
  }

  static void __exit program2_exit(void) {
    printk("[program2] : Module_exit\n");
  }

  module_init(program2_init);
  module_exit(program2_exit);