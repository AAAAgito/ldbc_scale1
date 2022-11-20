#include <assert.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct processNode
{
  pid_t pid;
  pid_t ppid;
  char name[20];
  struct processNode *children[256];
  struct processNode *parent;
  int count; // total account of children
  int index; // store the index in order to print correctly
} processNode;

processNode pidInfos[10000]; // store all the processes' infomation

int pid_count = 0;

int getlength(int x) // get the length of string(pid)
{
  int m, n = 0;
  m = x;
  do
  {
    n++;
    x /= 10;
  } while (x > 0);
  return n;
}

pid_t PNP(pid_t cpid, char name[])
{ // read process's name and its ppid

  char *str = (char *)malloc(sizeof(char) * 20);
  if (str == NULL)
  {
    printf("malloc failed\n");
    exit(1);
  }
  sprintf(str, "%d", cpid);

  /* get the path of the process*/
  char path[20] = "/proc/";
  strcat(path, str);
  strcat(path, "/stat");

  FILE *fp = fopen(path, "r");
  if (fp)
  {
    char state;
    pid_t pid, ppid;
    char pname[40];
    fscanf(
        fp, "%d (%s %c %d", &pid, pname, &state,
        &ppid); // from left to right: pid, process's name, process state, ppid
    pname[strlen(pname) - 1] = '\0';
    // printf("ppid:%d\n", ppid);
    strcpy(name, pname);
    fclose(fp);
    free(str);
    return ppid;
  }
  else
  {
    printf("fail to open the file");
    free(str);
    exit(1);
  }
}

void buildPstree() // build the pstree with nopid
{
  for (int i = 0; i < pid_count; i++)
  {
    // printf("father: %s\n", pidInfos[i].name);
    int k = 0;
    for (int j = 0; j < pid_count; j++)
    {
      if (pidInfos[j].ppid == pidInfos[i].pid)
      {
        pidInfos[i].children[k] = &pidInfos[j];
        pidInfos[j].parent = &pidInfos[i];
        // pidInfos[j].index += strlen(pidInfos[i].name);
        k++;
        // printf("child[%d]: %s\n", k, pidInfos[j].name);
        // printf("child[%d]: %s\n", k, pidInfos[i].children[k]->name);
      }
    };
    pidInfos[i].count = k;
  }
  for (int i = 0; i < pid_count; i++)
  {
    if (pidInfos[i].parent == NULL)
    {
      pidInfos[i].index = 0;
    }
    else
    {
      processNode root = pidInfos[i];
      int k = 0;
      while ((root.parent) != NULL)
      {
        if (k == 0)
        {
          pidInfos[i].index = pidInfos[i].index + strlen(root.parent->name) + 1;
        }
        else
        {
          pidInfos[i].index = pidInfos[i].index + strlen(root.parent->name) + 3;
        }
        // printf("%d\n", strlen(root.parent->name));
        k++;
        root = *(root.parent);
      }
    }
  }
}

void buildPstree_p() // build the pstree with pid
{
  for (int i = 0; i < pid_count; i++)
  {
    // printf("father: %s\n", pidInfos[i].name);
    int k = 0;
    for (int j = 0; j < pid_count; j++)
    {
      if (pidInfos[j].ppid == pidInfos[i].pid)
      {
        pidInfos[i].children[k] = &pidInfos[j];
        pidInfos[j].parent = &pidInfos[i];
        // pidInfos[j].index += strlen(pidInfos[i].name);
        k++;
        // printf("child[%d]: %s\n", k, pidInfos[j].name);
        // printf("child[%d]: %s\n", k, pidInfos[i].children[k]->name);
      }
    };
    pidInfos[i].count = k;
  }
  for (int i = 0; i < pid_count; i++)
  {
    if (pidInfos[i].parent == NULL)
    {
      pidInfos[i].index = 0;
    }
    else
    {
      processNode root = pidInfos[i];
      int k = 0;
      while ((root.parent) != NULL)
      {
        if (k == 0)
        {
          pidInfos[i].index = pidInfos[i].index + strlen(root.parent->name) + 2;
        }
        else
        {
          pidInfos[i].index = pidInfos[i].index + strlen(root.parent->name) +
                              3 + getlength(root.parent->pid) + 2;
        }
        // printf("%d\n", strlen(root.parent->name));
        k++;
        root = *(root.parent);
      }
    }
  }
}

void printPstree(processNode root) // print the pstree with nopid
{
  if (root.count == 0)
  {
    printf("-%s\n", root.name);
    return;
  }
  else
  {
    if (root.pid == pidInfos[0].pid)
    {
      printf("%s-", root.name);
    }
    else
    {
      printf("-%s-", root.name);
    }
    for (int i = 0; i < root.count; i++)
    {
      if (i == 0)
      {
        printf("+");
      }
      else
      {
        for (int j = 0; j < root.children[i]->index; j++)
        {
          printf(" ");
        };
        printf("|");
      }
      printPstree(*(root.children[i]));
    }
  }
}

void printPstree_p(processNode root) // print the pstree with pid
{
  if (root.count == 0)
  {

    printf("-%s(%d)\n", root.name, root.pid);
    return;
  }
  else
  {
    if (root.pid == pidInfos[0].pid)
    {
      printf("%s(%d)-", root.name, root.pid);
    }
    else
    {
      printf("-%s(%d)-", root.name, root.pid);
    }
    for (int i = 0; i < root.count; i++)
    {
      if (i == 0)
      {
        printf("+");
      }
      else
      {
        for (int j = 0; j < root.children[i]->index + 2; j++)
        {
          printf(" ");
        };
        printf("|");
      }
      printPstree_p(*(root.children[i]));
    }
  }
}

void getprocessNode() // store processes' info into a processNode
{
  DIR *dp = opendir("/proc");

  if (!dp)
  {
    printf("fail to open the directory");
    exit(1);
  }
  else
  {
    int pid = 0;
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL)
    {
      pid = atoi(entry->d_name); // copy the pid
      if ((pid == 0))
      {
        continue;
      }
      else
      {
        pidInfos[pid_count].pid = pid;
        pidInfos[pid_count].ppid = PNP(pid, pidInfos[pid_count].name);
        pid_count++;
      }
    }
    return;
  }
}

int main(int argc, char *argv[])
{
  int ret;
  if ((ret = getopt(argc, argv, "pnVAl")) == -1)
  {
    getprocessNode();
    buildPstree();
    processNode root = pidInfos[0];
    printPstree(root);
    printf("\n");
  }
  else
  {
    switch (ret)
    {
    case 'p':
      getprocessNode();
      buildPstree_p();
      processNode root = pidInfos[0];
      printPstree_p(root);
      printf("\n");
      break;
    case 'n':
      getprocessNode();
      buildPstree();
      processNode root2 = pidInfos[0];
      printPstree(root2);
      printf("\n");
      break;
    case 'V':
      printf(
          "pstree (PSmisc) 22.21\nCopyright (C) 1993-2009 Werner Almesberger "
          "and Craig Small\n\nPSmisc comes with ABSOLUTELY NO WARRANTY.\nThis "
          "is free software, and you are welcome to redistribute it under\nthe "
          "terms of the GNU General Public License.\nFor more information "
          "about these matters, see the files named COPYING.\n");
      break;
    case 'A':
      getprocessNode();
      buildPstree();
      processNode root3 = pidInfos[0];
      printPstree(root3);
      printf("\n");
      break;
    case 'l':
      getprocessNode();
      buildPstree();
      processNode root4 = pidInfos[0];
      printPstree(root4);
      printf("\n");
      break;
    };
    return 0;
  }
}