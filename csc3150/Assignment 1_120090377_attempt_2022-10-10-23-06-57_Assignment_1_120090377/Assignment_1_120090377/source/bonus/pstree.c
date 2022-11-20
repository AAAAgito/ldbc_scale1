#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/*
 *--------------------------------------
 *   Proc
 *   -   Struct to store the process's info
 *--------------------------------------
 */
struct proc {
  char comm[128];  //  TASK_COMM_LEN is of 16 characters
  pid_t pid;
  pid_t ppid;       // pid of parent process
  pid_t threadNum;  //  the process group id
  struct proc *parent;
  struct proc *next;
  struct proc *children[256];  //  the child process
  int childrenCnt;
};

struct proc *procHeadNode;
struct proc *procHeadNodePointer;
struct proc *procRootNode;

/*
 *--------------------------------------
 *   Function declaration
 *--------------------------------------
 */
int containsOnlyDigits(const char *s);
void initProc(char *fileContent);
void initTree();
struct proc *findProcByPID(pid_t pid);
void printTree();

/*
 *--------------------------------------
 *   containsOnlyDigits
 *   -   TRUE if the string contains only digits
 *--------------------------------------
 */
int containsOnlyDigits(const char *s) {
  while (*s) {
    if (isdigit(*s++) == 0) return 0;
  }

  return 1;
}

/*
 *--------------------------------------
 *   initProc
 *   -   Initialize the process node and use a linked list to store them.
 *   -   linkedlist starting with the procHeadNode.
 *--------------------------------------
 */
void initProc(char *fileContent) {
  //  Tokenize the fileContent using space
  char *stringToken;
  stringToken = strtok(fileContent, " ");
  int tracker = 0;
  struct proc *procNode;
  procNode = (struct proc *)malloc(sizeof(struct proc));
  //  Get all the informtion to initialize one specific node.
  while (stringToken != NULL) {
    /*
     *   pid: 0
     *   name/comm: 1
     *   state :2
     *   ppid: 3
     *
     */
    if (tracker == 0) {
      procNode->pid = atoi(stringToken);
      // printf("The pid stored is : %d\n", procNode->pid);
    } else if (tracker == 1) {
      strcpy(procNode->comm, stringToken);
      // printf("The name stored is : %s\n", procNode->comm);

    } else if (tracker == 3) {
      procNode->ppid = atoi(stringToken);
      // printf("The ppid stored is : %d\n", procNode->ppid);
    } else if (tracker == 19) {
      procNode->threadNum = atoi(stringToken);
      // printf("The threadNum stored is : %d\n", procNode->threadNum);
      // break;
    }

    // printf("%s \n", stringToken);
    stringToken = strtok(NULL, " ");
    tracker++;
  }
  procNode->childrenCnt = 0;
  procNode->next = procHeadNode;
  // printf("TEST for the NEXT, the ppid = %d, the pid = %d, the childrenCNT =
  // %d \n", procHeadNode->ppid, procHeadNode->pid, procHeadNode->childrenCnt);
  procHeadNode = procNode;  //  This is modified here
}

/*
 *--------------------------------------
 *   initTree
 *   -   Initialize the tree
 *   -   set every node's parent/children/childrenCnt
 *--------------------------------------
 */
void initTree() {
  if (procHeadNode->next == NULL) {
    return;
  }

  //  HeadNode is an empty node.
  struct proc *procTempNode, *procTmpParNode;
  procTempNode = (struct proc *)malloc(sizeof(struct proc));
  procTempNode = procHeadNode;

  //  This failed.
  while (procTempNode != NULL) {
    /*
    if(procTempNode->pid == 0){   //  this is the root node of the tree.
        procRootNode = procTempNode;
        procTempNode = procTempNode->next;
        continue;
    }
    */
    procTmpParNode = findProcByPID(procTempNode->ppid);
    if (procTmpParNode == NULL) {
      // printf("Gee\n");
      procTempNode = procTempNode->next;
      continue;
    }
    procTempNode->parent = procTmpParNode;
    procTmpParNode->children[procTmpParNode->childrenCnt] = procTempNode;
    procTmpParNode->childrenCnt++;
    // printf("TEST for the tree, the ppid = %d, the pid = %d, the childrenCNT =
    // %d \n", procTmpParNode->ppid, procTmpParNode->pid,
    // procTmpParNode->childrenCnt);
    procTempNode = procTempNode->next;
  }
}

