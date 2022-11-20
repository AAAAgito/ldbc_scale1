#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define PATH "/proc"
#define BUFFSIZE_INFO 50
struct pro_inf { // 创建结构体存储pid和ppid
  int ppid;
  char *name;
  char *pid;
  struct pro_inf *sibl; //兄弟姐妹
  struct pro_inf *chld; //孩子
};
typedef struct pro_inf process;

void set_pid_list(char *path, process **head, char *father, int depth,
                  int flag);
void build_tree(char *pid, process *head, int dep);
int insert(process *head, process *node, char *ppid_str);
void insert_node(process *node, process *head);
process *create_pro(char *pro_name, char *pro_pid);
void print_tree(process *head, int len, int *buf, int flag);
int cmp(const char *a, const char *b);
/*-----------------------------------------------------------开始-----------------------------------------------------------------*/
int main(int argc, char *argv[]) { // argc:运行程序时给main发送参数的个数
  int file_num = argc - 1; // argv:存放返回字符串参数的指针数组
  printf("argc:%d\n", argc);
  for (int i = 0; i < argc; i++) {
    printf("argv:%s ", argv[i]);
  }
  int state = 0;
  int ch; // 判别输入功能类型
  while ((ch = getopt(argc, argv, "acp")) != -1) {
    switch (ch) {
    case 'a':
      printf("Have option: -a \n");
      break;
    case 'c': // 相同子进程不隐藏
      printf("Have option: -c \n");
      break;
    case 'p': // 显示PID
      printf("Have option: -p \n");
      state = 1;
      break;
    case '?':
      printf("Unknown option: %c \n", (char)optopt);
    }
  }

  int buf[1024] = {0};
  process **head = (process **)malloc(sizeof(process *));
  set_pid_list(PATH, head, " ", 0, state); // 获取当前所有进程的pid和ppid
  print_tree((*head), 0, buf, state);
  printf("\n");
  // process * head=create_pro("HL","1");
  // insert(head,create_pro("zyc","2"),"1");
  // insert(head,create_pro("zyc1","3"),"1");
  // insert(head,create_pro("zyc2","4"),"1");
  // print_tree(head,0,buf,1);
  // printf("\n");
  return 0;
}

void set_pid_list(char *path, process **head, char *father, int depth,
                  int flag) {
  DIR *dir_ptr;
  struct dirent *dirent_p;
  dir_ptr = opendir(path); // 打开/proc，返回DIR指针
  if (dir_ptr == NULL) {
    // fprintf(stderr, "can not open /proc\n");
    return;
  }
  if (depth == 0)
    *head = create_pro("", "0"); // 创建初始进程
  while ((dirent_p = readdir(dir_ptr)) != NULL) {
    if (DT_DIR == dirent_p->d_type) {
      int f = 0;
      for (int i = 0; i < strlen(dirent_p->d_name); i++) {
        if (dirent_p->d_name[i] < '0' || dirent_p->d_name[i] > '9')
          f = -1;
        break;
      }
      if (f != -1 && depth == 1 && !strcmp(dirent_p->d_name, father)) {
        continue;
      }
      build_tree(dirent_p->d_name, *head, depth);
      if (flag && depth == 0) { // 获取线程
        char buf[1024];
        sprintf(buf, "%s%s%s", "/proc/", dirent_p->d_name, "/task");
        set_pid_list(buf, head, dirent_p->d_name, 1, flag);
      }
    }
  }
  closedir(dir_ptr);
}

