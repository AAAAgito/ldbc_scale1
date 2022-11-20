#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct process {
	char *p_name;
	char *p_pid;
	struct process *bro;
	struct process *son;
};

typedef struct process process;

int list_dir(const char *, const char *, process **, int, int);
int check_digit(const char *);
int build_tree(const char *, process *, int);
process *create_process(const char *, const char *);
int check_repeat(process *, process *);
process *get_last_child(process *);
int has_bro(process *);
int insert(process *, process *, const char *);
int print_tr(process *, int, int *, int);
int print_tr2(process *, int, int *);

// open the directory and read the file.
int list_dir(const char *path, const char *father, process **head, int dep,
	     int flag)
{
	DIR *dir;
	struct dirent *entry;
	dir = opendir(path);
	if (dir == NULL) {
		perror("fail to open the directory\n");
		return -1;
	}
	// create the starting process
	if (dep == 0) {
		*head = create_process("", "0");
	}
	while ((entry = readdir(dir))) {
		if (DT_DIR == entry->d_type) {
			if (check_digit(entry->d_name) != -1) {
				if (dep == 1 &&
				    !strcmp(entry->d_name, father)) {
					continue;
				}
				build_tree(entry->d_name, *head, dep);
				if (dep == 0 && flag) {
					char buf[1024];
					strcpy(buf, "/proc/");
					strcat(buf, entry->d_name);
					strcat(buf, "/task");
					list_dir(buf, entry->d_name, head, 1,
						 flag);
				}
			}
		}
	}
	closedir(dir);
	return 0;
}

int check_digit(const char *m)
{
	int flag = 0;
	for (; *m; ++m) {
		if (*m < '0' || *m > '9') {
			return -1;
		}
	}
	return 0;
}

// build the entire tree structure
int build_tree(const char *path_name, process *head, int depth)
{
	FILE *fp;
	char charc[1024];
	strcpy(charc, "/proc/");
	strcat(charc, path_name);
	strcat(charc, "/status");
	fp = fopen(charc, "r");
	char pid[1024], ppid[1024], name[1024];
	while ((fscanf(fp, "%s", charc)) != EOF) {
		if (strcmp(charc, "Pid:") == 0) {
			fscanf(fp, "%s", pid);
		}
		if (strcmp(charc, "PPid:") == 0) {
			fscanf(fp, "%s", ppid);
		}
		if (strcmp(charc, "Name:") == 0) {
			fscanf(fp, "%s", name);
		}
	}
	charc[0] = 0;
	if (depth) {
		strcpy(charc, "{");
	}
	strcat(charc, name);
	if (depth) {
		strcat(charc, "}");
	}
	insert(head, create_process(charc, pid), ppid);
	return 0;
}

void ch_me(void *t)
{
	if (t == NULL) {
		perror("malloc error");
		exit(0);
	}
}

process *create_process(const char *process_name, const char *ppid)
{
	process *p = (process *)malloc(sizeof(process));
	ch_me(p);
	p->p_name = (char *)malloc(sizeof(char) * (strlen(process_name) + 1));
	p->p_pid = (char *)malloc(sizeof(char) * (strlen(ppid) + 1));
	ch_me(p->p_name);
	ch_me(p->p_pid);
	strcpy(p->p_name, process_name);
	strcpy(p->p_pid, ppid);
	p->bro = NULL;
	p->son = NULL;
	return p;
}

int compare(const char *x, const char *y)
{
	int tmp1 = atoi(x), tmp2 = atoi(y);
	if (x == y) {
		return 0;
	} else if (x > y) {
		return 1;
	} else {
		return -1;
	}
}

int check_repeat(process *head1, process *head2)
{
	if (head1 == NULL || head2 == NULL)
		return -1;
	if (strcmp(head1->p_name, head2->p_name) != 0)
		return -1;
	int count = 1;
	while (head1->son && head2->son) {
		if (strcmp(head1->son->p_name, head2->son->p_name) == 0 &&
		    head1->son->bro == NULL && head2->son->bro == NULL) {
			head1 = head1->son;
			head2 = head2->son;
			count += 1;
		} else {
			return -1;
			break;
		}
	}
	if ((head1->son != NULL && head2->son == NULL) ||
	    (head1->son == NULL && head2->son != NULL)) {
		return -1;
	}
	return count;
}

process *get_last_child(process *head)
{
	process *lastChild;
	while (head->son) {
		head = head->son;
	}
	lastChild = head;
	return lastChild;
}

int has_bro(process *head)
{
	process *trav = head;
	while (trav->bro) {
		if (check_repeat(trav, trav->bro) != -1) {
			trav = trav->bro;
		} else {
			return 0;
			break;
		}
	}
	return 1;
}

