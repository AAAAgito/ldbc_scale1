#include <linux/err.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/pid.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static struct task_struct* task;

struct wait_opts
{
    enum pid_type wo_type;
    int wo_flags;
    struct pid* wo_pid;

    struct waitid_info* wo_info;
    int wo_stat;
    struct rusage* wo_rusage;

    wait_queue_entry_t child_wait;
    int notask_error;
};

extern pid_t kernel_clone(struct kernel_clone_args* kargs);
extern struct filename* getname_kernel(const char* filename);
extern long do_wait(struct wait_opts* wo);
extern int do_execve(
    struct filename* filename,
    const char __user* const __user* __argv,
    const char __user* const __user* __envp);


int my_exec(void)
{
    const char path[] = "/home/vagrant/csc3150/Assignment1/Program2/test";
    const char* const argv[] = {path, NULL, NULL};
    const char* const envp[] = {"HOME=/", "PATH=/sbin:/user/sbin:/bin:/usr/bin", NULL};

    printk("[program2] : Child process");

    int result;
    result = do_execve(getname_kernel(path), NULL, NULL);

    if (!result)
    {
        return 0;
    }

    do_exit(result);
    return 0;
}

void my_wait(pid_t pid)
{
    int status;
    int a;
    struct wait_opts wo;
    struct pid* wo_pid = NULL;
    enum pid_type type;
    type = PIDTYPE_PID;
    wo_pid = find_get_pid(pid);

    wo.wo_type = type;
    wo.wo_pid = wo_pid;
    wo.wo_flags = WEXITED;
    wo.wo_info = NULL;
    wo.wo_stat = status;
    wo.wo_rusage = NULL;

    a = do_wait(&wo);

    char sig[10] = "";
    switch (wo.wo_stat & 0x7f)
    {
    case SIGABRT:
        strcpy(sig, "SIGABRT");
        break;
    case SIGALRM:
        strcpy(sig, "SIGALRM");
        break;
    case SIGBUS:
        strcpy(sig, "SIGBUS");
        break;
    case SIGCONT:
        strcpy(sig, "SIGCONT");
        break;
    case SIGCHLD:
        strcpy(sig, "SIGCHLD");
        break;
    case SIGFPE:
        strcpy(sig, "SIGFPE");
        break;
    case SIGHUP:
        strcpy(sig, "SIGHUP");
        break;
    case SIGILL:
        strcpy(sig, "SIGILL");
        break;
    case SIGINT:
        strcpy(sig, "SIGINT");
        break;
    case SIGIO:
        strcpy(sig, "SIGIO");
        break;
    case SIGKILL:
        strcpy(sig, "SIGKILL");
        break;
    case SIGPIPE:
        strcpy(sig, "SIGPIPE");
        break;
    case SIGPWR:
        strcpy(sig, "SIGPWR");
        break;
    case SIGPROF:
        strcpy(sig, "SIGPROF");
        break;
    case SIGQUIT:
        strcpy(sig, "SIGQUIT");
        break;
    case SIGSEGV:
        strcpy(sig, "SIGSEGV");
        break;
    case SIGSTKFLT:
        strcpy(sig, "SIGSTKFLT");
        break;
    case SIGSYS:
        strcpy(sig, "SIGSYS");
        break;
    case SIGTSTP:
        strcpy(sig, "SIGTSTPT");
        break;
    case SIGTERM:
        strcpy(sig, "SIGTERM");
        break;
    case SIGTTOU:
        strcpy(sig, "SIGTTOU");
        break;
    case SIGTRAP:
        strcpy(sig, "SIGTRAP");
        break;
    case SIGTTIN:
        strcpy(sig, "SIGTTIN");
        break;
    case SIGURG:
        strcpy(sig, "SIGURG");
        break;
    case SIGUSR1:
        strcpy(sig, "SIGUSR1");
        break;
    case SIGUSR2:
        strcpy(sig, "SIGUSR2");
        break;
    case SIGVTALRM:
        strcpy(sig, "SIGVTALRM");
    case SIGXCPU:
        strcpy(sig, "SIGXCPU");
        break;
    case SIGXFSZ:
        strcpy(sig, "SIGXFSZ");
        break;
    default:
        break;
    }
    if (sig == "")
    {
        if ((wo.wo_stat & 0x7f) == 0)
        {
            printk("[program2] : The return signal is %d\n", wo.wo_stat);
            printk("[program2] : Child process terminated");
        }
        else if ((wo.wo_stat >> 8) == 19)
        {
            printk("[program2] : get Stop signal");
            printk("[Program2] : The return signal is %d\n", wo.wo_stat >> 8);
            printk("[program2] : Child process STOPS");
        }
    }
    else
    {
       printk("[program2] : get %s signal", sig);
        //printk("[program2] : child process is aborted");
        //printk("[program2] : child process alarmed");
        //printk("[program2] : child process got memory error");
        //printk("[program2] : child process got arthemetic error");
        //printk("[program2] : child process is hung up");
        //printk("[program2] : child process got illegal instructions");
        //printk("[program2] : child process is interrupted");
       // printk("[program2] : child process is killed");
       //printk("[program2] : child process is piped");
       //printk("[program2] : child process quitted");
      //printk("[program2] : child process got segment faults");
       //printk("[program2] : child process stopped");
       //printk("[program2] : child process terminated");
       printk("[program2] : child process is trapped");
        printk("[program2] : The return signal is %d\n", (wo.wo_stat & 0x7f));
    }
    put_pid(wo_pid);

    return;
}

int my_fork(void* argc)
{
    int i;
    struct k_sigaction* k_action = &current->sighand->action[0];
    for (i = 0; i < _NSIG; i++)
    {
        k_action->sa.sa_handler = SIG_DFL;
        k_action->sa.sa_flags = 0;
        k_action->sa.sa_restorer = NULL;
        sigemptyset(&k_action->sa.sa_mask);
        k_action++;
    }

    struct kernel_clone_args args =
        {
            .flags = SIGCHLD,
            .stack = &my_exec,
            .stack_size = 0,
            .parent_tid = NULL,
            .child_tid = NULL,
            .exit_signal = SIGCHLD};

    pid_t pid = kernel_clone(&args);
    printk("[program2] : The child process has pid = %d\n", pid);
    printk("[program2] : The parent process has pid = %d\n", (int)current->pid);
    my_wait(pid);

    return 0;
}

static int __init program2_init(void)
{
    printk("[program2] : Module_init Zhenkun Huo 120090822\n");
    printk("[program2] : Module_init create kernel start\n");
    task = kthread_create(&my_fork, NULL, "MyThread");

    if (!IS_ERR(task))
    {
        printk("[program2] : Kthread starts\n");
        wake_up_process(task);
    }

    return 0;
}

static void __exit program2_exit(void)
{
    printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);