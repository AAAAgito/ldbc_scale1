#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char* argv[]) {
    // shared memories
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;
    pid_t* sh_pids = mmap(NULL, sizeof(pid_t) * argc, prot, flags, -1, 0);
    int* sh_stats = mmap(NULL, sizeof(int) * argc, prot, flags, -1, 0);

    pid_t pid = fork();

    // parent process
    if (pid != 0) {
        waitpid(pid, sh_stats + 1, WUNTRACED);
        sh_pids[0] = getpid();

        // print process tree
        printf("The process tree: %ld", (long)getpid());
        for (int i = 1; i < argc; ++i) printf("->%ld", (long)sh_pids[i]);
        printf("\n\n");

        // print execution details
        for (int i = argc - 1; i > 0; --i) {
            printf("The process(pid=%ld) of parent(pid=%ld) ", (long)sh_pids[i], (long)sh_pids[i - 1]);
            int status = sh_stats[i];
            if (WIFEXITED(status)) {
                printf("terminates normally\n");
                printf("It's exit status = %d\n\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("is terminated by signal\n");
                int signal = WTERMSIG(status);
                printf("It's signal number = %d (%s)\n\n", signal, strsignal(signal));
            } else if (WIFSTOPPED(status)) {
                printf("is stopped\n");
                int signal = WSTOPSIG(status);
                printf("It's signal number = %d (%s)\n\n", signal, strsignal(signal));
            }
        }

        return 0;
    }

    // child processes
    for (int i = 1; i < argc; ++i) {
        if (i + 1 != argc && (pid = fork()) == 0) continue;

        if (pid != 0) waitpid(pid, sh_stats + i + 1, WUNTRACED);
        sh_pids[i] = getpid();

        char* const args[] = {argv[i], NULL};
        execv(argv[i], args);

        return 0;
    }

    return 0;
}
