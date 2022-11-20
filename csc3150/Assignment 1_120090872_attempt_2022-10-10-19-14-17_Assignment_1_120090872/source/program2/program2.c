#include <linux/module.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/jiffies.h>
#include <linux/kmod.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static struct task_struct *task;

// implement fork function
int my_fork(void *argc)
{

	// set default sigaction for current process
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++)
	{
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        printf("[program2] : The child process has pid = %d \n", getpid());
    }
    else
    {
        printf("[program2] : This is the parent process, pid = %d \n", getpid());
    }
    if (pid == 0)
    {
        int i;
        char *args[argc];
        for (i = 0; i < argc - 1; i++)
        {
            args[i] = argv[i + 1];
        }
        args[argc - 1] = NULL;
        printf("[program2] : child process\n");
        execve(argv[1], args, NULL);
        perror("execve");
    }
    else
    {
        int status;
        waitpid(pid, &status, WUNTRACED);
        if (WIFEXITED(status)){}
        else if (WIFSIGNALED(status))
        {
            if (WTERMSIG(status) == 1)
            {
                printf("[program2] : get SIGHUP signal\n");
            }
            else if (WTERMSIG(status) == 2)
            {
                printf("[program2] : get SIGINT signal\n");
            }
            else if (WTERMSIG(status) == 3)
            {
                printf("[program2] : get SIGQUIT signal\n");
            }
            else if (WTERMSIG(status) == 4)
            {
                printf("[program2] : get SIGILL signal\n");
            }
            else if (WTERMSIG(status) == 5)
            {
                printf("[program2] : get SIGTRAP signal\n");
            }
            else if (WTERMSIG(status) == 6)
            {
                printf("[program2] : get SIGABRT signal\n");
            }
            else if (WTERMSIG(status) == 7)
            {
                printf("[program2] : get SIGBUS signal\n");
            }
            else if (WTERMSIG(status) == 8)
            {
                printf("[program2] : get SIGFPE signal\n");
            }
            else if (WTERMSIG(status) == 9)
            {
                printf("[program2] : get SIGKILL signal\n");
            }
            else if (WTERMSIG(status) == 11)
            {
                printf("[program2] : get SIGSEGV signal\n");
            }
            else if (WTERMSIG(status) == 13)
            {
                printf("[program2] : get SIGPIPE signal\n");
            }
            else if (WTERMSIG(status) == 14)
            {
                printf("[program2] : get SIGALRM signal\n");
            }
            else if (WTERMSIG(status) == 15)
            {
                printf("[program2] : get SIGTERM signal\n");
            }
        }
        printf("[program2] : The return signal is %d\n", WEXITSTATUS(status));
    }

	/* fork a process using kernel_clone or kernel_thread */

	/* execute a test program in child process */

	/* wait until child process terminates */

	return 0;
}

static int __init program2_init(void)
{
	task = kthread_create(&my_fork,NULL,"MyFork");

	printk("[program2] : Module_init {Hanqi LIU} {120090872}\n");
	printk("[program2] : Module_init create kthread start\n");
	/* write your code here */

	if (!IS_ERR(task))
	{
		printk("[program2] : Module_init kthread start\n");
		wake_up_process(task);
	}
	/* create a kernel thread to run my_fork */

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
