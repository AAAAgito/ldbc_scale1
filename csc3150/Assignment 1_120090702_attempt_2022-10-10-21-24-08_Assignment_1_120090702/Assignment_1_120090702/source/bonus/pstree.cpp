#include <cstring>
#include <dirent.h>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;

typedef struct threadNode {
  string name;
  int pid;
} TN;

typedef struct node {
  string name;
  int pid;
  int ppid;
  vector<TN *> child_threads;
  struct node *child;
  struct node *next;
  struct node *parent;
} NODE;

map<int, NODE *> pid_map;
stack<NODE *> sk;
priority_queue<int> values;

void printTree1(NODE *cur, int space) { // short output
  while (cur->next != NULL) {
    cout << "──" << cur->name << "(" << cur->pid << ")";
    if (cur->child != NULL) {
      printTree1(cur->child, space + 12);
    }
    cout << '\n';
    for (int i = 0; i < space; i++) {
      cout << " ";
    }
    cout << "|";
    cur = cur->next;
  }
  cout << "──" << cur->name << "(" << cur->pid << ")";
  if (cur->child != NULL) {
    printTree1(cur->child, space + 12);
  }
}

void printTree2(NODE *cur, int space) { // all output
  while (cur->next != NULL) {
    cout << "──" << cur->name;
    if (!cur->child_threads.empty()) {
      cout << "────"
           << "{" << cur->child_threads[0]->name << "}" << endl;
      for (int j = 1; j < cur->child_threads.size(); j++) {
        for (int i = 0; i < space + 24; i++) {
          cout << " ";
        }
        cout << "|" << endl;
        for (int i = 0; i < space + 24; i++) {
          cout << " ";
        }
        cout << "──"
             << "{" << cur->child_threads[j]->name << "}" << endl;
      }
    }
    if (cur->child != NULL) {
      printTree2(cur->child, space + 12);
    }
    cout << '\n';
    for (int i = 0; i < space; i++) {
      cout << " ";
    }
    cout << "|";
    cur = cur->next;
  }
  cout << "──" << cur->name;
  if (!cur->child_threads.empty()) {
    cout << "────"
         << "{" << cur->child_threads[0]->name << "}" << endl;
    for (int j = 1; j < cur->child_threads.size(); j++) {
      for (int i = 0; i < space + 24; i++) {
        cout << " ";
      }
      cout << "|" << endl;
      for (int i = 0; i < space + 24; i++) {
        cout << " ";
      }
      cout << "──"
           << "{" << cur->child_threads[j]->name << "}" << endl;
    }
  }
  if (cur->child != NULL) {
    printTree2(cur->child, space + 12);
  }
}

void printTree3(NODE *cur, int space) { // with pid
  while (cur->next != NULL) {
    cout << "──" << cur->name << "(" << cur->pid << ")";
    if (!cur->child_threads.empty()) {
      cout << "────"
           << "{" << cur->child_threads[0]->name << "}"
           << "(" << cur->child_threads[0]->pid << ")" << endl;
      for (int j = 1; j < cur->child_threads.size(); j++) {
        for (int i = 0; i < space + 24; i++) {
          cout << " ";
        }
        cout << "|" << endl;
        for (int i = 0; i < space + 24; i++) {
          cout << " ";
        }
        cout << "──"
             << "{" << cur->child_threads[j]->name << "}"
             << "(" << cur->child_threads[j]->pid << ")" << endl;
      }
    }
    if (cur->child != NULL) {
      printTree3(cur->child, space + 12);
    }
    cout << '\n';
    for (int i = 0; i < space; i++) {
      cout << " ";
    }
    cout << "|";
    cur = cur->next;
  }
  cout << "──" << cur->name << "(" << cur->pid << ")";
  if (!cur->child_threads.empty()) {
    cout << "────"
         << "{" << cur->child_threads[0]->name << "}"
         << "(" << cur->child_threads[0]->pid << ")" << endl;
    for (int j = 1; j < cur->child_threads.size(); j++) {
      for (int i = 0; i < space + 24; i++) {
        cout << " ";
      }
      cout << "|" << endl;
      for (int i = 0; i < space + 24; i++) {
        cout << " ";
      }
      cout << "──"
           << "{" << cur->child_threads[j]->name << "}"
           << "(" << cur->child_threads[j]->pid << ")" << endl;
    }
  }
  if (cur->child != NULL) {
    printTree3(cur->child, space + 12);
  }
}

int numberOnly(
    const struct dirent *dir) { // only the number name denote the process
  int n = strlen(dir->d_name);
  for (int i = 0; i < n; i++) {
    if (!isdigit(dir->d_name[i]))
      return 0;
    else
      return 1;
  }
}

string findName(char *proc_info) {
  int i;
  if (strncmp(proc_info, "Name:", 5) == 0) {
    for (i = 5; i < strlen(proc_info); i++) {
      if (proc_info[i] != '\t')
        break;
    }
    string name = "";
    int k = 0;
    for (int j = i; j < strlen(proc_info); j++) {
      name += proc_info[j];
    }
    if (name[name.length() - 1] == '\n')
      name = name.substr(0, name.length() - 1);
    return name;
  } else
    return "";
}

int findPid(char *proc_info) {
  int i;
  if (strncmp(proc_info, "Pid:", 4) == 0) {
    for (i = 4; i < strlen(proc_info); i++) {
      if (isdigit(proc_info[i]))
        break;
    }
    string pids = "";
    int k = 0;
    for (int j = i; j < strlen(proc_info); j++) {
      pids += proc_info[j];
    }
    int pidi = atoi(pids.c_str());
    return pidi;
  } else
    return -1;
}

