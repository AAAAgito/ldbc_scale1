// bonus problem for assignment 1, CSC3150, 22Fall
// Author: Lai
// Date: 2022/09/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>

/* Core data structure, "node" */
struct ps_node {
	char name[128]; // process name
	pid_t pid; // this pid
	pid_t ppid; // parent pid
	int type; // pid(1), tid(0)
	struct ps_node *list_next; // the linked list next node pointer
	struct ps_node *child_head; // the tree children's head node pointer
	struct ps_node *child_next; // the tree next children node pointer
	int child_size; // the number of children
	int needDisp; // bool to record this node need to print out or be compressed
};

/* Some static variables, for argument and other*/
static struct ps_node *list_head = NULL;
static int displayPid = 0;
static int compactDisp = 1;
static pid_t target = 1;

// helper function test if a string is a number
int isANumber(char *s)
{
	while ((*s) != '\0') {
		if (!isdigit(*s))
			return 0;
		++s;
	}
	return 1;
}

// helper function to trim left part of a string
char *ltrim(char *s)
{
	while (!isalnum(*s))
		s++;
	return s;
}

// helper function to trim right part of a string
char *rtrim(char *s)
{
	char *back = s + strlen(s);
	while (!isalnum(*--back))
		;
	*(back + 1) = '\0';
	return s;
}

// helper function to trim both sides of a string
char *trim(char *s)
{
	return rtrim(ltrim(s));
}

void readSavePidStatus(char *path)
{
	FILE *fp;
	char *line = NULL;
	char *key;
	char *value;

	size_t len = 0;
	ssize_t read;

	int pid;
	int ppid;
	char name[64];

	fp = fopen(path, "r");
	if (fp == NULL) {
		printf("file open failed!");
		exit(EXIT_FAILURE);
	}
	while ((read = getline(&line, &len, fp)) != -1) {
		// printf("Retrieved line of length %zu:\n", read);
		key = strtok(line, ":");
		value = strtok(NULL, ":");
		if (key != NULL && value != NULL) {
			key = trim(key);
			value = trim(value);
			if (strcmp(key, "Name") == 0) {
				// found line Name
				strcpy(name, value);
			} else if (strcmp(key, "PPid") == 0) {
				// found line PPid
				ppid = atoi(value);
			} else if (strcmp(key, "Pid") == 0) {
				// found line Pid
				pid = atoi(value);
			}
		}
	}
	// printf("\n%s, %d, %d", name, ppid, pid);
	// construct a node
	struct ps_node *node = (struct ps_node *)malloc(sizeof(struct ps_node));
	if (node == NULL) {
		printf("failed!!!\n");
		exit(EXIT_FAILURE);
	}
	// assign values to node
	strcpy(node->name, name);
	node->pid = pid;
	node->ppid = ppid;
	node->type = 1;
	node->child_next = NULL;
	node->child_head = NULL;
	node->list_next = list_head;
	node->needDisp = 1;
	node->child_size = 0;
	// insert the node to the list
	list_head = node;

	// end operations
	fclose(fp);
	if (line)
		free(line);
}

void readSaveTidStatus(char *path)
{
	FILE *fp;
	char *line = NULL;
	char *key;
	char *value;

	size_t len = 0;
	ssize_t read;

	int pid;
	int ppid;
	char name[64];

	fp = fopen(path, "r");
	if (fp == NULL) {
		printf("file open failed!");
		exit(EXIT_FAILURE);
	}
	while ((read = getline(&line, &len, fp)) != -1) {
		key = strtok(line, ":");
		value = strtok(NULL, ":");
		if (key != NULL && value != NULL) {
			key = trim(key);
			value = trim(value);
			if (strcmp(key, "Name") == 0) {
				// found line Name
				strcpy(name, value);
			} else if (strcmp(key, "Tgid") == 0) {
				// found line Tgid
				ppid = atoi(value);
			} else if (strcmp(key, "Pid") == 0) {
				// found line Pid
				pid = atoi(value);
			}
		}
	}
	// construct a node
	struct ps_node *node = (struct ps_node *)malloc(sizeof(struct ps_node));
	if (node == NULL) {
		printf("failed!!!\n");
		exit(EXIT_FAILURE);
	}
	// assign values to node
	strcpy(node->name, name);
	node->pid = pid;
	node->ppid = ppid;
	node->type = 0;
	node->child_next = NULL;
	node->child_head = NULL;
	node->list_next = list_head;
	node->needDisp = 1;
	node->child_size = 0;
	// insert the node to the list
	list_head = node;

	// end operations
	fclose(fp);
	if (line)
		free(line);
}

