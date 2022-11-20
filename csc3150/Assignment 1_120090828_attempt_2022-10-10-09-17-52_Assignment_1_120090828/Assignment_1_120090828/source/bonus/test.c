#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <ctype.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/types.h>

#define PROCFS_ROOT "/proc"

// struct dirent {//used in the readdir function
//                ino_t          d_ino;       /* Inode number */
//                off_t          d_off;       /* Not an offset; see below */
//                unsigned short d_reclen;    /* Length of this record */
//                unsigned char  d_type;      /* Type of file; not supported
//                                               by all filesystem types */
//                char           d_name[256]; /* Null-terminated filename */
//            };

char *trim(char *str) // used to delete the white space
{
	char *s0 = str;
	char *s1;
	if (s0) {
		s1 = s0 + strlen(str) - 1;
		while (*s0 && isspace(*s0))
			s0++;
		while (s1 > s0 && isspace(*s1))
			*s1-- = '\0';
	}
	return s0;
}

struct pstree_node {
	char name[128];
	pid_t pid;
	pid_t ppid;
	struct pstree_node *parent;
	struct pstree_node *children[128]; // children node are store in an array
	struct pstree_node *next; // help to create a linked list
	int childNo; // mark it is the NO.X children of parent node
};
// int print_tree(struct pstree_node *root, int len);
// int print_tree(struct pstree_node* , int , int* , int ) ;
int print_tree(struct pstree_node *root, int level);

static struct pstree_node *link_list_head;

int linkedlist_insert(char *procname, pid_t pid, pid_t ppid)
{
	struct pstree_node *node;
	node = (struct pstree_node *)malloc(
		sizeof(struct pstree_node)); // the size may be large
	if (node == NULL) {
		printf("Unable to allocate memory for node\n");
		return 1;
	}
	strcpy(node->name, procname);
	node->pid = pid;
	node->ppid = ppid;
	node->children[0] = NULL;
	node->parent = NULL;
	node->next = link_list_head; // insert from the head
	node->childNo = 0;
	link_list_head = node; // mark the new start
	return 0;
}

int read_files_inside(char *dirname)
{
	char filename[256];
	char line[256];
	char procname[256];
	char pid[32];
	char ppid[32];
	char *key;
	char *value;
	FILE *p_file;
	int ll; // help with create the linked list
	strcpy(filename, dirname);
	strcat(filename,
	       "/status"); // the things we want to read is /PID/status
	p_file = fopen(filename, "r"); // read only
	if (p_file == NULL) {
		return 1;
	}
	while (fgets(line, sizeof(line), p_file) != NULL) {
		key = strtok(line, ":"); // get the things in front of :
		value = strtok(NULL, ":"); // get the things after :
		if (key != NULL && value != NULL) {
			trim(key);
			trim(value);
			if (strcmp(key, "Pid") == 0) {
				strcpy(pid,
				       value); // check whether this line is the PID line
			} else if (strcmp(key, "PPid") == 0) {
				strcpy(ppid,
				       value); // check whether this line is the PPID line
			} else if (strcmp(key, "Name") == 0) {
				strcpy(procname,
				       value); // check whether this line is the name line
			}
		}
	}
	ll = linkedlist_insert(
		&procname[0], atoi(pid),
		atoi(ppid)); // insert the information to the linked list
	return ll;
}

struct pstree_node *find_node(pid_t pid) // used to find the parent process in
	// the next fucntion and teh main
	// function
{
	struct pstree_node *node;
	for (node = link_list_head; node != NULL;
	     node = node->next) { // find in the link list
		if (node->pid == pid) {
			return node;
		}
	}
	return NULL;
}

int make_tree(void)
{
	int i;
	struct pstree_node *node;
	struct pstree_node *parent_node;

	for (node = link_list_head; node != NULL;
	     node = node->next) { // search the whole link list
		i = 0;
		parent_node = find_node(
			node->ppid); // access the parent node directly
		if (parent_node != NULL) {
			node->parent = parent_node; // link as parent node
			while (parent_node->children[i++] != NULL) {
			}; // jump to the last children node + 1
			parent_node->children[i - 1] =
				node; // insert as children node to the end
			// parent_node->children[i] = NULL;
			node->childNo = i; // mark it is the NO.X children node
		}
	}
	return 0;
}

// // i don't know how to print a tree
// int print_tree(struct pstree_node *root, int len) {
//   int i = 1; // used to mark which line it is, but fail
//   int k = 0; // used to mark how many white space should add
//   char *name;
//   printf("%s", root->name);
//   if (root->children[0] == NULL) { // the exit of the fucntion
//     printf("\n");
//     return 0;
//   } else {
//     while (root->children[i] != NULL) {
//       //   printf("\n |--");
//       name = root->name;
//       k = strlen(name);

//       print_tree(root->children[i], len + k); // print the children node
//       i += 1;
//       printf("\n");
//       for (int l = 0; l < len; ++l) {
//         if (i > 0)
//           printf(" "); // try to make sure the children process are in the
//           same
//                        // line, but fail anyway
//         else
//           ;
//       }
//       printf("| "); // try to link it in the terminal anyway
//     }
//   }
//   return 0;
// }

int print_tree(struct pstree_node *root, int level)
{
	int i;
	struct pstree_node *node;
	printf("name:%s pid: %d, ppid: %d\n", root->name, root->pid,
	       root->ppid);
	int j = 0;
	node = root->children[j];
	while (node != NULL) {
		print_tree(node, level + 1);
		j += 1;
		node = root->children[j];
	}
	return 0;
}

int main(int argc, char **argv)
{
	DIR *dirp;
	struct dirent *directory_entry; // can be seen in the front
	char dirname[256];
	int state[1024];
	int flag = 1;
	struct pstree_node *node;

	dirp = opendir(PROCFS_ROOT); // open the directly

	if (dirp == NULL) { // fail
		perror("Unabled to open /proc");
		return 1;
	}

	directory_entry = readdir(dirp); // read
	while (flag) {
		if (directory_entry != NULL) {
			if (directory_entry->d_type ==
			    DT_DIR) { // returns a pointer to a dirent structure
				strcpy(&dirname[0], PROCFS_ROOT);
				strcat(&dirname[0], "/");
				strcat(&dirname[0], directory_entry->d_name);
				read_files_inside(
					dirname); // read the things inside the directry
			}
		}
		flag = (directory_entry != NULL);
		directory_entry = readdir(
			dirp); // read all of the files or directry inside the folder /proc
	};

	make_tree(); // make linked list into tree

	node = find_node(1); // find the parent note of a tree
	// print_tree(node, 0); // print the tree
	// print without a line
	for (node = link_list_head; node != NULL; node = node->next) {
		if (node->parent == NULL) {
			print_tree(node, 0);
		}
	}

	return 0;
}