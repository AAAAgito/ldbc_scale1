
#include "DuLinkList.h"
#include "math.h"

HelperNode HARR[100000] = {0}; // helperArr
int MAX_PID = 0;
DuLNode *pROOT = NULL; // The root node of the tree

int max(int a, int b) {
  if (a >= b) {
    return a;
  }
  return b;
}

int isSpace(char c) {
  if (c == 9     // TAB
      || c == 10 // LF(line feed)
      || c == 13 // CR(carriage return)
      || c == 32 // white space
  ) {
    return 1;
  }
  return 0;
}

//*************Declaration************//
//******The technique of parsing the status file comes from the source code of
// tinyxml**************
//**from tinyxmlparser.cpp line 28 and the following information
//**Reference******

// Consecutive spaces are skipped by the function(from tinyxml)
char *SkipWhiteSpace(char *p) {
  while (1) {
    if (!isSpace(*p))
      return p;
    p++;
  }

  return p;
}

// Read a non-whitespace string by the function*(from tinyxml)
char *ReadElem(char *p, char *elem) {
  char *start = p;

  while (1) {
    if (isSpace(*p))
      break;
    p++;
  }
  strncpy(elem, start, p - start);
  elem[p - start] = '\0';
  return p;
}

void getInfoIn_status(char *status_path, int *ppid, int *uid,
                      int *gid, //(from tinyxml)
                      char *name) {
  FILE *fp = fopen(status_path, "r");
  if (!fp) {
    printf("Cannot Open %s", status_path);
    exit(1);
  }
  while (!feof(fp)) {
    char line[256] = {0};
    for (int i = 1; i <= 10; i++) {
      // 1st line (name)
      // 7th line (ppid)
      // 9th line (uid)
      // 10th line (gid）
      fgets(line, sizeof(line), fp);
      // fgets read a line of a file to the first agrument which includes the \n
      // in the end(the normal end is \n\0) the second paremeter in fgets input
      // the length of the string buf printf("%s", line);
      if (i == 1) {
        // printf("%s", line);
        char *p = line;
        char elem[128] = {0};

        // Name:
        p = SkipWhiteSpace(p);
        p = ReadElem(p, elem);
        // printf("%s\n", elem);

        // name
        p = SkipWhiteSpace(p);
        p = ReadElem(p, elem);
        // printf("name=%s\n", elem);
        strcpy(name, elem);
      } else if (i == 7) {
        // printf("%s", line);
        char *p = line;
        char elem[128] = {0};

        // PPID:
        p = SkipWhiteSpace(p);
        p = ReadElem(p, elem);
        // printf("%s\n", elem);

        // ppid
        p = SkipWhiteSpace(p);
        p = ReadElem(p, elem);
        // printf("ppid=%s\n", elem);
        int int_ppid = atoi(elem);
        assert(int_ppid >= 0);
        *ppid = int_ppid;
      } else if (i == 9) {
        // printf("%s", line);

        char *p = line;
        char elem[128] = {0};

        // UID:
        p = SkipWhiteSpace(p);
        p = ReadElem(p, elem);
        // printf("%s\n", elem);

        // uid
        p = SkipWhiteSpace(p);
        p = ReadElem(p, elem);
        // printf("uid=%s\n", elem);
        int int_uid = atoi(elem);
        assert(int_uid >= 0);
        *uid = int_uid;

      } else if (i == 10) {
        // printf("%s", line);

        char *p = line;
        char elem[128] = {0};

        // GID:
        p = SkipWhiteSpace(p);
        p = ReadElem(p, elem);
        // printf("%s\n", elem);

        // gid
        p = SkipWhiteSpace(p);
        p = ReadElem(p, elem);
        int int_gid = atoi(elem);
        assert(int_gid >= 0);
        *gid = int_gid;
        // printf("uid=%d\n", int_gid);
      }
    }

    break;
  }
  fclose(fp);
}

char *uidToName(uid_t uid) {
  struct passwd *pw_ptr;
  static char numstr[10];

  if ((pw_ptr = getpwuid(uid)) == NULL) {
    sprintf(numstr, "%d", uid);
    return numstr;
  } else
    return pw_ptr->pw_name;
}

