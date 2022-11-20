#ifndef _BASIC_H
#define _BASIC_H

#include "string.h"
#include "ctype.h"

#define true 1
#define false 0
#define fileSize 2048
#define fileName ent->d_name
#define stat_addr "/proc/%d/stat"
#define task_addr "/proc/%d/task"
#define child_addr "/proc/%d/task/%d/children"
#define fileNotEnd ((fscanf(fp, "%d", &ch)) != EOF)
#define GETPID fscanf(fp, "%d", &ptr->pid)
#define GETNAME fscanf(fp, "%s", temp)
#define GETPPID fscanf(fp, "%d", &ptr->ppid)
#define NORMAL (!output_pid && !no_simple && !highlight && ptr->nthr > 0)

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define SWAP(x, y, t)                                                          \
	({                                                                     \
		t = x;                                                         \
		x = y;                                                         \
		y = t;                                                         \
		t = NULL;                                                      \
	})

char filename[512], procname[256];
int visit[32768];

extern int sort_by_pid;
extern int output_pid;
extern int no_simple;
extern int highlight;
extern int is_ascci;
extern int is_unicode;

typedef struct myProcess {
	int pid, ppid, nson, nthr;
	char name[fileSize];
	struct myProcess *son[128];
	struct myProcess *thr[128];
} Process;

extern int isnumber(char *s);

#endif