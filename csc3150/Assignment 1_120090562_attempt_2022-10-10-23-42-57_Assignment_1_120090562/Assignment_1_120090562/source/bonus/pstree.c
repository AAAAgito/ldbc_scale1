/**
 * @file pstree.c
 * @author Derong Jin (120090562@link.cuhk.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-10-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

#define DEBUG 0
#define STATIC_READ_TEST 0
#define INT_PID 1
#define TEST2022 0

typedef unsigned long long ullong;

const char *usage[] = {
	"Usage: pstree [-cgptT] [ -n ]",
	"              [ -A ] [ PID ]",
	"   or: pstree -V",
	"",
	"Display a tree of processes.",
	"",
	"  -A, --ascii         use ASCII line drawing characters",
	"  -c, --compact-not   don't compact identical subtrees",
	"  -g, --show-pgids    show process group ids; implies -c",
	"  -n, --numeric-sort  sort output by PID",
	"  -p, --show-pids     show PIDs; implies -c",
	"  -t, --thread-names  show full thread names",
	"  -T, --hide-threads  hide threads, show only processes",
	"  -V, --version       display version information",
	"",
	"  PID    start at this PID; default is 1 (init)",
	NULL
};

struct proc_stat_t {
	int pid;
	char *comm;
	char state;
	int ppid;
	int pgrp;
	int session;
	int tty_nr;
	int tpgid;
	uint flags;
	ulong minflt;
	ulong cminflt;
	ulong majflt;
	ulong cmajflt;
	ulong utime;
	ulong stime;
	long cutime;
	long cstime;
	long priority;
	long nice;
	long num_threads;
	long itrealvalue;
	ullong starttime;
	ulong vsize;
	long rss;
	ulong rsslim;
	ulong startcode; // [PT]
	ulong endcode; // [PT]
	ulong startstack; // [PT]
	ulong kstkesp; // [PT]
	ulong kstkeip; // [PT]
	ulong signal;
	ulong blocked;
	ulong sigignore;
	ulong sigcatch;
	ulong wchan; // [PT]
	ulong nswap;
	ulong cnswap;
	int exit_signal;
	int processor;
	uint rt_priority;
	uint policy;
	ullong delayacct_blkio_ticks;
	ulong guest_time;
	long cguest_time;
	ulong start_data; // [PT]
	ulong end_data; // [PT]
	ulong start_brk; // [PT]
	ulong arg_start; // [PT]
	ulong arg_end; // [PT]
	ulong env_start; // [PT]
	ulong env_end; // [PT]
	int exit_code; // [PT]
};

struct trie_node_t {
	struct trie_node_t **child;
	void *data;
};

struct linked_node_t {
	struct linked_node_t *nextp;
	void *data;
};

struct task_t {
	struct proc_stat_t status;
	struct task_t *nextp;
};

struct process_t {
	pid_t pid;
	int cnt_tasks;
	struct proc_stat_t status;
	struct process_t *nextp;
	struct task_t *tasks;
	struct trie_node_t *root;
	struct linked_node_t *head;
};

struct dir_iter_t {
	DIR *dp;
	struct dirent *ep;
};

typedef struct message_node_t {
	char *message;
	void *head;
	struct proc_stat_t *data;
	int is_task;
	int cnt;
	int nson;
} pnt;

typedef struct edge_t {
	pnt *dst;
	struct edge_t *nextp;
} ent;

enum ORDER { PID_BASE, NAME_BASE, RANDOM };
enum STATE { NORMAL, VERSION, EXIT };

struct args {
	enum ORDER order;
	enum STATE state;
	int compact_not;
	int show_pid;
	int show_gpids;
	int thread_names;
	int hide_threads;
	pid_t root_pid;
} g_args;

typedef struct io_name {
	int first, last;
	struct io_name *nextp;
} io_block;

typedef struct cache {
	io_block *w;
} io_cache;

int cnt_process;
pnt *prt = NULL;
struct process_t *processes, *root, **look_up_array = NULL;

#if STATIC_READ_TEST
const char *const proc_dir = "./test/read/";
const char *const proc_stat_fmt = "./test/read/%d/stat";
#elif INT_PID
// format: /proc/[pid]/stat
const char *const proc_dir = "/proc/";
const char *const proc_stat_int_fmt = "/proc/%d/stat";
const char *const proc_stat_ca_fmt = "/proc/%s/stat";
const char *const task_stat_int_dir = "/proc/%d/task/";
const char *const task_stat_ca_dir = "/proc/%s/task/";
const char *const task_stat_ca_fmt = "/proc/%s/task/%s/stat";
const char *const task_stat_int_fmt = "/proc/%d/task/%d/stat";
#else
// aborted implementation
// const char * const proc_dir = "/proc";
// const char * const proc_stat_fmt = "/proc/%s/stat";

#endif

// helper function
char *fname_at(struct dir_iter_t *dir)
{
	if (dir->ep)
		return dir->ep->d_name;
	return NULL;
}

// helper function
static int ca2int(char *src)
{
	char *c = src;
	int cnt = 0, flag = 1, ans = 0;
	while (*c < '0' || *c > '9') {
		if (*c == '-')
			flag = -1;
		c++;
	}
	while ('0' <= *c && *c <= '9') {
		ans = ans * 10 + *c - '0';
		c++;
	}
	return flag * ans;
}

void adde(pnt *src, pnt *dst)
{
	ent *new_edge = (ent *)malloc(sizeof(ent));
	*new_edge = (ent){ dst, src->head };
	src->head = new_edge;
	src->nson++;
}

int cmp_process_by_id(const void *a, const void *b)
{
	return (*((struct process_t **)a))->pid -
	       (*((struct process_t **)b))->pid;
}

int cmp_pnt_1(const void *a, const void *b)
{
	return strcmp((*((pnt **)a))->message, (*((pnt **)b))->message) > 0;
}

int cmp_pnt(const void *a, const void *b)
{
	if (g_args.order == PID_BASE)
		return (*((pnt **)a))->data->pid - (*((pnt **)b))->data->pid;
	if ((*((pnt **)a))->is_task)
		return 1;
	return strcmp((*((pnt **)a))->data->comm, (*((pnt **)b))->data->comm) >
	       0;
}

void delete_cache(io_cache *cache)
{
	io_block *tmp;
	while (cache->w != NULL) {
		tmp = cache->w;
		cache->w = cache->w->nextp;
		free(tmp);
	}
	cache->w = NULL;
	return;
}

// helper function
int is_pid(char *s)
{
	int i = 0;
	while (*s != '\0') {
		if (*s < '0' || *s > '9')
			return 0;
		s++;
	}
	return 1;
}

// helper function
struct process_t *find_by_pid(pid_t pid)
{
	int l = 0, r = cnt_process - 1, mid; // perform binary search
	if (look_up_array == NULL) {
		// no lookup table error
		return NULL; // TODO: may need further implement
	}
	while (l <= r) {
		mid = (l + r) >> 1;
		if (look_up_array[mid]->pid == pid) {
			return look_up_array[mid];
		}
		if (look_up_array[mid]->pid < pid)
			l = mid + 1;
		else
			r = mid - 1;
	}
	return NULL;
}

void next_item(struct dir_iter_t *dir)
{
	dir->ep = readdir(dir->dp);
}

int finished(struct dir_iter_t *dir)
{
	return dir->ep ? 0 : 1;
}

int init_iter(struct dir_iter_t *dir, const char *const dir_name)
{
	dir->dp = opendir(dir_name);
	if (dir->dp != NULL) {
		dir->ep = readdir(dir->dp);
		return 0;
	}
	return 1; // could not open directory
}

void exit_iter(struct dir_iter_t *dir)
{
	(void)closedir(dir->dp);
}

// pstree -V
void pstree_version(FILE *fp)
{
	if (fp) {
		fprintf(fp, "pstree version -.-\n");
	} else {
		// error
	}
	return;
}

int free_proc_stat(struct proc_stat_t *stat)
{
	if (stat != NULL) {
		free(stat->comm);
	}
	return 0; // free suceeded.
}

int free_process(struct process_t *head)
{
	struct task_t *nextp = NULL;
	free_proc_stat(&(head->status));
	while (head->tasks != NULL) {
		nextp = head->tasks->nextp;
		free(head->tasks);
		head->tasks = nextp;
	}
	free(head);
	return 0;
}

int free_all_process(struct process_t *head)
{
	struct process_t *next_val = NULL;
	while (head != NULL) {
		next_val = head->nextp;
		free_process(head);
		head = next_val;
	}
	return 0;
}

// read process stat (by FILE*)
int fread_proc_stat(FILE *fp, struct proc_stat_t *stat)
{
	int i, tmp_l;
	char c, buffer[1000];
	fscanf(fp, "%d", &(stat->pid));
	c = '\0';
	i = 0;
	while (c != '(')
		c = fgetc(fp);
	c = fgetc(fp);
	while (c != ')') {
		buffer[i++] = c;
		c = fgetc(fp);
	}
	buffer[i] = '\0';
	tmp_l = strlen(buffer);
#if DEBUG
	fprintf(stderr, "%d: (%3d:%3d, %d) %s", stat->pid, tmp_l, i, tmp_l == i,
		buffer);
#endif
	stat->comm = (char *)malloc(sizeof(char) * (tmp_l + 1));
	strcpy(stat->comm, buffer);
	stat->comm[tmp_l] = '\0';
	c = fgetc(fp);
	while (c == ' ')
		c = fgetc(fp);
	stat->state = c;
	fscanf(fp, "%d%d%d%d%d", &(stat->ppid), &(stat->pgrp), &(stat->session),
	       &(stat->tty_nr), &(stat->tpgid));
	fscanf(fp, "%u", &(stat->flags));
	fscanf(fp, "%lu%lu%lu%lu%lu%lu", &(stat->minflt), &(stat->cminflt),
	       &(stat->majflt), &(stat->cmajflt), &(stat->utime),
	       &(stat->stime));
	fscanf(fp, "%ld%ld%ld%ld%ld%ld", &(stat->cutime), &(stat->cstime),
	       &(stat->priority), &(stat->nice), &(stat->num_threads),
	       &(stat->itrealvalue));
	fscanf(fp, "%llu", &(stat->starttime));
	fscanf(fp, "%lu", &(stat->vsize));
	fscanf(fp, "%ld", &(stat->rss));
	fscanf(fp, "%lu%lu%lu%lu%lu%lu%lu%lu%lu%lu%lu%lu%lu", &(stat->rsslim),
	       &(stat->startcode), &(stat->endcode), &(stat->startstack),
	       &(stat->kstkesp), &(stat->kstkeip), &(stat->signal),
	       &(stat->blocked), &(stat->sigignore), &(stat->sigcatch),
	       &(stat->wchan), &(stat->nswap), &(stat->cnswap));
	fscanf(fp, "%d%d", &(stat->exit_signal), &(stat->processor));
	fscanf(fp, "%u%u", &(stat->rt_priority), &(stat->policy));
	fscanf(fp, "%llu", &(stat->delayacct_blkio_ticks));
	fscanf(fp, "%lu", &(stat->guest_time));
	fscanf(fp, "%ld", &(stat->cguest_time));
	fscanf(fp, "%lu%lu%lu%lu%lu%lu%lu", &(stat->start_data),
	       &(stat->end_data), &(stat->start_brk), &(stat->arg_start),
	       &(stat->arg_end), &(stat->env_start), &(stat->env_end));
	fscanf(fp, "%d", &(stat->exit_code));
	return 0; // read success
}

void fprint_proc_stat(FILE *fp, struct proc_stat_t *stat)
{
	fprintf(fp,
		"%d (%s) %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d\n",
		stat->pid, stat->comm, stat->state, stat->ppid, stat->pgrp,
		stat->session, stat->tty_nr, stat->tpgid, stat->flags,
		stat->minflt, stat->cminflt, stat->majflt, stat->cmajflt,
		stat->utime, stat->stime, stat->cutime, stat->cstime,
		stat->priority, stat->nice, stat->num_threads,
		stat->itrealvalue, stat->starttime, stat->vsize, stat->rss,
		stat->rsslim, stat->startcode, stat->endcode, stat->startstack,
		stat->kstkesp, stat->kstkeip, stat->signal, stat->blocked,
		stat->sigignore, stat->sigcatch, stat->wchan, stat->nswap,
		stat->cnswap, stat->exit_signal, stat->processor,
		stat->rt_priority, stat->policy, stat->delayacct_blkio_ticks,
		stat->guest_time, stat->cguest_time, stat->start_data,
		stat->end_data, stat->start_brk, stat->arg_start, stat->arg_end,
		stat->env_start, stat->env_end, stat->exit_code);
	return;
}

void do_read_test()
{
	FILE *fp;
	char buffer[1000];
	struct proc_stat_t stat;
	sprintf(buffer, proc_stat_int_fmt, 1);
	fp = fopen(buffer, "r");
	fread_proc_stat(fp, &stat);
	fclose(fp);
	fp = fopen("./tmp.txt", "w");
	fprint_proc_stat(fp, &stat);
	fclose(fp);
	return;
}

struct task_t *get_tasks(pid_t pid, int *cnt)
{
	char buffer[50];
	int tid;
	FILE *fp;
	struct dir_iter_t it;
	struct task_t *head = NULL, *now;
	(*cnt) = 0;
	sprintf(buffer, task_stat_int_dir, pid);
	//fprintf(stderr, "(%s)\n", buffer);
	for (init_iter(&it, buffer); !finished(&it); next_item(&it)) {
		if (!is_pid(fname_at(&it)))
			continue;
		tid = ca2int(fname_at(&it));
		if (tid == pid)
			continue;
		sprintf(buffer, task_stat_int_fmt, pid, tid);

		fp = fopen(buffer, "r");
		now = (struct task_t *)malloc(sizeof(struct task_t));
		now->nextp = head;
		fread_proc_stat(fp, &(now->status));
		head = now;
		(*cnt)++;
		fclose(fp);
	}
	exit_iter(&it);
	return head;
}

struct process_t *create_process_snap_shot(void)
{
	FILE *fp;
	char file_path[50];
	struct dir_iter_t it;
	struct proc_stat_t stat;
	struct process_t *head = NULL, *now;
	for (init_iter(&it, proc_dir); !finished(&it); next_item(&it)) {
		if (!is_pid(fname_at(&it)))
			continue;
#if DEBUG
		fprintf(stderr, "%p: ", (void *)&it);
#endif
		sprintf(file_path, proc_stat_ca_fmt, fname_at(&it));
#if DEBUG
		fprintf(stderr, "%s -> ", file_path);
#endif
		fp = fopen(file_path, "r");
#if DEBUG
		if (fp == NULL)
			fprintf(stderr, "closed!\n");
#endif

		if (fp == NULL) {
#if TEST2022
			fprintf(stdout, "ERROR: %s\n", file_path);
#endif
			continue;
		}
		now = (struct process_t *)malloc(sizeof(struct process_t));
		now->nextp = head;
		now->root = NULL;
		now->head = NULL;
		now->tasks = NULL;
		fread_proc_stat(fp, &(now->status));
		now->pid = now->status.pid;
		head = now;
		cnt_process++;
#if TEST2022
		fprintf(stdout, "%d's father is %d\n", now->status.pid,
			now->status.ppid);
#endif
		fclose(fp);
		now->tasks = get_tasks(now->pid, &(now->cnt_tasks));
#if DEBUG
		fprintf(stderr, "\n");
#endif
	}
	exit_iter(&it);
	return head;
}

void add_tree_edge(struct process_t *ppi, struct process_t *pi)
{
	struct linked_node_t *node_l;
	struct trie_node_t *node_t;
	switch (g_args.order) {
	case PID_BASE: // break; NOT implemented
	case NAME_BASE: // break; NOT implemented
	case RANDOM:
		// linked_list
		node_l = (struct linked_node_t *)malloc(
			sizeof(struct linked_node_t));
		node_l->nextp = ppi->head;
		node_l->data = (void *)pi;
		ppi->head = node_l;
		break;

	default:
		break;
	}
	return;
}

int fprint_tree(FILE *fp, struct process_t *cur, char *prefix, size_t num_p)
{
	int first = 1, i;
	size_t len;
	struct linked_node_t *pi = NULL;
	len = strlen(cur->status.comm);
	sprintf(prefix + num_p, "%s+", cur->status.comm);

	for (pi = cur->head; pi != NULL; pi = pi->nextp) {
		fprint_tree(fp, pi->data, prefix, num_p + len + 1);
		if (first) {
			// strncpy(prefix + num_p, "                    ", len + 1);
			for (i = 0; i < len; i++)
				prefix[num_p + i] = ' ';
			prefix[num_p + i] = '|';
			first = 0;
		}
	}
	if (cur->head == NULL) {
		prefix[num_p + len] = '\0';
		fprintf(fp, "%s\n", prefix);
		return 0;
	}

	return 0;
}

pnt *task_obj(struct task_t *t, char *p_name)
{
	pnt *node = (pnt *)malloc(sizeof(pnt));
	int len;
	char *name;
	if (g_args.thread_names) {
		len = strlen(t->status.comm);
		name = t->status.comm;
	} else {
		len = strlen(p_name);
		name = p_name;
	}
	node->cnt = 1;
	node->is_task = 1;
	node->head = NULL;
	node->message = (char *)malloc(sizeof(char) * (len + 20));
	node->data = &(t->status);
	if (g_args.show_pid) {
		if (g_args.show_gpids) {
			sprintf(node->message, "{%s}(%d, %d)", name,
				t->status.pid, t->status.pgrp);
		} else {
			sprintf(node->message, "{%s}(%d)", name, t->status.pid);
		}
	} else {
		if (g_args.show_gpids) {
			sprintf(node->message, "{%s}(%d)", name,
				t->status.pgrp);
		} else {
			sprintf(node->message, "{%s}", name);
		}
	}
	return node;
}

pnt *build_message_tree(struct process_t *node_p)
{
	pnt *node = NULL, *chld, **tmp;
	ent *e, *tmp_e;
	static char buf[1000];
	int len, i;
	struct task_t *it;
	struct linked_node_t *pi = NULL;
	if (node_p == NULL)
		return node;
	len = strlen(node_p->status.comm);
	node = (pnt *)malloc(sizeof(pnt));
	node->nson = 0;
	node->head = NULL;
	node->is_task = 0;
	node->cnt = 1;
	node->data = &(node_p->status);
	node->message = (char *)malloc(sizeof(char) * (len + 20));
	if (g_args.show_pid) {
		if (g_args.show_gpids) {
			sprintf(node->message, "%s(%d, %d)",
				node_p->status.comm, node_p->status.pid,
				node_p->status.pgrp);
		} else {
			sprintf(node->message, "%s(%d)", node_p->status.comm,
				node_p->status.pid);
		}
	} else {
		if (g_args.show_gpids) {
			sprintf(node->message, "%s(%d)", node_p->status.comm,
				node_p->status.pgrp);
		} else {
			sprintf(node->message, "%s", node_p->status.comm);
		}
	}
	for (pi = node_p->head; pi != NULL; pi = pi->nextp) {
		chld = build_message_tree(pi->data);
#if TEST2022
		fprintf(stdout, "BM: %d's father is %d\n", chld->data->pid,
			node_p->status.pid);
#endif
		adde(node, chld);
	}
#if DEBUG
	fprintf(stderr, "<<%d>>\n", node_p->cnt_tasks);
#endif
	if (!(g_args.hide_threads || node_p->cnt_tasks == 0)) {
		if (g_args.compact_not) {
			for (it = node_p->tasks; it != NULL; it = it->nextp) {
				adde(node, task_obj(it, node_p->status.comm));
			}
		} else {
			chld = (pnt *)malloc(sizeof(pnt));
			chld->head = NULL;
			chld->cnt = 1;
			chld->data = node->data;
			chld->nson = 0;
			chld->is_task = 1;
			chld->message =
				(char *)malloc(sizeof(char) * (len + 20));
			sprintf(chld->message, "%d*[{%s}]", node_p->cnt_tasks,
				node->message);
			adde(node, chld);
		}
	}

	tmp = (pnt **)malloc(sizeof(pnt *) * node->nson);
	if (!g_args.compact_not) {
		for (e = node->head, i = 0; e != NULL; e = e->nextp, i++)
			tmp[i] = e->dst;
		qsort(tmp, node->nson, sizeof(pnt *), cmp_pnt_1);
		for (e = node->head, i = 0; e != NULL; e = e->nextp, i++)
			e->dst = tmp[i];

		for (e = node->head; e != NULL && e->nextp != NULL;) {
			if (strcmp(e->dst->message, e->nextp->dst->message) ==
				    0 &&
			    (e->dst->head == NULL) &&
			    (e->nextp->dst->head == NULL)) {
				tmp_e = e->nextp;
				e->dst->cnt += e->nextp->dst->cnt;
				e->nextp = e->nextp->nextp;
				node->nson--;
				free(tmp_e->dst->message);
				free(tmp_e->dst);
				free(tmp_e);
			} else {
				e = e->nextp;
			}
		}

		for (e = node->head; e != NULL; e = e->nextp) {
			if (e->dst->cnt != 1) {
				sprintf(buf, "%d*[%s]", e->dst->cnt,
					e->dst->message);
				free(e->dst->message);
				e->dst->message = (char *)malloc(
					sizeof(char) * (strlen(buf) + 1));
				sprintf(e->dst->message, "%s", buf);
				e->dst->cnt = 1;
			}
		}
	}
	for (e = node->head, i = 0; e != NULL; e = e->nextp, i++)
		tmp[i] = e->dst;
	qsort(tmp, node->nson, sizeof(pnt *), cmp_pnt);
	for (e = node->head, i = 0; e != NULL; e = e->nextp, i++) {
		e->dst = tmp[i];
#if TEST2022
		fprintf(stdout, "L: %d's father is %d\n", e->dst->data->pid,
			node->data->pid);
#endif
	}
	e = node->head;
	if (node->nson == 1 && e->dst->head == NULL) {
		e = node->head;
		sprintf(buf, "%s---%s", node->message, e->dst->message);
		free(node->message);
		node->message =
			(char *)malloc(sizeof(char) * (strlen(buf) + 1));
		sprintf(node->message, "%s", buf);
		node->cnt = 1;
		node->head = NULL;
		node->nson = 0;
		tmp_e = e;
		free(tmp_e->dst->message);
		free(tmp_e->dst);
		free(tmp_e);
	}
	free(tmp);
	for (e = node->head, i = 0; e != NULL; e = e->nextp, i++) {
		//e->dst = tmp[i];
#if TEST2022
		fprintf(stdout, "I: %d's father is %d\n", e->dst->data->pid,
			node->data->pid);
#endif
	}

	return node;
}

int init(int argc, char **argv)
{
	int i, err = 0, len, j;
	g_args.order = NAME_BASE;
	g_args.compact_not = 0;
	g_args.hide_threads = 0;
	g_args.show_gpids = 0;
	g_args.show_pid = 0;
	g_args.thread_names = 0;
	g_args.root_pid = 1;
	g_args.state = NORMAL;
	if (argc == 1)
		return 0;
	for (int i = 1; !err && i < argc; i++) {
		if (argv[i][0] == '-') {
			if (strcmp(argv[i], "-V") == 0 ||
			    strcmp(argv[i], "--version") == 0) {
				g_args.state = VERSION;
				return 0;
			} else if (strcmp(argv[i], "-c") == 0 ||
				   strcmp(argv[i], "--compact-not") == 0) {
				g_args.compact_not = 1;
			} else if (strcmp(argv[i], "-g") == 0 ||
				   strcmp(argv[i], "--show-pgids") == 0) {
				g_args.show_gpids = 1;
				g_args.compact_not = 1;
			} else if (strcmp(argv[i], "-n") == 0 ||
				   strcmp(argv[i], "--numeric-sort") == 0) {
				g_args.order = PID_BASE;
			} else if (strcmp(argv[i], "-t") == 0 ||
				   strcmp(argv[i], "--thread-names") == 0) {
				g_args.thread_names = 1;
			} else if (strcmp(argv[i], "-p") == 0 ||
				   strcmp(argv[i], "--show-pids") == 0) {
				g_args.show_pid = 1;
				g_args.compact_not = 1;
			} else if (strcmp(argv[i], "-T") == 0 ||
				   strcmp(argv[i], "--hide-thread") == 0) {
				g_args.hide_threads = 1;
			} else if (strcmp(argv[i], "-A") == 0 ||
				   strcmp(argv[i], "--ascii") == 0) {
				;
			} else {
				if (strlen(argv[i]) == 1) {
					fprintf(stderr,
						"pstree: unrecognized symbol'-'");
					err = 1;
					break;
				}
				len = strlen(argv[i]);
				for (j = 1; !err && j < len; j++) {
					switch (argv[i][j]) {
					case 'c':
						g_args.compact_not = 1;
						break;
					case 'g':
						g_args.show_gpids = 1;
						g_args.compact_not = 1;
						break;
					case 't':
						g_args.thread_names = 1;
						break;
					case 'p':
						g_args.show_pid = 1;
						g_args.compact_not = 1;
						break;
					case 'T':
						g_args.hide_threads = 1;
						break;
					default:
						fprintf(stderr,
							"pstree: invalid/not-implemented option -- '%c'\n",
							*(argv[i] + j));
						err = 1;
						break;
					}
				}
			}
		} else if (is_pid(argv[i])) {
			g_args.root_pid = ca2int(argv[i]);
		} else {
			fprintf(stderr, "USER: not implemented.\n");
			err = 1;
			break;
		}
	}
	if (err) {
		for (int i = 0; usage[i] != NULL; i++) {
			fprintf(stderr, "%s\n", usage[i]);
		}
		return 1;
	}
	return 0;
}

void fprint_message_tree(FILE *fp, pnt *node, char *buffer, int w_pos,
			 int is_root, io_cache *cache)
{
#if TEST2022
	fprintf(stdout, "O: %d\n", node->data->pid);
#endif
	ent *e;
	io_block *new_io;
	int offset = strlen(node->message), start = 1, n_pos, i;
	if (node->head) {
		if (is_root) {
			sprintf(buffer + w_pos, "%s-+", node->message);
			offset += 2;
		} else {
			sprintf(buffer + w_pos, "-%s-+", node->message);
			offset += 3;
		}
	} else {
		if (is_root) {
			sprintf(buffer + w_pos, "%s", node->message);
		} else {
			sprintf(buffer + w_pos, "-%s", node->message);
			offset += 1;
		}
	}
	// sprintf(buffer + w_pos, "%s", node->message);
	n_pos = w_pos + offset;
	new_io = (io_block *)malloc(sizeof(io_block));
	new_io->first = w_pos;
	new_io->last = w_pos + offset - 1;
	new_io->nextp = cache->w;
	cache->w = new_io;
#if TEST2022
	fprintf(stdout, "O: %d\n", node->data->pid);
#endif
	for (e = node->head; e != NULL; e = e->nextp) {
		if (e->nextp == NULL)
			buffer[w_pos + offset - 1] = '`';
		fprint_message_tree(fp, e->dst, buffer, n_pos, 0, cache);
#if TEST2022
		fprintf(stdout, "P: %d's father is %d\n", e->dst->data->pid,
			node->data->pid);
#endif
		if (e->nextp != NULL)
			buffer[w_pos + offset - 1] = '|';
	}
	if (node->head == NULL) {
		fprintf(fp, "%s\n", buffer);
		for (new_io = cache->w; new_io != NULL;
		     new_io = new_io->nextp) {
			for (i = new_io->first; i < new_io->last; i++) {
				buffer[i] = ' ';
			}
			if (buffer[new_io->last] == '|' ||
			    buffer[new_io->last] == '+')
				buffer[new_io->last] = '|';
		}
		for (i = 0; i < w_pos; i++)
			if (buffer[i] == '`')
				buffer[i] = ' ';
		delete_cache(cache);
	}
	// free(node->message);
	// free(node);
}

void listdir(const char *path)
{
	struct dir_iter_t it;
	for (init_iter(&it, proc_dir); !finished(&it); next_item(&it)) {
		fprintf(stdout, "%s\n", fname_at(&it));
	}
}

int main(int argc, char **argv)
{
#if TEST2022
	listdir("/proc");
#endif

#if DEBUG
	fprintf(stderr, "[main] : args(%d): ", argc);

	for (int i = 0; i < argc; i++) {
		fprintf(stderr, "(%d: %s)", i, argv[i]);
	}
	fprintf(stderr, "\n");

#endif
	if (init(argc, argv))
		return 1;
	if (g_args.state == VERSION) {
		pstree_version(stdout);
		return 0;
	}
#if STATIC_READ_TEST
	do_read_test();
#endif
	int i;
	struct process_t *pi, *ppi;
	char prefix_buffer[10000];
	io_cache cache = { NULL };
	processes = create_process_snap_shot();
#if DEBUG
	fprintf(stderr, "read done.\n");
	fprintf(stderr, "check completeness.\n");
	for (pi = processes; pi != NULL; pi = pi->nextp) {
		fprintf(stderr, "[%5d] %s [%c]\n", pi->status.pid,
			pi->status.comm, pi->status.state);
	}
#endif
	look_up_array = (struct process_t **)malloc(sizeof(struct process_t *) *
						    (cnt_process + 5));
	for (pi = processes, i = 0; pi != NULL; pi = pi->nextp, i++)
		look_up_array[i] = pi;
	qsort(look_up_array, cnt_process, sizeof(struct process_t *),
	      cmp_process_by_id);
	// build tree
	for (pi = processes; pi != NULL; pi = pi->nextp) {
		ppi = find_by_pid(pi->status.ppid);
		if (ppi == NULL)
			continue; // zombie
		add_tree_edge(ppi, pi);
	}
	// TODO: root is hard coded
	root = find_by_pid(g_args.root_pid);
	// fprint_tree(stdout, root, prefix_buffer, 0);
	prt = build_message_tree(root);
	fprint_message_tree(stdout, prt, prefix_buffer, 0, 1, &cache);
	free(look_up_array);
	free_all_process(processes);

	return 0;
}