void ReadAndStoreAllProcessInfo() {
  DIR *dp = opendir("/proc");
  if (!dp) {
    printf("Cannot Open/proc/");
    // pause();
    exit(1);
  } else {
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
      int pid = 0;
      int ppid = 0;
      int uid = 0;
      int gid = 0;
      char name[NAME_LEN] = {0};
      char status_path[256] = {0};
      if ((pid = atoi(entry->d_name)) ==
          0) { // pass when we meet 0 or it is not a number
        continue;
      } else { // store in pidinfo (name and pid and ppid)
        // printf("d_name : %s\n", entry->d_name);
        sprintf(status_path, "/proc/%d/status", pid);
        getInfoIn_status(status_path, &ppid, &uid, &gid, name);
        HARR[pid].flag_activated = 1;
        HARR[pid].ppid = ppid;
        HARR[pid].uid = uid;
        HARR[pid].gid = gid;
        strcpy(HARR[pid].name, name);
        HARR[pid].heapPointer = NULL;
        MAX_PID = max(MAX_PID, pid);
      }
    }
  }
}

void MakeTree() {
  for (int pid = 0; pid <= MAX_PID; pid++) {
    if (HARR[pid].flag_activated) {
      int ppid = HARR[pid].ppid;

      DuLNode *parent_heapPointer;
      if (ppid == -1) {
        printf("MakeTree err: ppid == -1");
        exit(1);
      } else if (ppid == 0) {
        parent_heapPointer = pROOT;
      } else {
        parent_heapPointer = HARR[ppid].heapPointer;
      }

      if (parent_heapPointer->data.childsList.cursize == 0) {
        InitList(&(parent_heapPointer->data.childsList));
      }
      PsTreeNode e = {0};
      e.pid = pid;
      e.ppid = ppid;
      e.uid = HARR[pid].uid;
      e.gid = HARR[pid].gid;
      strcpy(e.name, HARR[pid].name);

      DuLNode *self_heapPointer =
          Push_Back(&(parent_heapPointer->data.childsList), e);
      HARR[pid].heapPointer = self_heapPointer;
    }
  }
}

void _PrintTree(DuLNode *proot, int level, char option) {
  //-n Sort all processes in numerical ascending order
  //-p Display pid after the name
  //-V Display the version number of pstree
  //-u Displays the user the process belongs to after the name
  //-g Display the process group id after the name

  // Print the version number
  if (option == 'V') {
    printf("pstree (PSmisc) 22.21\n");
    printf("Copyright (C) 1993-2009 Werner Almesberger and Craig Small\n");
    printf("\n");
    printf("PSmisc comes with ABSOLUTELY NO WARRANTY.\n");
    printf("This is free software, and you are welcome to redistribute it "
           "under\n");
    printf("the terms of the GNU General Public License.\n");
    printf("For more information about these matters, see the files named "
           "COPYING.\n");
    return;
  }

  // print
  for (int i = 0; i < level; i++) {
    printf("  ");
  }
  // printf("%s(%d,%d)\n",proot->data.name, proot->data.pid, proot->data.ppid);
  if (option == '\0' || option == 'n') {
    printf("%s\n", proot->data.name);
  } else if (option == 'p') {
    printf("%s(pid=%d)\n", proot->data.name, proot->data.pid);
  } else if (option == 'u') {
    printf("%s(user=%s)\n", proot->data.name, uidToName(proot->data.uid));
  } else if (option == 'g') {
    printf("%s(gid=%d)\n", proot->data.name, proot->data.gid);
  }

  // The situation of the parent node
  if (proot->data.childsList.cursize > 0) {
    // Qsort the list
    if (option == 'n') {
      Qsort(&(proot->data.childsList), proot->data.childsList.cursize);
    }

    // Traverse
    DuLNode *p = proot->data.childsList.head->next;
    while (p != proot->data.childsList.head) {
      level++;
      _PrintTree(p, level, option);
      level--;
      p = p->next;
    }
  }
}

void PrintTree(DuLNode *proot, char option) { _PrintTree(proot, 0, option); }

int main(int argc, char *argv[]) {
  char option;
  if (argc == 1) {
    option = '\0'; // simplest situation
  } else if (argc == 2) {
    if (strlen(argv[1]) < 2) {
      printf(
          "option ERR,the parameter is too short,please input -n/p/V/u/g"); // short
      exit(1);
    } else {
      option = argv[1][1];
    }
  }

  if (option != '\0' && option != 'n' && option != 'p' && option != 'V' &&
      option != 'u' && option != 'g') {
    printf(
        "option ERR,the parameter letter is wrong,please input-n/p/V/u/g"); // wrong number or not include
    exit(1);
  }

  pROOT = NewNode();
  pROOT->data.pid = 0;
  pROOT->data.ppid = -1;
  pROOT->data.uid = 0;
  pROOT->data.gid = 0;
  strcpy(pROOT->data.name, "ROOT_pid0");

  HARR[0].flag_activated = 0;

  ReadAndStoreAllProcessInfo();

  MakeTree();

  PrintTree(pROOT->data.childsList.head->next, option);

  return 0;
}