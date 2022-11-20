#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <getopt.h>

extern const char *__progname;
#define PROC_DIR "/proc"
#define ROOT_PID 1
#define COMM_LEN 64
#define MAX_DEPTH 200

typedef struct proc {
	char comm[COMM_LEN + 2 +
		  1]; // add 1 for '\0', another 2 for thread brackets
	pid_t pid;
	pid_t pgid;
	struct child *children;
	struct proc *parent;
	struct proc *next; // next proc in the list
} PROC;

typedef struct child {
	PROC *child;
	struct child *next; // next pointer to proc in the list of child
} CHILD;

static PROC *list = NULL;
static int capacity = 0;
static int width[MAX_DEPTH], more[MAX_DEPTH];
static int comp = 1, pids = 0, pgids = 0, by_pid = 0;
static int output_width = 132;
static int line_pos = 1;

static void print_char(char ch)
{
	line_pos++;
	if (line_pos <= output_width)
		putchar(ch);
	else if (line_pos == output_width + 1) {
		putchar('+');
	}
}

static void print_string(const char *str)
{
	while (*str)
		print_char(*str++);
}

static int print_int(int x)
{
	int digits, div;
	digits = 0;
	for (div = 1; x / div; div *= 10)
		digits++;
	if (!digits)
		digits = 1;
	for (div /= 10; div; div /= 10)
		print_char('0' + (x / div) % 10);
	return digits;
}

static PROC *find_proc(pid_t pid)
{
	PROC *process;

	for (process = list; process; process = process->next)
		if (process->pid == pid)
			return process;
	return NULL;
}

static PROC *new_proc(const char *comm, pid_t pid)
{
	PROC *new;

	if (!(new = malloc(sizeof(PROC)))) {
		perror("malloc");
		exit(1);
	}
	strncpy(new->comm, comm, COMM_LEN + 2);
	new->comm[COMM_LEN + 1] = '\0'; // terminator
	new->pid = pid;
	new->children = NULL;
	new->parent = NULL;
	new->next = list;
	return list = new;
}

static void add_proc(const char *comm, pid_t pid, pid_t ppid, pid_t pgid)
{
	PROC *this, *parent;
	CHILD *new, **process;
	int cmp;

	if (!(this = find_proc(pid)))
		this = new_proc(comm, pid);
	else {
		strcpy(this->comm, comm);
	}
	if (pid == ppid)
		ppid = 0;
	this->pgid = pgid;
	if (!(parent = find_proc(ppid))) {
		parent = new_proc("?", ppid);
	}
	if (!(new = malloc(sizeof(CHILD)))) {
		perror("malloc");
		exit(1);
	}
	new->child = this;
	for (process = &parent->children; *process; process = &(*process)->next)
		if (by_pid) {
			if ((*process)->child->pid > this->pid)
				break;
		} else if ((cmp = strcmp((*process)->child->comm, this->comm)) >
			   0) {
			break;
		}
	new->next = *process;
	*process = new;
	this->parent = parent;
}

static int tree_equal(const PROC *a, const PROC *b)
{
	const CHILD *walk_a, *walk_b;
	int i;

	if (strcmp(a->comm, b->comm))
		return 0;
	for (walk_a = a->children, walk_b = b->children; walk_a && walk_b;
	     walk_a = walk_a->next, walk_b = walk_b->next)
		if (!tree_equal(walk_a->child, walk_b->child))
			return 0;
	return !(walk_a || walk_b);
}

static void print_tree(PROC *current, int level, int rep, int leaf, int last,
		       int closing)
{
	CHILD *process, *next, **scan;
	int lvl, i, add, offset, len, swapped, info, count, comm_len, first;
	const char *tmp, *here;
	char comm_tmp[5];

	assert(closing >= 0);
	if (!current)
		return;
	if (!leaf)
		for (lvl = 0; lvl < level; lvl++) {
			for (i = width[lvl] + 1; i; i--)
				print_char(' ');
			print_string(lvl == level - 1 ?
					     last ? "`-" : "|-" :
					     more[lvl + 1] ? "| " : "  ");
		}
	if (rep < 2)
		add = 0;
	else {
		add = print_int(rep) + 2;
		print_string("*[");
	}
	comm_len = 0;
	for (here = current->comm; *here; here++)
		if (*here == '\\') {
			print_string("\\\\");
			comm_len += 2;
		} else if (*here > ' ' && *here <= '~') {
			print_char(*here);
			comm_len++;
		} else {
			sprintf(comm_tmp, "\\%03o", (unsigned char)*here);
			print_string(comm_tmp);
			comm_len += 4;
		}
	offset = line_pos;
	if (pids) {
		print_char('(');
		(void)print_int(current->pid);
		print_char(')');
	}
	if (pgids) {
		print_char('(');
		(void)print_int(current->pgid);
		print_char(')');
	}
	if (!current->children) {
		while (closing--)
			print_char(']');
		putchar('\n');
		line_pos = 1;
	}
	more[level] = !last;
	width[level] = comm_len + line_pos - offset + add;
	if (line_pos >= output_width) {
		print_string("-+-");
		print_string("+");
		putchar('\n');
		line_pos = 1;
		return;
	}
	first = 1;
	for (process = current->children; process; process = next) {
		count = 0;
		next = process->next;
		if (comp) {
			scan = &process->next;
			while (*scan)
				if (!tree_equal(process->child, (*scan)->child))
					scan = &(*scan)->next;
				else {
					if (next == *scan)
						next = (*scan)->next;
					count++;
					*scan = (*scan)->next;
				}
		}
		if (first) {
			print_string(next ? "-+-" : "---");
			first = 0;
		}
		print_tree(process->child, level + 1, count + 1,
			   process == current->children, !next,
			   closing + (count ? 1 : 0));
	}
}