int findPPid(char *proc_info) {
  int i;
  if (strncmp(proc_info, "PPid:", 5) == 0) {
    for (i = 5; i < strlen(proc_info); i++) {
      if (isdigit(proc_info[i]))
        break;
    }
    string ppids;
    int k = 0;
    for (int j = i; j < strlen(proc_info); j++) {
      ppids += proc_info[j];
    }
    int ppidi = atoi(ppids.c_str());
    return ppidi;
  } else
    return -1;
}

int findThreads(char *proc_info) {
  int i;
  if (strncmp(proc_info, "Threads:", 8) == 0) {
    for (i = 4; i < strlen(proc_info); i++) {
      if (isdigit(proc_info[i]))
        break;
    }
    string Threadss = "";
    int k = 0;
    for (int j = i; j < strlen(proc_info); j++) {
      Threadss += proc_info[j];
    }
    int Threadsi = atoi(Threadss.c_str());
    return Threadsi;
  } else
    return -1;
}

void getThreads(NODE *new_node) {
  int pid = new_node->pid;
  char pidc[10];
  sprintf(pidc, "%d", pid);
  string pids = pidc;
  char thread_info[100];
  struct dirent **namelist;
  string thread_name;
  int thread_pid;
  FILE *fp2;

  char path2[] = "/proc/";
  strcat(path2, pids.c_str());
  strcat(path2, "/task");

  int m = scandir(path2, &namelist, numberOnly, alphasort);
  for (int i = 0; i < m; i++) {
    char path3[100];
    strcpy(path3, path2);
    if (namelist[i]->d_name == pids)
      continue; // ignore self

    strcat(path3, "/");
    strcat(path3, namelist[i]->d_name);
    strcat(path3, "/status");
    fp2 = fopen(path3, "r");
    while (!feof(fp2)) {
      fgets(thread_info, 100, fp2);
      if (findName(thread_info) != "") {
        thread_name = findName(thread_info);
      } else if (findPid(thread_info) != -1) {
        thread_pid = findPid(thread_info);
        TN *new_tn = new TN;
        new_tn->name = thread_name;
        new_tn->pid = thread_pid;

        new_node->child_threads.push_back(new_tn);
        break;
      }
    }
    fclose(fp2);
  }
}

void linkNext(NODE *child, NODE *parent) {
  if (parent->child == NULL) {
    parent->child = child;
  } else {
    NODE *cur = parent->child;
    while (cur->next != NULL)
      cur = cur->next;
    cur->next = child;
  }
}

int main(int argc, char *argv[]) {
  FILE *fp;
  struct dirent **namelist;
  char proc_info[100];
  int pid, ppid, threads;
  string name;
  int mode;

  string arg = "";
  for (int i = 0; i < argc - 1; i++) {
    arg += argv[i + 1];
  }
  // cout<<"arg:"<<arg<<endl;
  if (arg == "" || arg == "-l")
    mode = 1;
  else if (arg == "-c")
    mode = 2;
  else if (arg == "-p")
    mode = 3;

  int n = scandir("/proc", &namelist, numberOnly,
                  alphasort); // n is the number of all number dictory
  // printf("n is %d", n);
  if (n < 0) {
    printf("error in scandir\n");
  } else {
    for (int i = 0; i < n; i++) {
      char path[] = "/proc/";
      strcat(path, namelist[i]->d_name);
      strcat(path, "/status");
      fp = fopen(path, "r"); // enter every dictory to see the name and pid

      while (!feof(fp)) {
        fgets(proc_info, 100, fp);
        // printf(proc_info);
        // create and link NODE
        if (findName(proc_info) != "") {
          name = findName(proc_info);
        } else if (findPid(proc_info) != -1) {
          pid = findPid(proc_info);
        } else if (findPPid(proc_info) != -1) {
          ppid = findPPid(proc_info);
        } else if (findThreads(proc_info) != -1) { // threads appear latest
          threads = findThreads(proc_info);
          if (ppid == 2)
            continue; // the child process of kthreadd is not shown in pstree, I
                      // don't know why
          NODE *new_node = new NODE;
          new_node->name = name;
          new_node->pid = pid;
          new_node->ppid = ppid;
          new_node->child = NULL;
          new_node->next = NULL;
          if (threads > 1) {
            getThreads(new_node);
          }

          pid_map.insert(pair<int, NODE *>(pid, new_node));
          sk.push(new_node);
        }
      }
      fclose(fp);

      while (!sk.empty()) {
        NODE *child_node = sk.top();
        sk.pop();
        if (child_node->pid == 1) {
          child_node->parent = NULL;
          continue;
        }
        map<int, NODE *>::iterator iter;
        iter = pid_map.find(child_node->ppid);
        if (iter == pid_map.end()) {
          continue;
        }
        NODE *parent_node = iter->second;
        child_node->parent = parent_node;
        // if(child_node->ppid != 1){
        //     cout<<child_node->name<<"pid:"<<child_node->pid<<"ppid:"<<child_node->ppid<<parent_node->name<<endl;
        // }
        linkNext(child_node, parent_node);
      }
    }
  }
  NODE *head = pid_map[1];
  printf("systemd──");
  if (mode == 1 || mode == 2)
    printTree2(head->child, 8);
  else if (mode == 3)
    printTree3(head->child, 8);
}