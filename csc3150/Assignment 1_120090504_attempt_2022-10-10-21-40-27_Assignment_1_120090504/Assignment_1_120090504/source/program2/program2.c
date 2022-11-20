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

// struct kernel_clone_args
// {
// 	u64 flags;
// 	int __user *pidfd;
// 	int __user *child_tid;
// 	int __user *parent_tid;
// 	int exit_signal;
// 	unsigned long stack;
// 	unsigned long stack_size;
// 	unsigned long tls;
// 	pid_t *set_tid;
// 	size_t set_tid_size;
// 	int cgroup;
// 	struct cgroup *cgrp;
// 	struct css_set *cset;
// };

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
extern int do_execve(struct filename *filename,
                     const char __user *const __user *__argv,
                     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);

static struct task_struct *process;

int my_wait(pid_t pid) {
  int status;
  struct wait_opts wo;
  struct pid *wo_pid = NULL;
  enum pid_type type;
  type = PIDTYPE_PID;
  wo_pid = find_get_pid(pid);

  wo.wo_type = type;
  wo.wo_pid = wo_pid;
  wo.wo_flags = WEXITED; //| WUNTRACED;
  wo.wo_info = NULL;
  wo.wo_stat = status;
  wo.wo_rusage = NULL;

  int dowait;
  dowait = do_wait(&wo);
  // printk("dowait : %d." , dowait);

  put_pid(wo_pid);

  return wo.wo_stat;
}

int my_exec(void) {
  int result;

  const char path[] = "/tmp/test";

  result = do_execve(getname_kernel(path), NULL, NULL);

  // printk("myexec : %d.",result);

  if (!result) {
    return 0;
  }
  // do_exit(result);
  return 0;
}

// implement fork function

int my_fork(void *argc) {
  // set default sigaction for current process

  pid_t pid;

  struct kernel_clone_args tasm;

  tasm.flags = SIGCHLD;
  tasm.exit_signal = SIGCHLD;
  tasm.stack = (unsigned long)&my_exec;
  tasm.stack_size = 0;
  tasm.parent_tid = NULL;
  tasm.child_tid = NULL;
  tasm.tls = 0;

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
  // printk("fa q ta: %d",pid);
  pid = kernel_clone(&tasm);
  // printk("ka q ta again");
  /* execute a test program in child process */

  printk("[program2] : The child process has pid= %d\n", pid);
  printk("[program2] : This is the parentprocess, pid = %d\n",
         (int)current->pid);
  printk("[program2] : child process");

  /* wait until child process terminates */

  // my_wait(pid);

  int signal;
  int terminate;
  // printk("[program2] : Read to wait");
  signal = my_wait(pid);
  // printk("[program2] : signal value = %d", signal);

  if (signal == 0) {
    printk("[program2] : child process gets normal termination\n");
    printk("[program2] : The return signal is %d", signal);
  } else {
    terminate = signal & 0x7f;
    switch (signal) {
    case 1:
      printk("[program2] : get SIGHUP signal\n");
      printk("[program2] : child process is hung up\n");
      printk("[program2] : The return signal is 1\n");
      break;

    case 2:
      printk("[program2] : get SIGINT signal\n");
      printk("[program2] : terminal interrupt\n");
      printk("[program2] : The return signal is 2\n");
      break;

    case 131:
      printk("[program2] : get SIGQUIT signal\n");
      printk("[program2] : terminal quit\n");
      printk("[program2] : The return signal is 3\n");
      break;

    case 132:
      printk("[program2] : get SIGILL signal\n");
      printk("[program2] : child process has illegal instruction error\n");
      printk("[program2] : The return signal is 4\n");
      break;

    case 133:
      printk("[program2] : get SIGTRAP signal\n");
      printk("[program2] : child process has trap error\n");
      printk("[program2] : The return signal is 5\n");
      break;

    case 134:
      printk("[program2] : get SIGABRT signal\n");
      printk("[program2] : child process has abort error\n");
      printk("[program2] : The return signal is 6\n");
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

    case 9:
      printk("[program2] : get SIGKILL signal\n");
      printk("[program2] : child process is killed\n");
      printk("[program2] : The return signal is 9\n");
      break;

    case 139:
      printk("[program2] : get SIGSEGV signal\n");
      printk("[program2] : child process has segmentation fault error\n");
      printk("[program2] : The return signal is 11\n");
      break;

    case 13:
      printk("[program2] : get SIGPIPE signal\n");
      printk("[program2] : child process has pipe error\n");
      printk("[program2] : The return signal is 13\n");
      break;

    case 14:
      printk("[program2] : get SIGALARM signal\n");
      printk("[program2] : child process has alarm error\n");
      printk("[program2] : The return signal is 14\n");
      break;

    case 15:
      printk("[program2] : get SIGTERM signal\n");
      printk("[program2] : child process is terminated\n");
      printk("[program2] : The return signal is 15\n");
      break;

    case 4991:
      printk("[program2] : get SIGSTOP signal\n");
      printk("[program2] : child process stopped\n");
      printk("[program2] : The return signal is 17\n");
      break;

    default:
      printk("[program2] : get SIGCONT signal\n");
      printk("[program2] : child process countinued\n");
      printk("[program2] : The return signal is 19\n");
      break;
    }

    printk("%d", signal);
    printk("%d", terminate);
  }
  return 0;
}

static int __init program2_init(void) {
  printk("[program2] : Module_init Song Chenghao 120090504\n");

  /* write your code here */

  /* create a kernel thread to run my_fork */
  printk("[program2] : Module_init create kthread start\n");

  process = kthread_create(&my_fork, NULL, "Mythread");

  if (!IS_ERR(process)) {
    printk("[program2] : Module_init kthread starts\n");
    wake_up_process(process);
  }

  return 0;
}

static void __exit program2_exit(void) {
  printk("[program2] : Module_exit./my\n");
}

module_init(program2_init);
module_exit(program2_exit);