static void read_proc(void)
{
	DIR *procDir;
	struct dirent *pidDir;
	FILE *file;
	struct stat st;
	char *path, *comm;
	char readbuf[BUFSIZ + 1];
	char *rest, *endptr;
	pid_t pid, ppid, pgid;
	int fd, size;

	if (!(procDir = opendir(PROC_DIR))) {
		perror(PROC_DIR);
		exit(1);
	}
	while ((pidDir = readdir(procDir)) != NULL) {
		pid = strtol(pidDir->d_name, &endptr, 10);
		if (pidDir->d_name[0] != '\0' &&
		    *endptr == '\0') { // condtition for valid number
			if (!(path = malloc(strlen(PROC_DIR) +
					    strlen(pidDir->d_name) + 10)))
				exit(2);
			sprintf(path, "%s/%d/stat", PROC_DIR, pid);
			if (file = fopen(path, "r")) {
				sprintf(path, "%s/%d", PROC_DIR, pid);
				if (stat(path, &st) < 0) {
					perror(path);
					exit(1);
				}
				size = fread(readbuf, 1, BUFSIZ, file);
				if (ferror(file) == 0) {
					readbuf[size] = '\0'; // terminator
					/* construct comm str and handle threads */
					if ((comm = strchr(readbuf, '(')) &&
					    (rest = strrchr(comm, ')'))) {
						++comm;
						*rest = '\0';
						if (sscanf(rest + 2,
							   "%*c %d %d", &ppid,
							   &pgid) ==
						    2) { // handle threads
							DIR *taskDir;
							struct dirent *tidDir;
							char *taskpath;
							int thread;
							if (!(taskpath = malloc(
								      strlen(path) +
								      10)))
								exit(2);
							sprintf(taskpath,
								"%s/task",
								path);
							if (taskDir = opendir(
								    taskpath)) {
								while (tidDir = readdir(
									       taskDir)) {
									if (thread = atoi(
										    tidDir->d_name)) {
										if (thread !=
										    pid) {
											char *threadname;
											char finalpath
												[PATH_MAX +
												 1];
											FILE *tmpfile;
											sprintf(finalpath,
												"%s/%d/stat",
												taskpath,
												thread);
											if (!(threadname = malloc(
												      COMM_LEN +
												      2 +
												      1)))
												exit(2);
											if ((tmpfile = fopen(
												     finalpath,
												     "r")) !=
											    NULL) { // open thread file
												sprintf(threadname,
													"{%.*s}",
													COMM_LEN,
													comm);
												fclose(tmpfile);
											}
											add_proc(
												threadname,
												thread,
												pid,
												pgid);
										}
									}
								}
								(void)closedir(
									taskDir);
							}
							add_proc(comm, pid,
								 ppid, pgid);
						}
					}
				}
				(void)fclose(file);
			}
		}
	}
	(void)closedir(procDir);
}

int main(int argc, char **argv)
{
	PROC *current;
	pid_t pid;
	char termcap_area[1024];
	char *termname, *endptr;
	int c, pid_set;
	char *optstr = "Vngpc";
	struct option options[] = { { "version", 0, NULL, 'V' },
				    { "numeric-sort", 0, NULL, 'n' },
				    { "show-pgids", 0, NULL, 'g' },
				    { "show-pids", 0, NULL, 'p' },
				    { "compact", 0, NULL, 'c' },
				    { 0, 0, 0, 0 } };
	pid = ROOT_PID;
	while ((c = getopt_long(argc, argv, optstr, options, NULL)) != -1)
		switch (c) {
		case 'V':
			printf("pstree (PSmisc) ChenHuaxun\nCopyright (C) 1993-2009 Werner Almesberger and Craig Small\n\nPSmisc comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to redistribute it under\nthe terms of the GNU General Public License.\nFor more information about these matters, see the files named COPYING.\n");
			return 0;
		case 'n':
			by_pid = 1;
			break;
		case 'g':
			pgids = 1;
			comp = 0;
			break;
		case 'p':
			pids = 1;
			comp = 0;
			break;
		case 'c':
			comp = 0;
			break;
		default:
			printf("not support this argument\n");
			printf("only support -V -n -g -p -c\n");
			return 0;
		}
	read_proc();
	print_tree(find_proc(pid), 0, 1, 1, 1, 0);
	return 0;
}