void build_tree(char *p, process *head, int dep) {
  FILE *fp1;
  char file_name[1024]; // SIZE
  char pid[1024];
  char ppid[1024];
  char name[1024]; // 线程路径
  char file_info[1024];
  sprintf(file_name, "%s/%s/status", PATH, p); // /proc/pid/status
  // printf("file_name: %s\n",file_name);
  fp1 = fopen(file_name, "r");
  if (fp1 == NULL) {
    return;
  }
  while ((fscanf(fp1, "%s", file_name)) !=
         EOF) { // 从 fp1中以字符串形式输入到file_name中

    if (strcmp(file_name, "Pid:") == 0) { // file_name和Pid比较前4位,找到PID
      fscanf(fp1, "%s", pid);
    }
    if (strcmp(file_name, "PPid:") == 0) { // file_name和PPid比较前5位,找到PPID
      fscanf(fp1, "%s", ppid);
    }
    if (strcmp(file_name, "Name:") == 0) { // file_name和Name比较前5位,找到名字
      fscanf(fp1, "%s", name);
    }
  }
  file_name[0] = 0;
  file_info[0] = 0;
  if (dep)
    strcpy(file_name, "{");
  strcat(file_name, name); // node 名
  if (dep)
    strcat(file_name, "}");

  // printf("pid:%s,name:%s, file_name:%s\n",pid,name, file_name);
  insert(head, create_pro(file_name, pid), ppid);
  fclose(fp1);
}

int insert(process *head, process *node, char *pid_str) {
  if (head == NULL)
    return -1;
  if (node == NULL)
    return -1;
  while (head) {
    if (strcmp(head->pid, pid_str) == 0) { // 相等
      if (head->chld)
        insert_node(node, head->chld);
      else
        head->chld = node;
      return 0;
    } else {
      if (0 == insert(head->chld, node, pid_str))
        return 0;
    }
    head = head->sibl;
  }
  // printf("insert done\n");
  return -1;
}
int cmp(const char *a, const char *b) {
  int tempa = atoi(a), tempb = atoi(b);
  if (a == b)
    return 0;
  if (a > b)
    return 1;
  return -1;
}
void insert_node(process *node, process *head) {
  process *current = head->sibl;
  process *previous = head;
  while (current) {
    int state = strcmp(head->name, node->name); // 比较目标是否一致
    if (1 == state || (0 == state && -1 == cmp(head->pid, node->name))) {
      previous->sibl = node;
      node->sibl = current;
      return;
    }
    previous = current;
    current = current->sibl;
  }
  previous->sibl = node;
  // printf("insert_node done\n");
  return;
}
process *create_pro(char *pro_name, char *pro_pid) {
  // printf("create_process\n");
  process *p1 = (process *)malloc(sizeof(process));
  if (p1 == NULL) { // 检验
    perror("memory error\n");
    exit(0);
  }
  p1->name = (char *)malloc(sizeof(char) * (strlen(pro_name) + 1));
  if (p1->name == NULL) {
    perror("memory error\n");
    exit(0);
  }
  strcpy(p1->name, pro_name);

  p1->pid = (char *)malloc(sizeof(char) * (strlen(pro_pid) + 1));
  strcpy(p1->pid, pro_pid);
  if (p1->pid == NULL) {
    perror("memory error\n");
    exit(0);
  }
  // printf("cpid: %s, cname: %s\n",p1->pid,p1->name);
  p1->sibl = NULL;
  p1->chld = NULL;
  return p1;
}

void print_tree(process *head, int len, int *buf, int flag) {
  if (head == NULL) {
    return;
  } // 错误
    // printf("p\n");
  while (head != NULL) {
    printf("%s", head->name); // 先打印头Node名字
    if (flag == 1)
      printf("(%s)", head->pid); // 打印pid
    if (head->chld != NULL) {
      if (head->chld->sibl == NULL)
        printf("\u2500\u2500\u2500");
      else
        printf("\u2500\u252c\u2500");
      int size =
          strlen(head->name) + len + 3 + (flag * (strlen(head->pid) + 2));
      if (head->chld->sibl != NULL)
        buf[size - 2] = 1;
      print_tree(head->chld, size, buf, flag);
    }
    head = head->sibl;
    if (head != NULL) {
      printf("\n");
      if (!head->sibl)
        buf[len - 2] = 0;
      for (int i = 0; i < len - 2; ++i)
        if (buf[i])
          printf("\u2502");
        else
          printf(" ");
      if (!head->sibl)
        printf("\u2514\u2500");
      else
        printf("\u251c\u2500");
    }
  }
}