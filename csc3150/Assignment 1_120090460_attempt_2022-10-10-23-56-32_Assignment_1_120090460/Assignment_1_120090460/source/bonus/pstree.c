#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct node
{
    char pname[128];
    pid_t pid;
    pid_t ppid;
    pid_t pgid;
    int threadNumber;
    struct node* parent;
    struct node* child[512];
    struct node* next;

    int childNumber;
};

static struct node* listHead;

void createNode(char* fileName);
struct node* findNodeWithPID(pid_t pid);
void createTree(void);
void printTree(const struct node* root, const char* indent);
void printTreeP(const struct node* head, const char* indent);
void printTreec(const struct node* head, const char* indent);

// int main(int argc, char* argv[]) {
int main()
{
    DIR* dirStream = opendir("/proc");
    struct dirent* pidDir;

    char dirName[128];

    if (dirStream == NULL)
    {
        perror("The process file cannot be gotten successfully");
        return 0;
    }
    // printf("get the directory\n");
    while ((pidDir = readdir(dirStream)) != NULL)
    {
        if (pidDir->d_type == DT_DIR)
        {
            //            check whether it is a directory
            // printf("The directory name is %s\n", pidDir->d_name);
            if (pidDir->d_name[0] >= 48 && pidDir->d_name[0] <= 57)
            {
                //                the directory is a process directory

                strcpy(dirName, "/proc/");
                strcat(dirName, pidDir->d_name);
                strcat(dirName, "/stat");
                // printf("The directory name %s\n", dirName);
                createNode(dirName);
            }
        }
    }

    struct node* head = listHead;

    createTree();

    while (head->parent != NULL)
    {
        head = head->parent;
    }

    printf("The root node is %s\n and the ppid is %d\n and one is %s\n, the ppid for 1 is %d\n", head->pname, head->ppid, head->next->pname, head->next->ppid);


    // char * arg[argc];

    // for (int i = 0; i < argc - 1; i++) {
    //     arg[i] = argv[i + 1];
    // }
    // arg[argc - 1] = NULL;

    // switch (arg[argc - 2][1])
    // {
    // case 'p':
    // {
    //     //        add the pid
    //     //            printTreeP(head, "");
    // }
    // case 'c':
    // {
    //     //        if it is a thread, list the name if the process name is different to the thread
    //     //            printTreec(head, "");
    // }
    // case 't':
    // {
    //     //        expand all the threads, the thread name is the same to the process
    // }
    // case 'g':
    // {
    //     //        add the 5th parameter of stat
    // }
    // case 'T':
    // {
    //     //        check the 4 and 5 parameter of stat
    //     //        if they are equal, this is a process or they are a thread
    // }
    //     //        default: printTree(head);
    // default:;
    // }
    return 0;
}

void createNode(char* fileName)
{
    FILE* pidFile = fopen(fileName, "r");
    if (pidFile == NULL)
    {
        return;
    }
    char statFile[256];
    char* token;

    fgets(statFile, sizeof(statFile), pidFile);

    //    1 is the pid
    //    2 is the name of the process
    //    4 is the parent pid
    //    5 is the process group id
    //    20 is the number of the thread
    struct node* pstreeNode;
    pstreeNode = (struct node*)malloc(sizeof(struct node));
    pstreeNode->pid = atoi(strtok(statFile, " "));
    pstreeNode->childNumber = 0;
    for (int i = 2; i <= 20; i++)
    {
        token = strtok(NULL, " ");
        switch (i)
        {
            case 2:
            {
                strcpy(pstreeNode->pname, token);
                //                break;
            }
            case 4:
            {
                pstreeNode->ppid = atoi(token);

                //                break;
            }
            case 5:
            {
                pstreeNode->pgid = atoi(token);
                //                break;
            }
            case 20:
            {
                pstreeNode->threadNumber = atoi(token);
                //                break;
            }
            default:;
        }
    }

    pstreeNode->next = listHead;
    listHead = pstreeNode;
}

struct node* findNodeWithPID(pid_t pid)
{
    struct node* pstreeNode;
    for (pstreeNode = listHead; pstreeNode != NULL; pstreeNode = pstreeNode->next)
    {
        if (pstreeNode->pid == pid)
        {
            return pstreeNode;
        }
    }
    return NULL;
}

