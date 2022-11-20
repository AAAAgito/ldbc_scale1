#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define PATH "/proc"

struct processtid {
  int pid;
  int ppid;
  char name[64];
};

struct processinfo {
  int pid;
  int ppid;
  char name[64];
  int numberofthreads;
  bool same;
  struct processtid *tids[100];
};

struct pstreeNode {
  struct processinfo *nodeInfo;
  struct pstreeNode *parent;
  struct pstreeNode *children[128];
  struct pstreeNode *next;
  char childpretext[1000];
};

struct processinfo pInfos[1000];
int processNumber = 0;
static struct pstreeNode *pstreehead;
static struct pstreeNode *Nodeptr;

struct processtid *gettidInfos(char *filename) {
  char name[50];
  char *nameptr;
  nameptr = name;
  int pid;
  int ppid;
  char *probe;
  int lineNumber = 0;
  FILE *fp = fopen(filename, "r");

  struct processtid *tidInfo;
  tidInfo = (struct processtid *)malloc(sizeof(struct processtid));

  char buffer[50];
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    lineNumber++;
    int nameindex = 0;
    if (lineNumber == 1) {
      probe = strrchr(buffer, ':');
      probe++;
      sscanf(probe, "%s", nameptr);
      for (int i = 0; i < strlen(name); i++) {
        tidInfo->name[i] = name[i];
      }
      for (int i = 0; i < strlen(name); i++) {
        name[i] = '\0';
      }
    } else if (lineNumber == 6) {
      probe = strrchr(buffer, ':');
      while (!isdigit(*probe)) {
        probe++;
      }
      sscanf(probe, "%d", &pid);
      tidInfo->pid = pid;
    } else if (lineNumber == 7) {
      probe = strrchr(buffer, ':');
      while (!isdigit(*probe)) {
        probe++;
      }
      sscanf(probe, "%d", &ppid);
      tidInfo->ppid = ppid;
    }
    for (int i = 0; i < strlen(buffer); i++) {
      buffer[i] = '\0';
    }
  }
  return tidInfo;
}
void getpInfos(char *filename) {
  char name[50];
  char *nameptr;
  nameptr = name;
  int pid;
  int ppid;
  char *probe;
  int lineNumber = 0;
  FILE *fp = fopen(filename, "r");

  char buffer[50];
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    lineNumber++;
    int nameindex = 0;
    if (lineNumber == 1) {
      probe = strrchr(buffer, ':');
      probe++;
      sscanf(probe, "%s", nameptr);
      for (int i = 0; i < strlen(name); i++) {
        pInfos[processNumber].name[i] = name[i];
      }
      for (int i = 0; i < strlen(name); i++) {
        name[i] = '\0';
      }
    } else if (lineNumber == 6) {
      probe = strrchr(buffer, ':');
      while (!isdigit(*probe)) {
        probe++;
      }
      sscanf(probe, "%d", &pid);
      pInfos[processNumber].pid = pid;
    } else if (lineNumber == 7) {
      probe = strrchr(buffer, ':');
      while (!isdigit(*probe)) {
        probe++;
      }
      sscanf(probe, "%d", &ppid);
      if (ppid != -1)
        pInfos[processNumber].ppid = ppid;
      else
        pInfos[processNumber].ppid = 0;
    }
    for (int i = 0; i < strlen(buffer); i++) {
      buffer[i] = '\0';
    }
  }
  struct pstreeNode *Node;
  Node = (struct pstreeNode *)malloc(sizeof(struct pstreeNode));
  Node->nodeInfo = &pInfos[processNumber];
  Node->parent = NULL;
  Node->children[0] = NULL;
  Node->nodeInfo->same = false;
  if (pstreehead == NULL)
    pstreehead = Node;
  Nodeptr->next = Node;
  Nodeptr = Node;

  DIR *dir_ptr;
  struct dirent *direntp;
  char tidfile[50] = "/proc/";
  char tidPath[100];
  char task[] = "/task/";
  char status[] = "/status";
  char pidstr[50];
  char tid_pidstr[50];
  int tidnumber = 0;
  int tid_pid;
  sprintf(pidstr, "%d", pInfos[processNumber].pid);
  strcat(tidfile, pidstr);
  strcat(tidfile, task);
  dir_ptr = opendir(tidfile);
  if (dir_ptr == NULL) {
    fprintf(stderr, "can not open /proc/%s/task\n", pidstr);
    exit(0);
  }
  while (direntp = readdir(dir_ptr)) {
    tid_pid = atoi(direntp->d_name);
    if (tid_pid != 0 && tid_pid != pid) {
      sprintf(tid_pidstr, "%d", tid_pid);
      strcpy(tidPath, tidfile);
      strcat(tidPath, tid_pidstr);
      strcat(tidPath, status);
      Node->nodeInfo->tids[tidnumber] = gettidInfos(tidPath);
      tidnumber++;
      for (int i = strlen(tidfile); i < strlen(tidPath); i++) {
        tidPath[i] = '\0';
      }
    }
  }
  Node->nodeInfo->numberofthreads = tidnumber;
  if (Node->nodeInfo->numberofthreads != 0) {
    if (Node->nodeInfo->numberofthreads >= 2) {
      Node->nodeInfo->same = true;
      for (int i = 0; i < strlen(Node->nodeInfo->tids[0]->name); i++) {
        if (Node->nodeInfo->tids[0]->name[i] !=
            Node->nodeInfo->tids[1]->name[i]) {
          Node->nodeInfo->same = false;
          break;
        }
      }
    }
  }
}

