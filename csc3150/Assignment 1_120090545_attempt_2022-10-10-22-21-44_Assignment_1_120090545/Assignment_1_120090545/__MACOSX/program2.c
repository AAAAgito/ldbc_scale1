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
#include <linux/wait.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;

struct wait_opts {
  enum pid_type wo_type; // It is defined in ‘/include/linux/pid.h’.
  int wo_flags;          // Wait options. (0, WNOHANG, WEXITED, etc.)
  struct pid *wo_pid;    // Kernel's internal notion of a process identifier.
                         // “Find_get_pid()”
  struct siginfo __user *wo_info;  // Singal information.
  int __user *wo_stat;             // Child process’s termination status
  struct rusage __user *wo_rusage; // Resource usage
  wait_queue_entry_t child_wait;   // Task wait queue
  int notask_error;
};

extern struct filename *getname_kernel(const char *);

extern pid_t kernel_clone(struct kernel_clone_args *kargs);

extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

int my_WIFSTOPPED(int status) { return (((status)&0xff) == 0x7f); }

int my_wait(pid_t pid) {
  char Signals[65][15] = {"",
                          "SIGHUP",
                          "SIGINT",
                          "SIGQUIT",
                          "SIGILL",
                          "SIGTRAP",
                          "SIGABRT",
                          "SIGBUS",
                          "SIGFPE",
                          "SIGKILL",
                          "SIGUSR1",
                          "SIGSEGV",
                          "SIGUSR2",
                          "SIGPIPE",
                          "SIGALRM",
                          "SIGTERM",
                          "SIGSTKFLT",
                          "SIGCHLD",
                          "SIGCONT",
                          "SIGSTOP",
                          "SIGTSTP",
                          "SIGTTIN",
                          "SIGTTOU",
                          "SIGURG",
                          "SIGXCPU",
                          "SIGXFSZ",
                          "SIGVTALRM",
                          "SIGPROF",
                          "SIGWINCH",
                          "SIGIO",
                          "SIGPWR",
                          "SIGSYS",
                          "SIGRTMIN",
                          "SIGRTMIN+1",
                          "SIGRTMIN+2",
                          "SIGRTMIN+3",
                          "SIGRTMIN+4",
                          "SIGRTMIN+5",
                          "SIGRTMIN+6",
                          "SIGRTMIN+7",
                          "SIGRTMIN+8",
                          "SIGRTMIN+9",
                          "SIGRTMIN+10",
                          "SIGRTMIN+11",
                          "SIGRTMIN+12",
                          "SIGRTMIN+13",
                          "SIGRTMIN+14",
                          "SIGRTMIN+15",
                          "SIGRTMAX-14",
                          "SIGRTMAX-13",
                          "SIGRTMAX-12",
                          "SIGRTMAX-11",
                          "SIGRTMAX-10",
                          "SIGRTMAX-9",
                          "SIGRTMAX-8",
                          "SIGRTMAX-7",
                          "SIGRTMAX-6",
                          "SIGRTMAX-5",
                          "SIGRTMAX-4",
                          "SIGRTMAX-3",
                          "SIGRTMAX-2",
                          "SIGRTMAX-1",
                          "SIGRTMAX"};

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
  wo.wo_stat = (int __user *)status;
  wo.wo_rusage = NULL;

  int state, a;
  a = do_wait(&wo);
  state = (int)wo.wo_stat;
  state %= 128;

  if (my_WIFSTOPPED(wo.wo_stat)) {
    printk("[program2] : child process get SIGSTOP signal\n");
    printk("[program2] : child process stopped\n");
    printk("[program2] : The return signal is 19\n");
  } else {
    if (state == 0)
      printk("[program2] : child process exit normally\n");
    else
      printk("[program2] : get %s signal\n", Signals[state]);
    printk("[program2] : child process terminated\n");
    printk("[program2] : The return signal is %d\n", state);
  }

  put_pid(wo_pid);
  return a;
}

int my_exec(void) {
  int result;
  result = do_execve(
      getname_kernel(
          "/home/vagrant/csc3150/template_source/source/program2/test"),
      NULL, NULL);
  if (result == 0)
    return 0;
  else {
    do_exit(result);
  }
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
  struct kernel_clone_args args = {
      .flags = SIGCHLD,
      .child_tid = NULL,
      .parent_tid = NULL,
      .stack = (unsigned long)&my_exec,
      .stack_size = 0,
      .tls = 0,
      .exit_signal = SIGCHLD,
  };
  pid = kernel_clone(&args);

  /* execute a test program in child process */
  printk("[program2] : The child process has pid = %d\n", pid);
  printk("[program2] : This is the parent process, pid = %d\n",
         (int)current->pid);
  printk("[program2] : child process\n");

  /* wait until child process terminates */
  my_wait(pid);

  return 0;
}

static int __init program2_init(void) {

  /* write your code here */
  printk("[program2] : Module_init 李佳齐 120090545\n");

  /* create a kernel thread to run my_fork */
  printk("[program2] : module_init create kthread start\n");
  task = kthread_create(&my_fork, NULL, "Mythread");
  if (!IS_ERR(task)) {
    printk("[program2] : Module_init kthread starts\n");
    wake_up_process(task);
  }

  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);

/*
sudo su
gcc test.c -o test
insmod program2.ko
rmmod program2.ko
dmesg
*/