void createTree(void)
{
    struct node *pNode, *ppNode;
    int forTime = 0;

    for (pNode = listHead; pNode != NULL; pNode = pNode->next)
    {

        ppNode = findNodeWithPID(pNode->ppid);
        if (ppNode != NULL)
        {

            ppNode->child[ppNode->childNumber++] = pNode;
            pNode->parent = ppNode;

        }
    }
}

/*The main idea is that, we print a process name once we can print it
 * At the same time, we maintain a new line to record the next line
 * and it will update when a new child is found
 * the new line will release if it has no child
 * since this line is ended
 * and it go back to the next line of previous line*/

void printTree(const struct node* head, const char* indent)
{
    printf("%s", head->pname);

    if (head->childNumber == 0)
    {
        printf("\n");
        return;
    }
    else if (head->childNumber == 1)
    {
        char* new_indent = (char*)malloc(strlen(indent) + strlen(head->pname) + 3);
        //        printf("---");
        sprintf(new_indent, "%s\n", indent);
        printTree(head->child[0], new_indent);
    }
    else
    {
        //        printf("--");
        //            I should consider the thread
        if (head->threadNumber > 1)
        {
            char* new_indent = (char*)malloc(strlen(indent) + strlen(head->pname) + 3 + 4 + 5);
            sprintf(new_indent, "%s%d*[{%s}]\n", "---", head->threadNumber - 1, head->pname);
            printf("%s", new_indent);
            return;
        }
        else
        {
            char* new_indent = (char*)malloc(strlen(indent) + strlen(head->pname) + 3);
            //            I should consider the indent (vertical bar for the next level)
            sprintf(new_indent, "%s |-%s", indent, head->pname);
            //                Here directly print
            printTree(head->child[0], new_indent);
            for (int childCount = 1; childCount < head->childNumber; childCount++)
            {
                printTree(head->child[childCount++], new_indent);
            }
            //            this is where to manipulate the multi child
            //            At the same time, here the new line always means the next line,
            //            with that next line, we should maintain the previous vertical bar
            //            I should try to use the branch bar and end bar
        }
        return;
    }
}

void printTreeP(const struct node* head, const char* indent)
{
    printf("%s(%d)", head->pname, head->pid);

    int childCount = 0;
    if (head->childNumber == 0)
    {
        return;
    }
    else if (childCount < head->childNumber)
    {
        printf("--");
        if (childCount == head->childNumber + 1)
        {
            //            I should consider the thread
            //            if (head->threadNumber > 1) {
            //                printf("%d*[{%s}]", head->threadNumber - 1, head->pname);
            //                return;
            //            }

            printf("|");
        }
        printf("--");
        //        printTreeP(head->child[childCount++]);
    }
    else
    {
        return;
    }
}

void printTreec(const struct node* head, const char* indent)
{
    printf("%s(%d)", head->pname, head->pid);

    int childCount = 0;
    if (head->childNumber == 0)
    {
        printf("\n");
        return;
    }
    else if (childCount == 1)
    {
        char* new_indent = (char*)malloc(strlen(indent) + strlen(head->pname) + 3);
        printf("---");
        sprintf(new_indent, "%s", indent);
        printTreec(head->child[0], new_indent);
        //        if (childCount == head->childNumber + 1) {
        ////            I should consider the thread
        //            if (head->threadNumber > 1) {
        //                if (head->child[1]->pname == head->pname) {
        //                    printTreec(head->child[childCount++]);
        //                } else {
        //                    printf("%d*[{%s}]", head->threadNumber - 1, head->pname);
        //                    return;
        //                }
        //            }
        //            printf(" |-");

        //        printf("--");
        //        printTreec(head->child[childCount++]);
    }
    else
    {
        return;
    }
}

void printTreeT(struct node* head)
{
    printf("%s(%d)", head->pname, head->pid);

    int childCount = 0;
    if (head->childNumber == 0)
    {
        return;
    }
    else if (childCount < head->childNumber)
    {
        printf("--");
        if (childCount == head->childNumber + 1)
        {
            //            I should consider the thread
            if (head->threadNumber > 1)
            {
                for (int i = 0; i < head->threadNumber - 1; i++)
                {
                    printf("[{%s}]\n", head->pname);
                    printf("|--");
                }
                return;
            }

            printf("|");
        }
        printf("--");
        printTreeT(head->child[childCount++]);
    }
    else
    {
        return;
    }
}