void getprocessInfo() {
  int pid;
  DIR *dir_ptr;
  struct dirent *direntp;
  char path[50] = "/proc/";
  char status[] = "/status";
  char pidfile[20];

  dir_ptr = opendir(PATH);
  if (dir_ptr == NULL) {
    fprintf(stderr, "can not open /proc\n");
    exit(0);
  }
  while (direntp = readdir(dir_ptr)) {
    pid = atoi(direntp->d_name);
    if (pid != 0) {
      sprintf(pidfile, "%d", pid);
      strcat(path, pidfile);
      strcat(path, status);
      getpInfos(path);
      processNumber++;
      for (int i = 6; i < strlen(path); i++) {
        path[i] = '\0';
      }
    }
  }
}

struct pstreeNode *findNodeByPid(int pid) {
  struct pstreeNode *Node;
  Node = pstreehead;
  for (Node; Node != NULL; Node = Node->next) {
    if (Node->nodeInfo->pid == pid) {
      return Node;
    }
  }
  return NULL;
}

int numberofChild(struct pstreeNode *Node) {
  int count = 0;
  while (Node->children[count] != NULL) {
    count++;
  }
  return count;
}

void create_pstree(void) {
  int childnumber;
  struct pstreeNode *Node, *parentNode;
  parentNode = (struct pstreeNode *)malloc(sizeof(struct pstreeNode));
  Node = pstreehead;
  for (Node; Node != NULL; Node = Node->next) {
    parentNode = findNodeByPid(Node->nodeInfo->ppid);
    if (parentNode != NULL) {
      childnumber = 0;
      Node->parent = parentNode;
      while (parentNode->children[childnumber] != NULL)
        childnumber++;
      parentNode->children[childnumber] = Node;
      parentNode->children[childnumber + 1] = NULL;
    }
  }
}
bool checkarray(int array[], int length, int check) {
  for (int i = 0; i < length; i++) {
    if (check == array[i])
      return true;
  }
  return false;
}