// insert a node to the tree
void insert_node(process *head, process *node)
{
	process *cursor = head->bro;
	process *previous = head;
	while (cursor) {
		int stat = strcmp(head->p_name, node->p_name);
		if (stat == 1 ||
		    (stat == 0 && compare(head->p_pid, node->p_pid) == -1)) {
			previous->bro = node;
			node->bro = cursor;
			return;
		}
		previous = cursor;
		cursor = cursor->bro;
	}
	previous->bro = node;
	return;
}

int insert(process *head, process *node, const char *fp_pid)
{
	if (node == NULL || head == NULL) {
		return -1;
	}
	while (head) {
		if (strcmp(head->p_pid, fp_pid) == 0) {
			if (head->son) {
				insert_node(head->son, node);
			} else {
				head->son = node;
			}
			return 0;
		} else {
			if (insert(head->son, node, fp_pid) == 0) {
				return 0;
			}
		}
		head = head->bro;
	}
	return -1;
}

// print tree with process ID
int print_tr(process *head, int len, int *buffer, int flag)
{
	if (head == NULL)
		return -1;
	while (head) {
		printf("%s", head->p_name);
		if (flag) {
			printf("(%s)", head->p_pid);
		}
		if (head->son) {
			if (!head->son->bro) {
				printf("\u2500\u2500\u2500");
			} else {
				printf("\u2500\u252c\u2500");
			}
			int size = strlen(head->p_name) + len + 3 +
				   (flag * (strlen(head->p_pid) + 2));
			if (head->son->bro) {
				buffer[size - 2] = 1;
			}
			print_tr(head->son, size, buffer, flag);
		}
		head = head->bro;
		if (head) {
			printf("\n");
			if (!head->bro) {
				buffer[len - 2] = 0;
			}
			for (int j = 0; j < len - 2; j++) {
				if (buffer[j]) {
					printf("\u2502");
				} else {
					printf(" ");
				}
			}
			if (!head->bro) {
				printf("\u2514\u2500");
			} else {
				printf("\u251c\u2500");
			}
		}
	}
	return 0;
}

// print_tree for no PID.
int print_tr2(process *head, int len, int *buffer)
{
	if (head == NULL)
		return -1;

	while (head) {
		process *head2 = head;
		int count_repeat = 1;
		int chain_len = check_repeat(head, head->bro);
		while (check_repeat(head, head->bro) != -1) {
			count_repeat += 1;
			head = head->bro;
		}
		if (count_repeat > 1) {
			char node_name[10];
			sprintf(node_name, "%d", count_repeat);
			strcat(node_name, "*");
			if (chain_len == 1) {
				strcat(node_name, "[");
				strcat(node_name, head2->p_name);
				strcat(node_name, "]");
			} else {
				process *end;
				strcat(node_name, "[");
				strcat(node_name, head->p_name);
				end = get_last_child(head2);
				strcat(end->p_name, "]");
			}
			head2->p_name = node_name;
			head2->bro = head->bro;
			head = head2;
		}
		printf("%s", head->p_name);

		if (head->son) {
			if (!head->son->bro || has_bro(head->son)) {
				printf("\u2500\u2500\u2500");
			} else {
				printf("\u2500\u252c\u2500");
			}
			int size = strlen(head->p_name) + len + 3;
			if (head->son->bro && !has_bro(head->son)) {
				buffer[size - 2] = 1;
			}
			print_tr2(head->son, size, buffer);
		}

		head = head->bro;
		if (head) {
			printf("\n");
			if (!head->bro || has_bro(head)) {
				buffer[len - 2] = 0;
			}
			for (int j = 0; j < len - 2; j++) {
				if (buffer[j]) {
					printf("\u2502");
				} else {
					printf(" ");
				}
			}
			if (!head->bro || has_bro(head)) {
				printf("\u2514\u2500");
			} else {
				printf("\u251c\u2500");
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int buf[1024] = { 0 };
	int n = 0, p = 0;
	for (int idx = 1; idx < argc; idx++) {
		if (strcmp(argv[idx], "-p") == 0) {
			n = 1;
		} else if (strcmp(argv[idx], "") == 0) {
			p = 1;
		} else {
			puts(argv[idx]);
			puts("error command");
			exit(0);
		}
	}
	process **head = (process **)malloc(sizeof(process *));
	list_dir("/proc", " ", head, 0, p);
	if (n) {
		print_tr((*head), 0, buf, n);
	} else {
		print_tr2((*head), 0, buf);
	}
	printf("\n");
	return 0;
}