void listThreadDir(char *path, char *pname)
{
	DIR *pDir;
	struct dirent *ent;
	int i = 0;
	char taskStatusName[512];
	int fileType = 4; // we only want folder!

	pDir = opendir(path);
	memset(taskStatusName, 0, sizeof(taskStatusName));

	// read the current directory
	while ((ent = readdir(pDir)) != NULL) {
		if (ent->d_type & fileType) {
			// this is a folder
			if (strcmp(ent->d_name, ".") == 0 ||
			    strcmp(ent->d_name, "..") == 0 ||
			    strcmp(ent->d_name, pname) == 0) {
				// this `ent` belongs to "." or ".." or process
				continue;
			}
			if (isANumber(ent->d_name)) {
				// this is a process folder
				// 1. load the main process info
				sprintf(taskStatusName, "%s/%s/status", path,
					ent->d_name);
				readSaveTidStatus(taskStatusName);
			}
		}
	}
	closedir(pDir);
}

void listProcessDir(char *path)
{
	DIR *pDir;
	struct dirent *ent;
	int i = 0;
	char psStatusFileName[512];
	char threadDirname[512];
	int fileType = 4; // we only want folder!

	pDir = opendir(path);
	memset(psStatusFileName, 0, sizeof(psStatusFileName));

	// read the current directory
	while ((ent = readdir(pDir)) != NULL) {
		if (ent->d_type & fileType) {
			// this is a folder
			if (strcmp(ent->d_name, ".") == 0 ||
			    strcmp(ent->d_name, "..") == 0) {
				// this `ent` belongs to "." or ".."
				continue;
			}
			if (isANumber(ent->d_name)) {
				// this is a process folder
				// 1. load the main process info
				sprintf(psStatusFileName, "%s/%s/status", path,
					ent->d_name);
				readSavePidStatus(psStatusFileName);
				// 2. load the tid info
				sprintf(threadDirname, "%s/%s/task", path,
					ent->d_name);
				listThreadDir(threadDirname, ent->d_name);
			}
		}
	}
	closedir(pDir);
}

void traverseList(struct ps_node *start)
{
	struct ps_node *current = start;
	while (current != NULL) {
		printf("%s\n", current->name);
		current = current->list_next;
	}
}

int countValid(struct ps_node *start)
{
	int count = 0;
	struct ps_node *current = start;
	while (current != NULL) {
		if (current->needDisp == 1)
			++count;
		current = current->child_next;
	}
	return count;
}

struct ps_node *searchPPidList(struct ps_node *start, pid_t target)
{
	struct ps_node *current = start;
	while (current != NULL) {
		// printf("%s\n", current->name);
		if (current->pid == target) {
			return current;
		}
		current = current->list_next;
	}
	return NULL;
}

/* Build the tree. return the root node pointer. */
struct ps_node *build_tree()
{
	// make a root node (pid=0)
	struct ps_node *root = (struct ps_node *)malloc(sizeof(struct ps_node));
	if (root == NULL) {
		printf("failed!!!\n");
		exit(EXIT_FAILURE);
	}
	// assign values to root node
	strcpy(root->name, "root0");
	root->pid = 0;
	root->ppid = 0;
	root->type = 1;
	root->child_next = NULL;
	root->child_head = NULL;
	root->list_next = list_head;
	root->needDisp = 1;
	root->child_size = 0;
	list_head = root;

	struct ps_node *current = list_head->list_next;
	while (current != NULL) {
		struct ps_node *parentNode =
			searchPPidList(list_head, current->ppid);
		if (parentNode != NULL) {
			// add `current` as one of children in parent.
			current->child_next = parentNode->child_head;
			parentNode->child_head = current;
			parentNode->child_size += 1;
		}
		current = current->list_next;
	}

	return root;
}

// print non-leave node, return the indent used.
int formatPrintNonLeave(struct ps_node *node, int indent, char *prefix, int isFirst)
{
	if (isFirst)
		printf("%s", "-+");
	else
		printf("%s", prefix);

	if (displayPid)
		// printf("-%s (%d)\n", node->name, node->pid);
		printf("-%s (%d)", node->name, node->pid);
	else
		printf("-%s", node->name);
		// printf("-%s\n", node->name);
	// return 3;
	return strlen(node->name) + 2;
}

int formatPrintLeave(struct ps_node *node, int count, char *prefix, int isFirst)
{
	if (isFirst)
		printf("%s", "-+");
	else
		printf("%s", prefix);

	if (count > 1) {
		if (node->type == 1)
			printf("-%d*[%s]\n", count, node->name);
		else
			printf("-%d*[{%s}]\n", count, node->name);
	} else {
		if (node->type == 1)
			if (displayPid)
				printf("-%s (%d)\n", node->name, node->pid);
			else
				printf("-%s\n", node->name);
		else if (displayPid)
			printf("-{%s} (%d)\n", node->name, node->pid);
		else
			printf("-{%s}\n", node->name);
	}
	return 1;
}

