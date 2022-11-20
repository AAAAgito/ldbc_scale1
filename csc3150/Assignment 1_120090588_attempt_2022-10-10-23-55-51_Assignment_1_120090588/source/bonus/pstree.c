#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

struct pstree {
	pid_t pid;
	char name[50];
	struct children *child;
};

struct children {
	struct children *nextChild;
	struct pstree *pstree;
};

typedef struct process { //every process is a "Process" in this program
	char name[60];
	__pid_t pid;
	__pid_t ppid;
} Process;

Process processList[9000]; //record information of corresponding processes
int processCount = 0;

pid_t getProcessInfomation(__pid_t pid, char processName[])
{ //找到对应pid进程的name，返回ppid
	char *buffer = (char *)malloc(sizeof(char) * 30);
	sprintf(buffer, "%d", pid);
	char informationPath[30] = "/proc/";
	strcat(informationPath, buffer);
	strcat(informationPath, "/stat");
	FILE *bufferFile = fopen(informationPath, "r");
	__pid_t PID, PPID;
	char bufferChar;
	char bufferStr[40];
	fscanf(bufferFile, "%d %s %c %d", &PID, bufferStr, &bufferChar, &PPID);
	bufferStr[strlen(bufferStr) - 1] = '\0';
	fclose(bufferFile);
	strcpy(processName, bufferStr);
	free(buffer);
	return PPID;
}

void setProcessList()
{
	int bufferInt = 0;
	struct dirent *direntExample;
	DIR *bufferDir = opendir("/proc");
	while ((direntExample = readdir(bufferDir)) != NULL) {
		bufferInt = atoi(direntExample->d_name);
		if (bufferInt == 0)
			continue;
		else {
			processList[processCount].pid = bufferInt;
			processList[processCount].ppid = getProcessInfomation(
				bufferInt, processList[processCount].name);
			processCount++;
		}
	}
}

void getChildren(int pid, int intArr[])
{
	int count = 0;
	size_t i = 0;
	for (; i < processCount; i++) {
		if (processList[i].ppid == pid) {
			intArr[count++] = i;
		}
	}
}

void makeTree(struct pstree *root, int length)
{
	int childrenBuffer[600] = { 0 }; //children list of the root
	char bufferString[100] = { 0 };
	int intBuffer = 2;
	getChildren(root->pid, childrenBuffer);
	root->child = (struct children *)malloc(sizeof(struct children));
	struct children *child = root->child;
	if (childrenBuffer[0] == 0) {
		printf("%s(%d)", root->name, root->pid);
		return;
	} else {
		sprintf(bufferString, "%s(%d)", root->name, root->pid);
		printf("%s", bufferString);
		if (childrenBuffer[1] != 0)
			printf("┬─");
		else
			intBuffer = 0;
		for (int index = 0; index < 600 && childrenBuffer[index] != 0;
		     index++) { //child->pstree即为root，这里直接把root变为新的root(其子)
			child->pstree =
				(struct pstree *)malloc(sizeof(struct pstree));
			child->pstree->pid =
				processList[childrenBuffer[index]]
					.pid; //让root的pid变为childrenBuffer[i]的pid
			strcpy(child->pstree->name,
			       processList[childrenBuffer[index]].name);
			makeTree(child->pstree,
				 strlen(bufferString) + length + intBuffer);
			if (index + 1 < 600 && childrenBuffer[index + 1] != 0) {
				child->nextChild = (struct children *)malloc(
					sizeof(struct children));
				child = child->nextChild;
				printf("\n");
				for (size_t i = 0;
				     i < strlen(bufferString) + length; i++) {
					printf(" ");
				}
				printf("├─");
			}
		}
	}
}

void makeTree2(struct pstree *root, int length)
{
	int childrenBuffer[600] = { 0 }; //children list of the root
	char bufferString[100] = { 0 };
	int intBuffer = 2;
	getChildren(root->pid, childrenBuffer);
	root->child = (struct children *)malloc(sizeof(struct children));
	struct children *child = root->child;
	if (childrenBuffer[0] == 0) {
		printf("%s", root->name);
		return;
	} else {
		sprintf(bufferString, "%s-", root->name);
		printf("%s", bufferString);
		if (childrenBuffer[1] != 0)
			printf("+-");
		else
			intBuffer = 0;
		for (int index = 0; index < 600 && childrenBuffer[index] != 0;
		     index++) { //child->pstree即为root，这里直接把root变为新的root(其子)
			child->pstree =
				(struct pstree *)malloc(sizeof(struct pstree));
			child->pstree->pid =
				processList[childrenBuffer[index]]
					.pid; //让root的pid变为childrenBuffer[i]的pid
			strcpy(child->pstree->name,
			       processList[childrenBuffer[index]].name);
			makeTree2(child->pstree,
				  strlen(bufferString) + length + intBuffer);
			if (index + 1 < 600 && childrenBuffer[index + 1] != 0) {
				child->nextChild = (struct children *)malloc(
					sizeof(struct children));
				child = child->nextChild;
				printf("\n");
				for (size_t i = 0;
				     i < strlen(bufferString) + length; i++) {
					printf(" ");
				}
				printf("|-");
			}
		}
	}
}

