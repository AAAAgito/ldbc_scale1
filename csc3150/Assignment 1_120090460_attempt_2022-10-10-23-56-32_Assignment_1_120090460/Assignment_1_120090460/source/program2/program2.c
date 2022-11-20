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

static struct task_struct *task;

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

extern struct filename *getname_kernel(const char *filename);
extern pid_t kernel_clone(struct kernel_clone_args *kargs);
extern long do_wait(struct wait_opts *wo);
extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
// implement fork function

void my_wait(pid_t pid) {
  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTYPE_PID;
  //    look up a hash table and return a value
  wo_pid = find_get_pid(pid);
  int status;

  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WEXITED | WUNTRACED;
  wo.wo_info = NULL;
  wo.wo_stat = status;
  wo.wo_rusage = NULL;

  int waitValue;
  waitValue = do_wait(&wo);

  switch (wo.wo_stat & 0x7f) {
  case 1: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGHUP signal\n");
    printk("[program2] : Child Process is hung up");
    break;
  }
  case 2: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGINT signal\n");
    printk("[program2] : Terminal interrupts");
    break;
  }
  case 3: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGQUIT signal\n");
    printk("[program2] : Terminal quits");
    break;
  }
  case 4: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGILL signal\n");
    printk("[program2] : Child Process gets illegal instructions");
    break;
  }
  case 5: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGTRAP signal\n");
    printk("[program2] : Child Process gets trap error");
    break;
  }
  case 6: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGABRT signal\n");
    printk("[program2] : Child Process is aborted");
    break;
  }
  case 7: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGBUS signal\n");
    printk("[program2] : Child Process gets bus error");
    break;
  }
  case 8: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGFPE signal\n");
    printk("[program2] : Child Process gets floating point exception");
    break;
  }
  case 9: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGKILL signal\n");
    printk("[program2] : Child Process is killed");
    break;
  }
  case 10: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGUSRL signal\n");
    // printk("Terminal interrupts");
    break;
  }
  case 11: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGSEGV signal\n");
    printk("[program2] : Child Process has segmentation error");
    break;
  }
  case 12: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGUSR2 signal\n");
    // printk("Child Process gets illegal instructions");
    break;
  }
  case 13: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGPIPE signal\n");
    printk("[program2] : Child Process has pipe error");
    break;
  }
  case 14: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGALRM signal\n");
    printk("[program2] : Child Process alarms");
    break;
  }
  case 15: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGTERM signal\n");
    printk("[program2] : Child Process terminates");
    break;
  }
  case 16: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGTKFLT signal\n");
    // printk("Child Process gets illegal instructions");
    break;
  }
  case 17: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGCHLD signal\n");
    // printk("Terminal interrupts");
    break;
  }
  case 18: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGCONT signal\n");
    // printk("Terminal quits");
    break;
  }

  case 20: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGTSTP signal\n");
    // printk("Child Process is hung up");
    break;
  }
  case 21: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGTTIN signal\n");
    // printk("Terminal interrupts");
    break;
  }
  case 22: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGTTOU signal\n");
    // printk("Terminal quits");
    break;
  }
  case 23: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGURG signal\n");
    // printk("Child Process gets illegal instructions");
    break;
  }
  case 24: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    // printk("Child Process is hung up");
    break;
  }
  case 25: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGXFSZ signal\n");
    // printk("Terminal interrupts");
    break;
  }
  case 26: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGVTALRM signal\n");
    // printk("Terminal quits");
    break;
  }
  case 27: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGPROF signal\n");
    // printk("Child Process gets illegal instructions");
    break;
  }
  case 28: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGWINCH signal\n");
    // printk("Child Process is hung up");
    break;
  }
  case 29: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGIO signal\n");
    // printk("Terminal interrupts");
    break;
  }
  case 30: {
    printk("[program2] : Child Process gets SIGPWR signal\n");
    // printk("Terminal quits");
    break;
  }
  case 31: {
    printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
    printk("[program2] : Child Process gets SIGSYS signal\n");
    // printk("Child Process gets illegal instructions");
    break;
  }

  default: {
    if ((wo.wo_stat & 0x7f) == 0) {
      printk("[Program2] : The return signal is %d\n", wo.wo_stat & 0x7f);
      printk("[program2] : Child Process terminated normally");
    } else if ((wo.wo_stat >> 8) == 19) {
      printk("[Program2] : The return signal is %d\n", wo.wo_stat >> 8);
      printk("[program2] : Child Process STOPS");
      printk("[program2] : Child Process gets SIGSTOP signal\n");
    }
  }
  }

  put_pid(wo_pid);
  //    decrease the count and freed up the memory

  return;
}

int my_execve(void) {
  const char path[] = "/tmp/test";
  const char *const argv[] = {path, NULL, NULL};
  const char *const envp[] = {"HOME=/", "PATH=/sbin:/user/sbin:/bin:/usr/bin",
                              NULL};

  // struct filename *my_file = getname_kernel(path);
  printk("[program2] : child process");
  int result;
  result = do_execve(getname_kernel(path), NULL, NULL);

  if (!result) {
    return 0;
  }

  do_exit(result);
  return 0;
}

int my_fork(void *argc) {
  // set default sigaction for current process

  // current is a Macro which is get_crrent()
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
  struct kernel_clone_args args = {.flags = SIGCHLD,
                                   .exit_signal = SIGCHLD,
                                   .stack = &my_execve,
                                   .stack_size = 0,
                                   .parent_tid = NULL,
                                   .child_tid = NULL};

  /* execute a test program in child process */
  pid_t pid = kernel_clone(&args);
  printk("[program2] : The child process has pid = %d\n", pid);
  printk("[program2] : The parent process has pid = %d\n", (int)current->pid);

  /* wait until child process terminates */
  my_wait(pid);
  return 0;
}

static int __init program2_init(void) {

  printk("[program2] : Module_init Zhu_Luokai 120090460\n");

  /* create a kernel thread to run my_fork */
  printk("[program2] : Module_init create kthread start\n");
  task = kthread_create(&my_fork, NULL, "MyThread");

  /* Wake up new Thread if OK */
  if (!IS_ERR(task)) {
    // printk("Kthread starts\n");
    wake_up_process(task);
  }
  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