/*
 *--------------------------------------
 *   findProcByPID
 *   -   return the proc* by the PID
 *--------------------------------------
 */
struct proc *findProcByPID(pid_t pid) {
  struct proc *current = procHeadNode;
  while (current) {
    if (current->pid == pid) return current;
    current = current->next;
  }
  return NULL;
}

/*
 *--------------------------------------
 *   printTree
 *   -   print out the tree in the tree struct
 *--------------------------------------
 */
void dumpTree(const char *indent, bool last, struct proc *procNode) {
  printf("%s", indent);
  char *longerIndent;
  longerIndent = (char *)malloc(strlen(indent) + 2);
  char *displayName = procNode->comm;
  int displayThreadNum = procNode->threadNum;

  if (last) {  //  Has Children
    printf("\\-");
    strcpy(longerIndent, indent);
    strcat(longerIndent, " ");
  } else {  //  Don't have children
    printf("|--");
    strcpy(longerIndent, indent);
    strcat(longerIndent, "| ");
  }
  if (procNode->threadNum > 1) {
    printf("%s————%d*{%s}\n", displayName, displayThreadNum - 1, displayName);
  } else {
    printf("%s\n", procNode->comm);
  }
  for (int i = 0; i < procNode->childrenCnt; i++) {
    dumpTree(longerIndent, procNode->childrenCnt - 1, procNode->children[i]);
  }
}

/*
 *--------------------------------------
 *   main
 *   -   ./pstree
 *--------------------------------------
 */
int main(int argc, char *argv[]) {
  //  Get All input options
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);  // C standard
                      // printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);
  //  Head of the LinkedList of procNode
  procHeadNode = (struct proc *)malloc(sizeof(struct proc));
  procRootNode = (struct proc *)malloc(sizeof(struct proc));
  // procHeadNodePointer = (struct proc*) malloc(sizeof(struct proc));
  // procHeadNodePointer->next = procHeadNode;

  //  Get all the Directories'name.
  DIR *d;
  struct dirent *dir;
  d = opendir("/proc");
  if (d) {
    //  init all the process node
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_type != DT_REG && containsOnlyDigits(dir->d_name)) {
        //  Concat the file path to stat
        char *pathToStat;
        pathToStat = (char *)malloc(strlen("/proc/") + 20);
        strcpy(pathToStat, "/proc/");
        strcat(pathToStat, dir->d_name);
        strcat(pathToStat, "/stat");
        // printf("%s\n", pathToStat);
        FILE *fp = fopen(pathToStat, "r");
        //  Get information about thread
        if (fp) {
          // Using fgets to get all the content in the file.
          char fileContent[256];
          // printf("%s opened Suc\n", dir->d_name);
          fgets(fileContent, sizeof(fileContent), fp);
          // printf("%s\n", fileContent);
          initProc(fileContent);
          fclose(fp);
        } else {
          // 错误处理
          printf("[ERROR]: This Process is already gone!");
        }
      }
    }
    initTree();
    closedir(d);
    procRootNode = findProcByPID(1);
    // printf("TEST for the ROOT, the ppid = %d, the pid = %d, the childrenCNT =
    // %d \n, the name = %s \n", procRootNode->ppid, procRootNode->pid,
    // procRootNode->childrenCnt, procRootNode->comm);

    dumpTree(" ", true, procRootNode);
  }

  return 0;
}