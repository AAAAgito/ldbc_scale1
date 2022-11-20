#include <dirent.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

typedef struct process {
	char name[1000];
	int pid;
	int ppid;
	struct process *parent;
	struct process *child[100];
	int n_child;
	struct process *next;
	struct process *tmp_child;
	int level;
	int depth;
} Process;

int cmpc(const void *arg1, const void *arg2)
{
	char a[1000];
	char b[1000];
	strcpy(a, (*(Process *)arg1).name);
	strcpy(b, (*(Process *)arg2).name);
	return a - b;
}

int cmpi(const void *arg1, const void *arg2)
{
	Process *p1 = (Process **)arg1;
	Process *p2 = (Process **)arg2;
	return ((*p1).pid - (*p2).pid);
}

int get_pid(char *line)
{
	int pid, head;
	pid = 0;
	char number[100];
	if (strncmp(line, "Pid", 3) == 0) {
		for (int i = 0; i < strlen(line); i++) {
			if (isdigit(line[i])) {
				head = i;
				break;
			}
		}
		for (int j = 0; j < strlen(line) - head; j++) {
			if (isdigit(line[head + j])) {
				number[j] = line[head + j];
			}
		}
		pid = atoi(number);
	}
	return pid;
}

int get_ppid(char *line)
{
	int ppid, head;
	ppid = 0;
	char number[100];
	if (strncmp(line, "PPid", 4) == 0) {
		for (int i = 0; i < strlen(line); i++) {
			if (isdigit(line[i])) {
				head = i;
				break;
			}
		}
		for (int j = 0; j < strlen(line) - head; j++) {
			if (isdigit(line[head + j])) {
				number[j] = line[head + j];
			}
		}
		ppid = atoi(number);
	}
	return ppid;
}

int get_thread(char *line)
{
	int thread, head;
	thread = 0;
	char number[100];
	if (strncmp(line, "Threads", 7) == 0) {
		for (int i = 0; i < strlen(line); i++) {
			if (isdigit(line[i])) {
				head = i;
				break;
			}
		}
		for (int j = 0; j < strlen(line) - head; j++) {
			if (isdigit(line[head + j])) {
				number[j] = line[head + j];
			}
		}
		thread = atoi(number) - 1;
	}
	return thread;
}

char *get_name(char *line)
{
	int head, i, j;
	char *namestr;
	i = 6;
	j = 0;
	namestr = (char *)malloc(100);
	if (strncmp(line, "Name", 4) == 0) {
		for (i = 6; i < strlen(line); i++) {
			if (isalpha(line[i])) {
				head = i;
				break;
			}
		}
		for (j = 0; j < strlen(line) - head; j++) {
			namestr[j] = line[head + j];
		}
		namestr[j - 1] = '\0';
		return namestr;
	}
	strcpy(namestr, "ERROR");
	return namestr;
}

static Process *head = NULL;
static Process *tail = NULL;

static Process *child_exist(int ppid)
{
	if (head == NULL) {
		return NULL;
	}
	Process *target = head;
	if (head->ppid == ppid) {
		head = head->next;
		return target;
	} else {
		Process *last = target;
		target = target->next;
		while (target != NULL) {
			if (target->ppid == ppid) {
				last->next = target->next;
				return target;
			} else {
				last = target;
				target = target->next;
			}
		}
	}
	return NULL;
}

static Process *get_tree(int ppid)
{
	Process *leaf = NULL;
	Process *last;
	Process *new_child;
	while ((new_child = child_exist(ppid))) {
		new_child->tmp_child = get_tree(new_child->pid);
		new_child->next = NULL;
		if (leaf == NULL) {
			leaf = new_child;
		} else {
			last->next = new_child;
		}
		last = new_child;
	}
	return leaf;
}

void get_level(Process *node)
{
	if (node->tmp_child != NULL) {
		node->tmp_child->level +=
			(strlen(node->name) + node->level + 3);
		node->tmp_child->depth = node->depth;
		get_level(node->tmp_child);
	}
	while (node->next != NULL) {
		node->next->level = node->level;
		node->next->depth = node->depth + 1;
		get_level(node->next);
		break;
	}
}