// actually, we print the children of rnode.
void displayPSTree(struct ps_node *rnode, int level, char *prefix)
{
	if (level == 0) {
		// The root of the whole process
		int spaces = formatPrintNonLeave(rnode, 0, prefix, 0);
		strcat(prefix, "   |");
		// for (int j=0; j < spaces; ++j){
		// 	strcat(prefix, " ");
		// }
		// strcat(prefix, "|");
	}
	// go to current node's child node
	struct ps_node *current = rnode->child_head;
	int isFirstNode = 1; // first node is at first line.
	while (current != NULL) {
		if (current->needDisp == 0) {
			current = current->child_next;
			continue;
		}

		if (current->child_head != NULL) {
			// current node has children, non leave.
			if (countValid(current->child_next) == 0) {
				prefix[(1 + level) * 4 - 1] = '`';
			}

			formatPrintNonLeave(current, level, prefix, isFirstNode);
			isFirstNode = 0;
			int count = countValid(current->child_head);
			if (count > 1) {
				strcat(prefix, "   |");
			} else if (count == 1) {
				// prefix[strlen(prefix)-4] = '\0';
				strcat(prefix, "   `");
			} else
				strcat(prefix, "    ");

			if (countValid(current->child_next) == 0) {
				prefix[(1 + level) * 4 - 1] = ' ';
			}

			displayPSTree(current, level + 1, prefix);
			prefix[strlen(prefix) - 4] = '\0';
		} else {
			// current does NOT have children

			// compress
			if (compactDisp) {
				struct ps_node *current2 = current->child_next;
				int count = 1;
				while (current2 != NULL) {
					if (strcmp(current2->name,
						   current->name) == 0 &&
					    current2->child_head == NULL &&
					    current2->type == current->type) {
						count += 1;
						current2->needDisp = 0;
					}
					current2 = current2->child_next;
				}

				if (countValid(current->child_next) == 0) {
					prefix[(1 + level) * 4 - 1] = '`';
				}
				formatPrintLeave(current, count, prefix, isFirstNode);
				isFirstNode = 0;
			} else {
				if (countValid(current->child_next) == 0) {
					prefix[(1 + level) * 4 - 1] = '`';
				}
				formatPrintLeave(current, 1, prefix, isFirstNode);
				isFirstNode = 0;
			}
		}
		current = current->child_next;
	}
}

void printHelpDoc()
{
	printf("Usage: pstree [ -c ] [ -V ] [ -p ] [ --help ] [ PID ]\n");
	printf("Display a tree of processes.\n\n");

	printf("    -c, --compact     don't compact identical subtrees\n");
	printf("    -p, --show-pids   show PIDs; implies -c\n");
	printf("    -V, --version     display version information\n");
	printf("    --help            display helper doc\n");
	printf("    PID               start at this PID; default is 1 (init)\n");
}

void printVersion()
{
	printf("pstree (By Future) 1.0 Copyright (C) 2022 Lai.\n\n");

	printf("PSmisc comes with ABSOLUTELY NO WARRANTY.\n");
	printf("This is free software, and you are welcome to redistribute\n");
	printf("it under the terms of the GNU General Public License. \n");
}

int main(int argc, char *argv[])
{
	/* parse the argument */
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-h") == 0 ||
		    strcmp(argv[i], "--help") == 0) {
			printf("Future's pstree: welcome to the helper doc!\n");
			printHelpDoc();
			exit(0);
		} else if (strcmp(argv[i], "-V") == 0 ||
			   strcmp(argv[i], "--version") == 0) {
			printVersion();
			exit(0);
		} else if (strcmp(argv[i], "-c") == 0 ||
			   strcmp(argv[i], "--compact") == 0) {
			compactDisp = 0;
		} else if (strcmp(argv[i], "-p") == 0 ||
			   strcmp(argv[i], "--show-pids") == 0) {
			displayPid = 1;
			compactDisp = 0;
		} else if (isANumber(argv[1])) {
			target = atoi(argv[1]);
		} else {
			printf("pstree: unrecognized option '%s'\n", argv[i]);
			printHelpDoc();
			exit(1);
		}
	}

	/* save all process (threads) to the linked list */
	listProcessDir("/proc");

	/* build the process tree*/
	build_tree();

	struct ps_node *start_root = searchPPidList(list_head, target);
	if (start_root == NULL) {
		printf("Can not find the process with PID = %d!\n", target);
		exit(1);
	}
	/* prefix settings and display! */
	char prefix[128];
	prefix[0] = '\0';
	displayPSTree(start_root, 0, prefix);

	return 0;
}