void makeTree3(struct pstree *root, int length)
{
	int childrenBuffer[600] = { 0 };
	char bufferString[100] = { 0 };
	int intBuffer = 2;
	getChildren(root->pid, childrenBuffer);
	root->child = (struct children *)malloc(sizeof(struct children));
	struct children *child = root->child;
	if (childrenBuffer[0] == 0) {
		printf("%s", root->name);
		return;
	} else {
		sprintf(bufferString, "%s", root->name);
		printf("%s", bufferString);
		if (childrenBuffer[1] != 0)
			printf("┬─");
		else
			intBuffer = 0;
		for (int index = 0; index < 600 && childrenBuffer[index] != 0;
		     index++) { //child->pstree即为root，这里直接把root变为新的root(其子)
			child->pstree =
				(struct pstree *)malloc(sizeof(struct pstree));
			child->pstree->pid =
				processList[childrenBuffer[index]]
					.pid; //让root的pid变为childrenBuffer[i]的pid
			strcpy(child->pstree->name,
			       processList[childrenBuffer[index]].name);
			makeTree3(child->pstree,
				  strlen(bufferString) + length + intBuffer);
			if (index + 1 < 600 && childrenBuffer[index + 1] != 0) {
				child->nextChild = (struct children *)malloc(
					sizeof(struct children));
				child = child->nextChild;
				printf("\n");
				for (size_t i = 0;
				     i < strlen(bufferString) + length; i++) {
					printf(" ");
				}
				printf("├─");
			}
		}
	}
}

int main(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		assert(argv[i]);
	}
	if (argc == 1) {
		setProcessList();
		struct pstree *tree = (struct pstree *)malloc(10000);
		tree->pid = processList[0].pid;
		strcpy(tree->name, processList[0].name);
		makeTree3(tree, 0);
		printf("\n");
	} else {
		char *option;
		option = argv[1];
		if (!strcmp(option, "-A")) { //"pstree -A"!!!!!!!!!
			setProcessList();
			struct pstree *tree = (struct pstree *)malloc(10000);
			tree->pid = processList[0].pid;
			strcpy(tree->name, processList[0].name);
			makeTree2(tree, 0);
			printf("\n");
		}
		if (!strcmp(option, "-n")) { //"pstree -n"!!!!!!!!!
			setProcessList();
			struct pstree *tree = (struct pstree *)malloc(10000);
			tree->pid = processList[0].pid;
			strcpy(tree->name, processList[0].name);
			makeTree3(tree, 0);
			printf("\n");
		}
		if (!strcmp(option, "-V")) { //"pstree -V"!!!!!!!!!
			printf("pstree (PSmisc) 22.21\nCopyright (C) 1993-2009 Werner Almesberger and Craig Small\nPSmisc comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to redistribute it under\nthe terms of the GNU General Public License.\nFor more information about these matters, see the files named COPYING.\n");
		}
		if (!strcmp(option, "-p")) { //"pstree -p"!!!!!!!!!
			setProcessList();
			struct pstree *tree = (struct pstree *)malloc(10000);
			tree->pid = processList[0].pid;
			strcpy(tree->name, processList[0].name);
			makeTree(tree, 0);
			printf("\n");
		}
		if (!strcmp(option, "-U")) { //"pstree -U"!!!!!!!!!
			setProcessList();
			struct pstree *tree = (struct pstree *)malloc(10000);
			tree->pid = processList[0].pid;
			strcpy(tree->name, processList[0].name);
			makeTree3(tree, 0);
			printf("\n");
		}
		if (!strcmp(option, "-c")) { //"pstree -c"!!!!!!!!!
			setProcessList();
			struct pstree *tree = (struct pstree *)malloc(10000);
			tree->pid = processList[0].pid;
			strcpy(tree->name, processList[0].name);
			makeTree3(tree, 0);
			printf("\n");
		}
	}
	return 0;
}
//READ ME, PLEASE:
//First input "make" to compile "pstree.c" into "pstree". Secondly enter:
//1.  ./pstree 		   (no option)
//2.  ./pstree -A      (for option: -A)
//3.  ./pstree -n      (for option: -n)
//4.  ./pstree -V      (for option: -V)
//5.  ./pstree -p      (for option: -p)
//6.  ./pstree -U      (for option: -U)
//7.  ./pstree -c      (for option: -c)

//My "./pstree" is equivalent to the commend:"pstree -n"(default)
//(I make the pstree according to the PID order. As a result, it comes into being like this.)
//I really wonder if i could get at least 6 points in this case. Thanks so much!
