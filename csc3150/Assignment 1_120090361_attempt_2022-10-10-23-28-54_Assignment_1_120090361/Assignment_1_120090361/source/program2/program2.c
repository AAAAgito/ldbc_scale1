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

// copy the declarations of functions and structs
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

extern pid_t kernel_clone(struct kernel_clone_args *args);

extern long do_wait(struct wait_opts *wo);

extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);

extern struct filename *getname_kernel(const char *filename);

// declare the structs
static struct task_struct *task;

// the function which handles signal returned in my_wait function
void my_signal_handle(int status) {
  switch (status & (0x7f)) {
  // WTERMSIG
  case SIGHUP: // 1,hangup
    printk("[program2] : Get SIGHUP signal\n");
    printk("[program2] : Child process hanged up\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGINT: // 2,interrupt
    printk("[program2] : Get SIGINT signal\n");
    printk("[program2] : Child process was interrupted\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGQUIT: // 3,quit
    printk("[program2] : Get SIGQUIT signal\n");
    printk("[program2] : Child process quitted\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGILL: // 4,illegal_instr
    printk("[program2] : Get SIGILL signal\n");
    printk("[program2] : Child process found illegal instruction\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGTRAP: // 5,trap
    printk("[program2] : Get SIGTRAP signal\n");
    printk("[program2] : Child process was trapped\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGABRT: // 6,abort
    printk("[program2] : Get SIGABRT signal\n");
    printk("[program2] : Child process was aborted\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGBUS: // 7,bus
    printk("[program2] : Get SIGBUS signal\n");
    printk("[program2] : Child process encountered BUS error\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGFPE: // 8,floating
    printk("[program2] : Get SIGFPE signal\n");
    printk("[program2] : Child process encountered floating-point exception\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGKILL: // 9,kill
    printk("[program2] : Get SIGKILL signal\n");
    printk("[program2] : Child process was killed\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGSEGV: // 11,segment_fault
    printk("[program2] : Get SIGSEGV signal\n");
    printk("[program2] : Child process encountered segmentation violation\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGPIPE: // 13,pipe
    printk("[program2] : Get SIGPIPE signal\n");
    printk("[program2] : Child process encountered broken pipe\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGALRM: // 14,alarm
    printk("[program2] : Get SIGALRM signal\n");
    printk("[program2] : Child process alarmed\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  case SIGTERM: // 15,terminate
    printk("[program2] : Get SIGTERM signal\n");
    printk("[program2] : Child process terminated\n");
    printk("[program2] : The return signal is %d\n", status & (0x7f));
    break;
  default:
    // WIFSTOPPED
    if ((status & 0xff) == 0x7f) {
      printk("[program2] : Get SIGSTOP signal\n");
      printk("[program2] : Child process stopped\n");
      printk("[program2] : The return signal is %d\n", 19);
    }
    // WIFEXITED
    else {
      printk("[program2] : Normal termination\n");
      printk("[program2] : Child process stopped\n");
      printk("[program2] : The return signal is %d\n", status >> 8);
    }
    break;
  }
  return;
}

// implement my execve function
int my_execve(void) {

  int result;
  const char path[] = "/tmp/test";
  struct filename *flname = getname_kernel(path);
  printk("[program2] : Child process\n");
  result = do_execve(flname, NULL, NULL);
  if (!result)
    return 0;
  return 0;
}

// implement my wait function
void my_wait(pid_t pid) {

  long val;
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

  val = do_wait(&wo);
  my_signal_handle(wo.wo_stat);

  put_pid(wo.wo_pid);
  return;
}

// implement fork function
int my_fork(void *argc) {

  int i;
  pid_t pid;
  struct k_sigaction *k_action = &current->sighand->action[0];
  // create kernel_clone_args
  struct kernel_clone_args kargs = {
      .flags = SIGCHLD,
      .pidfd = NULL,
      .child_tid = NULL,
      .parent_tid = NULL,
      .exit_signal = SIGCHLD,
      .stack = (unsigned long)&my_execve,
      .stack_size = (unsigned long)0,
      .tls = (unsigned long)0,
  };

  // set default sigaction for current process
  for (i = 0; i < _NSIG; i++) {
    k_action->sa.sa_handler = SIG_DFL;
    k_action->sa.sa_flags = 0;
    k_action->sa.sa_restorer = NULL;
    sigemptyset(&k_action->sa.sa_mask);
    k_action++;
  }

  /* fork a process using kernel_clone or kernel_thread */
  pid = kernel_clone(&kargs);
  printk("[program2] : The child process has pid = %d\n", (int)pid);
  printk("[program2] : This is the parent process, pid = %d\n",
         (int)current->pid);

  /* execute a test program in child process */

  /* wait until child process terminates */
  my_wait(pid);

  return 0;
}

static int __init program2_init(void) {

  printk("[program2] : Module_init {Jiarui Chen} {120090361}\n");

  /* write your code here */
  /* create a kernel thread to run my_fork */
  printk("[program2] : Module_init create kthread start\n");
  task = kthread_create(&my_fork, NULL, "my_kthread");
  if (!IS_ERR(task)) {
    printk("[program2] : Module_init kthread start\n");
    wake_up_process(task);
  }

  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