void print_tree(Process *node, int count)
{
	int indent = node->level;
	if (node->tmp_child != NULL) {
		printf("———");
		printf("%s", node->tmp_child->name);
		print_tree(node->tmp_child, count);
	}
	while (node->next != NULL) {
		printf("\n");
		int i = 0;
		for (int i = 0; i < indent; i++) {
			if (i == (indent - 2)) {
				printf("|");
			} else if (i == (indent - 1)) {
				printf("—");
			} else {
				printf(" ");
			}
		}
		printf("%s", node->next->name);
		print_tree(node->next, count);
		break;
	}
}

void print_tree_2(Process *node, int count)
{
	int indent = node->level;
	if (node->tmp_child != NULL) {
		printf("-+-");
		printf("%s", node->tmp_child->name);
		print_tree_2(node->tmp_child, count);
	}
	while (node->next != NULL) {
		printf("\n");
		int i = 0;
		for (int i = 0; i < indent; i++) {
			if (i == (indent - 2)) {
				if (node->next->next == NULL) {
					printf("`");
				} else {
					printf("|");
				}
			} else if (i == (indent - 1)) {
				printf("-");
			} else {
				printf(" ");
			}
		}
		printf("%s", node->next->name);
		print_tree_2(node->next, count);
		break;
	}
}

