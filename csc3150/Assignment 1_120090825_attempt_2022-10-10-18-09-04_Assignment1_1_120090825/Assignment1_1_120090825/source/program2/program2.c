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
  enum pid_type wo_type; // It is defined in ‘/include/linux/pid.h’.
  int wo_flags;          // Wait options. (0, WNOHANG, WEXITED, etc.)
  struct pid *wo_pid;    // Kernel's internal notion of a process identifier.
                         // “Find_get_pid()”
  struct siginfo __user *wo_info; // Singal information.
  int wo_stat; // Child process’s termination status                change to
               // int wo_stat instand of int _user *wo_stat?
  struct rusage __user *wo_rusage; // Resource usage
  wait_queue_entry_t child_wait;   // Task wait queue
  int notask_error;
};

// extern the function

extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);

extern long do_wait(struct wait_opts *wo);

extern struct filename *getname_kernel(const char __user *filename);

extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);

// used for different case of signaled
signed char my_WIFSIGNALED(int status) {
  return (((signed char)(((status & 0x7f) + 1) >> 1)) > 0);
}
// used for output the stop status
int STOP_signal_output(int status) { return ((status & 0xff00) >> 8); }

// the execution of the test.c
int my_exec(void *p) {
  int result_exec;
  const char path[] =
      "/home/vagrant/csc3150/source/program2/test"; // The file path
                                                    // here!!!-----From
                                                    // /home/vagrant/csc3150/source/program2/test-----change
                                                    // to /tmp/test---
  struct filename *my_filename = getname_kernel(path);

  printk("[program2] : child process");

  result_exec = do_execve(my_filename, NULL, NULL);

  if (!result_exec) {
    return (0);
  }

  do_exit(result_exec);
}

// wait part
void my_wait(pid_t pid) {
  int status;

  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);

  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WEXITED | WSTOPPED; // add WSTOPPED
  wo.wo_info = NULL;
  wo.wo_stat = status; //
  wo.wo_rusage = NULL;

  int a;
  a = do_wait(&wo);

  // normal part
  // printk("%d",wo.wo_stat);//check
  if ((wo.wo_stat & 0x7f) == 0) {
    printk("[program2] : child process normally exit!\n");
    printk("[program2] : The return signal is 0\n");
  }

  // stop part
  else if ((wo.wo_stat & 0xff) == 127) {
    int stop_output = STOP_signal_output(wo.wo_stat);
    printk("[program2] : Process stop!\n");
    printk("[program2] : get SIGSTOP signal\n");
    printk("[program2] : The return signal of stop is %d",
           stop_output); // parameter too much?

  }

  else if (my_WIFSIGNALED(wo.wo_stat)) {

    switch (wo.wo_stat & 127) // *
    {

    // hangup
    case 1:
      printk("[program2] : get SIGHUP signal\n");
      printk("[program2] : child process hang up!\n");
      printk("[program2] : The return signal is 1\n");
      break;

    // interrupt
    case 2:
      printk("[program2] : get SIGINT signal\n");
      printk("[program2] : child process interrupt!\n");
      printk("[program2] : The return signal is 2\n");
      break;

    // quit
    case 3: ////////
      printk("[program2] : get SIGQUIT signal\n");
      printk("[program2] : child process quit!\n");
      printk("[program2] : The return signal is 3\n");
      break;

    // illegal_instr
    case 4: ////////
      printk("[program2] : get SIGILL signal\n");
      printk("[program2] : child process get the illegal instruction!\n");
      printk("[program2] : The return signal is 4\n");
      break;

    // trap
    case 5: ////////
      printk("[program2] : get SIGTRAP signal\n");
      printk("[program2] : child process trap!\n");
      printk("[program2] : The return signal is 5\n");
      break;

    // abort
    case 6: ////////
      printk("[program2] : get SIGABRT signal\n");
      printk("[program2] : child process abort!\n");
      printk("[program2] : The return signal is 6\n");
      break;

    // bus
    case 7: ////////
      printk("[program2] : get SIGBUS signal\n");
      printk("[program2] : child process get bus error here!\n");
      printk("[program2] : The return signal is 7\n");
      break;

    // floating
    case 8: //////
      printk("[program2] : get SIGFPE signal\n");
      printk("[program2] : child process gets floating error here!\n");
      printk("[program2] : The return signal is 8\n");
      break;

    // kill
    case 9:
      printk("[program2] : get SIGKILL signal\n");
      printk("[program2] : child process kill!\n");
      printk("[program2] : The return signal is 9\n");
      break;

    // NO 10

    // segment_fault
    case 11: /////
      printk("[program2] : get SIGSEGV signal\n");
      printk("[program2] : child process gets segment fault!\n");
      printk("[program2] : The return signal is 11\n");
      break;

    // NO 12

    // pipe
    case 13:
      printk("[program2] : get SIGPIPE signal\n");
      printk("[program2] : child process gets pipe error here!\n");
      printk("[program2] : The return signal is 13\n");
      break;

    // alarm
    case 14:
      printk("[program2] : get SIGALRM signal\n");
      printk("[program2] : child process gets alarm error here!\n");
      printk("[program2] : The return signal is 14\n");
      break;

    // terminate
    case 15:
      printk("[program2] : get SIGTERM signal\n");
      printk("[program2] : child process terminate!\n");
      printk("[program2] : The return signal is 15\n");
      break;

    default:
      // default for other case
      printk("The signal is not right in the samples!\n");
      break;
    }

  }

  else {
    // For continue
    printk("[program2] : child continue!\n");
  }

  put_pid(wo_pid); // decrease the count and free memory

  return;
}

// implement fork function
int my_fork(void *argc) {

  // set default sigaction for current process
  int i;

  pid_t pid;

  struct k_sigaction *k_action = &current->sighand->action[0];
  for (i = 0; i < _NSIG; i++) {
    k_action->sa.sa_handler = SIG_DFL;
    k_action->sa.sa_flags = 0;
    k_action->sa.sa_restorer = NULL;
    sigemptyset(&k_action->sa.sa_mask);
    k_action++;
  }

  /* fork a process using kernel_clone or kernel_thread */

  pid = kernel_thread(&my_exec, NULL, SIGCHLD);

  printk("[program2] : The child process has pid:%d \n", pid);
  printk("[program2] : This is the parent process, pid:%d \n",
         (int)current->pid);

  /* execute a test program in child process */

  /* wait until child process terminates */

  my_wait(pid);

  return 0;
}

static int __init program2_init(void) {

  printk("[program2] : Module_init {Zhang fengyu} {120090825}");
  /* write your code here */

  /* create a kernel thread to run my_fork */
  printk("[program2] : module_init create kthread start");
  task = kthread_create(&my_fork, NULL, "Mythread");

  // wake up new thread if ok
  if (!IS_ERR(task)) {
    printk("[program2] : module_init kthread start\n");
    wake_up_process(task);
  }

  return 0;
}

static void __exit program2_exit(void) { printk("[program2] : Module_exit\n"); }

module_init(program2_init);
module_exit(program2_exit);