int space;
bool pidshow = true;
bool collapse = true;
void print_pstree(struct pstreeNode *root, int type, char text[],
                  int indexofthreads) {
  char info[1000];
  struct pstreeNode *Node = root;
  if (Node->nodeInfo->pid == 1) {
    if (pidshow)
      sprintf(info, "%s(%d)", Node->nodeInfo->name, Node->nodeInfo->pid);
    else if (!collapse)
      sprintf(info, "%s", Node->nodeInfo->name);
    else
      sprintf(info, "%s", Node->nodeInfo->name);
    space = strlen(info) + 1;
  } else {
    if (type == 0) { // head of subtree
      if (pidshow)
        sprintf(info, "-%s(%d)", Node->nodeInfo->name, Node->nodeInfo->pid);
      else if (!collapse)
        sprintf(info, "-%s", Node->nodeInfo->name);
      else if (indexofthreads != -1) {
        sprintf(info, "-%d*[%s]", indexofthreads, Node->nodeInfo->name);
      } else
        sprintf(info, "-%s", Node->nodeInfo->name);
      space = strlen(text) + strlen(info) + 1;
    } else if (type == 1) {
      if (pidshow)
        sprintf(info, "%s-%s(%d)", text, Node->nodeInfo->name,
                Node->nodeInfo->pid);
      else if (!collapse)
        sprintf(info, "%s-%s", text, Node->nodeInfo->name);
      else if (indexofthreads != -1) {
        sprintf(info, "%s-%d*[%s]", text, indexofthreads, Node->nodeInfo->name);
      } else
        sprintf(info, "%s-%s", text, Node->nodeInfo->name);
      space = strlen(info) + 1;
      if (Node->children[0] != NULL && text[strlen(text) - 1] == '`')
        text[strlen(text) - 1] = ' ';
    } else if (type == 2) {
      if (Node->children[0] == NULL) {
        if (pidshow)
          sprintf(info, "-{%s}(%d)", Node->nodeInfo->tids[indexofthreads]->name,
                  Node->nodeInfo->tids[indexofthreads]->pid);
        else
          sprintf(info, "-{%s}", Node->nodeInfo->tids[indexofthreads]->name);
        space = strlen(text) + strlen(info) + 1;
      } else {
        if (pidshow)
          sprintf(info, "%s-{%s}(%d)", text,
                  Node->nodeInfo->tids[indexofthreads]->name,
                  Node->nodeInfo->tids[indexofthreads]->pid);
        else
          sprintf(info, "%s-{%s}", text,
                  Node->nodeInfo->tids[indexofthreads]->name);
        space = strlen(info) + 1;
      }
    } else if (type == 3) {
      if (pidshow)
        sprintf(info, "%s-{%s}(%d)", text,
                Node->nodeInfo->tids[indexofthreads]->name,
                Node->nodeInfo->tids[indexofthreads]->pid);
      else
        sprintf(info, "%s-{%s}", text,
                Node->nodeInfo->tids[indexofthreads]->name);
      space = strlen(info) + 1;
    } else if (type == 4) {
      if (Node->children[0] == NULL) {
        sprintf(info, "-%d*{%s}", indexofthreads,
                Node->nodeInfo->tids[0]->name);
        space = strlen(text) + strlen(info) + 1;
      } else {
        sprintf(info, "%s-%d*[{%s}]", text, indexofthreads,
                Node->nodeInfo->tids[0]->name);
        space = strlen(info) + 1;
      }
    } else if (type == 5) {
      if (Node->children[0] == NULL) {
        sprintf(info, "-{%s}", Node->nodeInfo->tids[indexofthreads]->name);
        space = strlen(text) + strlen(info) + 1;
      } else {
        sprintf(info, "%s-{%s}", text,
                Node->nodeInfo->tids[indexofthreads]->name);
        space = strlen(info) + 1;
      }
    } else if (type == 6) {
      sprintf(info, "%s-{%s}", text,
              Node->nodeInfo->tids[indexofthreads]->name);
      space = strlen(info) + 1;
    }
  }

  printf("%s", info);
  char newtext[1000];
  strcpy(newtext, text);

  if ((Node->children[1] != NULL && indexofthreads == -1) ||
      (Node->nodeInfo->numberofthreads >= 2 && indexofthreads == -1)) {
    for (int i = strlen(text); i < space; i++) {
      newtext[i] = ' ';
    }
    newtext[space] = '|';
  } else if (Node->children[0] != NULL && indexofthreads == -1) {
    for (int i = strlen(text); i < space; i++) {
      newtext[i] = ' ';
    }
  }

  if (Node->children[0] == NULL && indexofthreads == -1 &&
          Node->nodeInfo->numberofthreads == 0 ||
      (type == 1 && indexofthreads != -1) ||
      (type == 0 && indexofthreads != -1)) {
    printf("\n");
  } else if (type == 4 || type == 5 || type == 6) {
    printf("\n");
  } else if (Node->children[1] == NULL && indexofthreads == -1) {
    if (Node->nodeInfo->numberofthreads == 0 ||
        (Node->children[0] == NULL &&
         (Node->nodeInfo->same == true ||
          Node->nodeInfo->numberofthreads == 1) &&
         (pidshow == false && collapse == true))) {
      printf("--");
      newtext[space] = ' ';
    } else {
      printf("-+");
      newtext[space] = '|';
    }
  } else if (type == 2 || type == 3) {
    printf("\n");
  } else {
    printf("-+");
  }

  newtext[++space] = '\0';
  strcpy(Node->childpretext, newtext);

  int childindex = 0;
  int combinedindex[128];
  int combinenumber = 0;
  while (Node->children[childindex] != NULL && indexofthreads == -1) {
    if ((Node->children[childindex + 1] != NULL) &&
        Node->children[childindex]->children[0] == NULL &&
        Node->children[childindex + 1]->children[0] == NULL &&
        !strcmp(Node->children[childindex]->nodeInfo->name,
                Node->children[childindex + 1]->nodeInfo->name) &&
        Node->children[childindex]->nodeInfo->numberofthreads == 0 &&
        collapse && !pidshow) {
      int count = 1;
      int checkindex = childindex + 1;
      char *checkname = Node->children[childindex]->nodeInfo->name;
      while (Node->children[checkindex] != NULL) {
        if (!strcmp(checkname, Node->children[checkindex]->nodeInfo->name)) {
          combinedindex[count - 1] = checkindex;
          combinenumber++;
          count++;
        }
        checkindex++;
      }
      if (childindex == 0) {
        print_pstree(Node->children[childindex], 0, newtext, count);
      } else
        print_pstree(Node->children[childindex], 1, newtext, count);
      childindex = childindex++;
    } else if (childindex == 0) {
      print_pstree(Node->children[childindex], 0, Node->childpretext, -1);
    } else if (Node->children[childindex + 1] == NULL &&
               Node->nodeInfo->numberofthreads == 0 &&
               !checkarray(combinedindex, combinenumber, childindex)) {
      newtext[strlen(newtext) - 1] = '`';
      print_pstree(Node->children[childindex], 1, newtext, -1);
    } else if (!checkarray(combinedindex, combinenumber, childindex))
      print_pstree(Node->children[childindex], 1, Node->childpretext, -1);
    childindex++;
  }
  if (Node->nodeInfo->numberofthreads != 0 && indexofthreads == -1) {
    if (pidshow || !collapse) {
      int tidindex = 0;
      while (tidindex < Node->nodeInfo->numberofthreads) {
        if (tidindex == 0)
          print_pstree(Node, 2, Node->childpretext, tidindex);
        else if (tidindex == Node->nodeInfo->numberofthreads - 1) {
          newtext[strlen(newtext) - 1] = '`';
          print_pstree(Node, 3, newtext, tidindex);
        } else
          print_pstree(Node, 3, Node->childpretext, tidindex);
        tidindex++;
      }
    } else {
      if (Node->nodeInfo->same == true) {
        if (Node->children[0] != NULL) {
          newtext[strlen(newtext) - 1] = '`';
          print_pstree(Node, 4, newtext, Node->nodeInfo->numberofthreads);
        } else
          print_pstree(Node, 4, Node->childpretext,
                       Node->nodeInfo->numberofthreads);
      } else {
        int tidindex = 0;
        while (tidindex < Node->nodeInfo->numberofthreads) {
          if (tidindex == 0)
            print_pstree(Node, 5, Node->childpretext, tidindex);
          else if (tidindex == Node->nodeInfo->numberofthreads - 1) {
            newtext[strlen(newtext) - 1] = '`';
            print_pstree(Node, 6, newtext, tidindex);
          } else
            print_pstree(Node, 6, Node->childpretext, tidindex);
          tidindex++;
        }
      }
    }
  }
}

char *getop(int argc, char *argv[]) {
  if (argc == 1) {
    return NULL;
  }
  char *op = argv[1];
  char option[5];
  option[0] = op[1];
  strcpy(op, option);
  return op;
}

int main(int argc, char *argv[]) {
  char *option = getop(argc, argv);
  if (!option) {
    pidshow = false;
    collapse = true;
  } else {
    if (!strcmp(option, "p"))
      pidshow = true;
    else if (!strcmp(option, "c")) {
      pidshow = false;
      collapse = false;
    } else if (!strcmp(option, "V")) {
      printf("pstree 2022 (120090399 Yihao Peng)\n");
      return 0;
    }
  }
  pstreehead = (struct pstreeNode *)malloc(sizeof(struct pstreeNode));
  pstreehead = NULL;
  Nodeptr = (struct pstreeNode *)malloc(sizeof(struct pstreeNode));
  getprocessInfo();
  create_pstree();
  struct pstreeNode *Node;
  Node = pstreehead;
  print_pstree(Node, -1, "", -1);

  return 0;
};