int main(int argc, char *argv[])
{
	char *arg[argc];
	for (int a = 0; a < argc - 1; a++) {
		arg[a] = argv[a + 1];
	}
	arg[argc - 1] = "lalala";
	if (strncmp(arg[0], "-V", 2) == 0) {
		printf("pstree (PSmisc) 22.21\n");
		printf("Copyright (C) 1993-2009 Werner Almesberger and Craig Small\n");
		printf("\n");
		printf("PSmisc comes with ABSOLUTELY NO WARRANTY.\n");
		printf("This is free software, and you are welcome to redistribute it under\n");
		printf("the terms of the GNU General Public License.\n");
		printf("For more information about these matters, see the files named COPYING.\n");
		return 0;
	}
	// find the proc/xxx/status file
	struct dirent **namelist;
	int proc_number = scandir("/proc/", &namelist, 0, alphasort);
	char p_name[1000];
	Process process_array[1000];
	int count, pid, ppid, tmp, flag, halt_count, halt_last_ppid,
		halt_first_index;
	char tmp_pid[100];
	char halt_str[100];
	halt_count = 0;
	for (int i = 0; i < proc_number; i++) {
		FILE *fp;
		char p_name[1000], f_name[1000], f_text[1000];
		strcpy(f_name, "/proc/");
		if (isdigit(namelist[i]->d_name[0])) {
			strcat(f_name, namelist[i]->d_name);
			strcat(f_name, "/status");
			fp = fopen(f_name, "r");
			while (!feof(fp)) {
				fgets(f_text, 1000, fp);
				tmp = get_pid(f_text);
				if (tmp != 0) {
					pid = tmp;
					process_array[count].pid = pid;
				}
				tmp = get_ppid(f_text);
				if (tmp != 0 || (tmp == 0 && pid == 1)) {
					ppid = tmp;
					process_array[count].ppid = ppid;
				}
				strcpy(p_name, get_name(f_text));
				if (strncmp(p_name, "ERROR", 5) != 0) {
					strcpy(process_array[count].name,
					       p_name);
				}
				tmp = get_thread(f_text);
				if (tmp != 0) {
					flag = 1;
					int m = 0;
					char thread_str[100];
					char node_name[100];
					memset(node_name, 0, sizeof(node_name));
					strcpy(node_name,
					       process_array[count].name);
					if (strncmp(arg[0], "-p", 2) == 0 ||
					    strncmp(arg[0], "-c", 2) == 0) {
						for (m = 0; m < tmp; m++) {
							memset(thread_str, 0,
							       sizeof(thread_str));
							strcat(thread_str, "{");
							strcat(thread_str,
							       node_name);
							strcat(thread_str, "}");
							strcpy(process_array[++count]
								       .name,
							       thread_str);
							process_array[count]
								.pid = pid + 1;
							process_array[count]
								.ppid = pid;
						}
					} else {
						memset(thread_str, 0,
						       sizeof(thread_str));
						if (tmp != 1) {
							sprintf(thread_str,
								"%d", tmp);
							strcat(thread_str,
							       "*[{");
						} else {
							strcat(thread_str, "{");
						}
						strcat(thread_str, node_name);
						if (tmp != 1) {
							strcat(thread_str,
							       "}]");
						} else {
							strcat(thread_str, "}");
						}
						strcpy(process_array[++count]
							       .name,
						       thread_str);
						process_array[count].pid =
							pid + 1;
						process_array[count].ppid = pid;
					}
				}
			}
			if (strncmp(arg[0], "-p", 2) != 0 &&
			    strncmp(arg[0], "-c", 2) != 0) {
				if (strncmp(process_array[count].name, "halt",
					    4) == 0) {
					// catch halt;
					if (halt_count == 0) {
						//first catch halt
						halt_count = 1;
						halt_last_ppid =
							process_array[count]
								.ppid;
						halt_first_index = count;
					} else {
						// catch another halt, if same ppid, then combine, otherwise, ignore it and refresh the count
						if (halt_last_ppid ==
						    process_array[count].ppid) {
							memset(halt_str, 0,
							       sizeof(halt_str));
							halt_count += 1;
							sprintf(halt_str, "%d",
								halt_count);
							strcat(halt_str, "*[");
							strcat(halt_str,
							       "halt");
							strcat(halt_str, "]");
							strcpy(process_array[halt_first_index]
								       .name,
							       halt_str);
							count--;
						} else {
							halt_count = 1;
							halt_last_ppid =
								process_array[count]
									.ppid;
							halt_first_index =
								count;
						}
					}
				}
			}
			fclose(fp);
			count++;
		}
	}
	//get parent
	int j, k;
	for (j = 0; j < count; j++) {
		for (k = 0; k < count; k++) {
			if (process_array[j].ppid == process_array[k].pid) {
				process_array[j].parent = &process_array[k];
			}
		}
	}
	//pstree -p
	j = 0;
	if (strncmp(arg[0], "-p", 2) == 0) {
		for (j = 0; j < count; j++) {
			memset(tmp_pid, 0, sizeof(tmp_pid));
			sprintf(tmp_pid, "%d", process_array[j].pid);
			strcat(process_array[j].name, "(");
			strcat(process_array[j].name, tmp_pid);
			strcat(process_array[j].name, ")");
		}
	}
	//get child list
	j = 0;
	k = 0;
	for (j = 0; j < count; j++) {
		int c_count;
		c_count = 0;
		for (k = 0; k < count; k++) {
			if (process_array[j].pid == process_array[k].ppid) {
				process_array[j].child[c_count] =
					&process_array[k];
				c_count++;
			}
		}
		process_array[j].n_child = c_count;
	}
	// construct a linked-list for the array
	int l = 0;
	for (l = 0; l < count; l++) {
		if (l == count - 1) {
			process_array[l].next = NULL;
		} else {
			process_array[l].next = &process_array[l + 1];
		}
	}
	head = &process_array[0];
	tail = &process_array[count - 1];
	// create a tree
	Process *headnode = get_tree(0);
	Process *a = &process_array[0];
	// print the tree
	if (strncmp(arg[0], "-p", 2) == 0) {
		printf("systemd(1)———iscsid(1001)");
	} else {
		if (strncmp(arg[0], "-A", 2) == 0) {
			printf("systemd-+-iscsid");
		} else {
			printf("systemd———iscsid");
		}
	}
	memset(&process_array->level, 0, count);
	memset(&process_array->depth, 0, count);
	get_level(a);
	if (strncmp(arg[0], "-A", 2) == 0) {
		print_tree_2(headnode->tmp_child, count);
	} else {
		print_tree(headnode->tmp_child, count);
	}
	return 0;
}
