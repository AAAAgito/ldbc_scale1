#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_THREAD_COUNT 2048

typedef struct TProcessInfo {
	unsigned int TGID;
	unsigned int PID;
	unsigned int PPID;
	char ProcessName[200];
	struct TProcessInfo *ChildBranch;
	struct TProcessInfo *BrotherBranch;
	char IsFirstBranch;
	char IsThreadGroup;
	char IsVisited;
} ProcessInfo;

ProcessInfo ProcessInfoPool[MAX_THREAD_COUNT];
ProcessInfo Root;

char ShowPIDCall;
char NumericSortCall;
char NoTreePrintCall;
char UseASCIICall;
char DoNotCompactCall;
char NodePrefix[1000];
char FilterNeq[100];

struct dirent **FirstList;
struct dirent **SecondList;
int ProcessCount;
int SameFactor = 0;

int alphasort(const struct dirent **a, const struct dirent **b);

//to read file by line
int ReadFile(FILE *fp, char a[], int num)
{
	int i, k;
	k = 0;
	while ((i = fgetc(fp)) != EOF && i != '\n' && k < num - 1)
		a[k++] = i;
	a[k] = '\0';
	return k;
}

//to compare two string
int CompareStr(char a[], char b[])
{
	int la = strlen(a);
	int lb = strlen(b);
	int i;
	if (la != lb) {
		return 0;
	} else {
		for (i = 0; i < la; i++) {
			if (a[i] != b[i]) {
				return 0;
			}
		}
		return 1;
	}
}

//present node has brother, but no child
int YesBroNoChild(ProcessInfo *PresentNode)
{
	if (PresentNode->ChildBranch == NULL &&
	    PresentNode->BrotherBranch != NULL) {
		return 1;
	}
	return 0;
}

//present node has no brother, but child
int NoBroYesChild(ProcessInfo *PresentNode)
{
	if (PresentNode->ChildBranch != NULL &&
	    PresentNode->BrotherBranch == NULL) {
		return 1;
	}
	return 0;
}

//present node has brother, child
int YesBroYesChild(ProcessInfo *PresentNode)
{
	if (PresentNode->ChildBranch != NULL &&
	    PresentNode->BrotherBranch != NULL) {
		return 1;
	}
	return 0;
}

//present node has no brother, no child
int NoBroNoChild(ProcessInfo *PresentNode)
{
	if (PresentNode->ChildBranch == NULL &&
	    PresentNode->BrotherBranch == NULL) {
		return 1;
	}
	return 0;
}

//to compact same nodes
int SameTogether(ProcessInfo *PresentNode, char a[])
{
	char testRBQ[100];
	int i;
	SameFactor = 0;

	if (ShowPIDCall) {
		if (PresentNode->IsThreadGroup)
			sprintf(testRBQ, "{%s}(%d)", PresentNode->ProcessName,
				PresentNode->PID);
		else
			sprintf(testRBQ, "%s(%d)", PresentNode->ProcessName,
				PresentNode->PID);
	} else {
		if (PresentNode->IsThreadGroup)
			sprintf(testRBQ, "{%s}", PresentNode->ProcessName);
		else
			sprintf(testRBQ, "%s", PresentNode->ProcessName);
	}

	i = CompareStr(testRBQ, a);

	if (YesBroNoChild(PresentNode) && i == 1) {
		return 1 + SameTogether(PresentNode->BrotherBranch, a);
	} else {
		if ((i != 1)) {
			SameFactor = 1;
			return 0;
		} else if (NoBroYesChild(PresentNode)) {
			SameFactor = 2;
			return 0;
		} else if (NoBroNoChild(PresentNode)) {
			SameFactor = 3;
			return 1;
		} else if ((YesBroYesChild(PresentNode))) {
			return 0 + SameTogether(PresentNode->BrotherBranch, a);
		}
	}
}

//to scan directory
int ScanDir(const struct dirent *Dir)
{
	int i;
	if (!strcmp(Dir->d_name, FilterNeq))
		return 0;
	for (i = 0; i < strlen(Dir->d_name); i++) {
		if (Dir->d_name[i] < '0' || Dir->d_name[i] > '9')
			return 0;
	}
	return 1;
}

//to print out node
void DrawNode(ProcessInfo *PresentNode)
{
	int LastLength = strlen(NodePrefix);
	char PresentMess[100];
	int i;
	int k;
	int count; // to record the times of compact

	if (ShowPIDCall) {
		if (PresentNode->IsThreadGroup)
			sprintf(PresentMess, "{%s}(%d)",
				PresentNode->ProcessName, PresentNode->PID);
		else
			sprintf(PresentMess, "%s(%d)", PresentNode->ProcessName,
				PresentNode->PID);
	} else {
		if (PresentNode->IsThreadGroup)
			sprintf(PresentMess, "{%s}", PresentNode->ProcessName);
		else
			sprintf(PresentMess, "%s", PresentNode->ProcessName);
	}
	if (PresentNode->IsFirstBranch) {
		if (PresentNode->BrotherBranch == NULL) { //no bro

			if (PresentNode->ChildBranch != NULL) { //have child
				if (strcmp(PresentMess, "systemd") == 0 ||
				    strcmp(PresentMess, "systemd(1)") == 0) {
					printf("%s", PresentMess);
				} else {
					if (UseASCIICall == 1) {
						printf("--%s", PresentMess);
					} else {
						printf("──%s", PresentMess);
					}
				}
				if (UseASCIICall == 1) {
					printf("-");
				} else {
					printf("─");
				}
				if (strcmp(PresentMess, "systemd") == 0 ||
				    strcmp(PresentMess, "systemd(1)") == 0) {
					for (i = LastLength;
					     i < LastLength +
							 strlen(PresentMess) +
							 1;
					     i++) {
						NodePrefix[i] = ' ';
					}
				} else {
					for (i = LastLength;
					     i < LastLength +
							 strlen(PresentMess) +
							 3;
					     i++) {
						NodePrefix[i] = ' ';
					}
				}
				NodePrefix[i] = '\0';
				DrawNode(PresentNode->ChildBranch);
				NodePrefix[LastLength] = '\0';
			} else { //no child
				if (strcmp(PresentMess, "systemd") == 0 ||
				    strcmp(PresentMess, "systemd(1)") == 0) {
					printf("%s", PresentMess);
				} else {
					if (UseASCIICall == 1) {
						printf("--%s", PresentMess);
					} else {
						printf("──%s", PresentMess);
					}
				}
				printf("\n");
			}
		} else { //have bro

			if (PresentNode->ChildBranch != NULL) { //have child
				if (UseASCIICall == 1) {
					char pig[1] = "|";
					printf("+-%s", PresentMess);
					printf("-");
					strcat(NodePrefix, pig);
					for (i = LastLength + 1;
					     i < LastLength +
							 strlen(PresentMess) +
							 3;
					     i++) {
						NodePrefix[i] = ' ';
					}
				} else {
					printf("┬─%s", PresentMess);
					printf("─");
					strcat(NodePrefix, "│");
					for (i = LastLength + 3;
					     i < LastLength +
							 strlen(PresentMess) +
							 5;
					     i++) {
						NodePrefix[i] = ' ';
					}
				}

				NodePrefix[i] = '\0';
				DrawNode(PresentNode->ChildBranch);
				NodePrefix[LastLength] = '\0';
				DrawNode(PresentNode->BrotherBranch);
			} else { //no child
				if (DoNotCompactCall == 1) {
					if (UseASCIICall == 1) {
						printf("+-%s", PresentMess);
					} else {
						printf("┬─%s", PresentMess);
					}
					printf("\n");
					DrawNode(PresentNode->BrotherBranch);
				} else {
					count = 1;
					count = SameTogether(PresentNode,
							     PresentMess);
					if (count != 1) {
						if (ShowPIDCall) {
							if (PresentNode
								    ->IsThreadGroup)
								sprintf(PresentMess,
									"%d*[{%s}(%d)]",
									count,
									PresentNode
										->ProcessName,
									PresentNode
										->PID);
							else
								sprintf(PresentMess,
									"%d*[%s(%d)]",
									count,
									PresentNode
										->ProcessName,
									PresentNode
										->PID);
						} else {
							if (PresentNode
								    ->IsThreadGroup)
								sprintf(PresentMess,
									"%d*[{%s}]",
									count,
									PresentNode
										->ProcessName);
							else
								sprintf(PresentMess,
									"%d*[%s]",
									count,
									PresentNode
										->ProcessName);
						}
					}
					if (SameFactor == 1 ||
					    SameFactor == 2) {
						if (UseASCIICall == 1) {
							printf("+-%s",
							       PresentMess);
						} else {
							printf("┬─%s",
							       PresentMess);
						}
						printf("\n");
						if (count != 1) {
							for (k = 0;
							     k < count - 1;
							     k++) {
								PresentNode =
									PresentNode
										->BrotherBranch;
							}
						}
						DrawNode(
							PresentNode
								->BrotherBranch);
					} else if (SameFactor == 3) {
						if (UseASCIICall == 1) {
							printf("--%s",
							       PresentMess);
						} else {
							printf("──%s",
							       PresentMess);
						}
						printf("\n");
						if (count != 1) {
							for (k = 0;
							     k < count - 1;
							     k++) {
								PresentNode =
									PresentNode
										->BrotherBranch;
							}
						}
					}
				}
			}
		}
	} else {
		printf("%s", NodePrefix);
		if (PresentNode->BrotherBranch == NULL) { //no bro
			if (PresentNode->ChildBranch != NULL) { //have child
				if (UseASCIICall == 1) {
					printf("`-%s", PresentMess);
					printf("-");
				} else {
					printf("└─%s", PresentMess);
					printf("─");
				}
				for (i = LastLength;
				     i < LastLength + strlen(PresentMess) + 3;
				     i++) {
					NodePrefix[i] = ' ';
				}
				NodePrefix[i] = '\0';
				DrawNode(PresentNode->ChildBranch);
				NodePrefix[LastLength] = '\0';
			} else { //no child
				if (UseASCIICall == 1) {
					printf("`-%s", PresentMess);
				} else {
					printf("└─%s", PresentMess);
				}
				printf("\n");
			}
		} else { //have bro
			if (PresentNode->ChildBranch != NULL) { //have child
				if (UseASCIICall == 1) {
					char pig[1] = "|";
					printf("|-%s", PresentMess);
					printf("-");
					strcat(NodePrefix, pig);
					for (i = LastLength + 1;
					     i < LastLength +
							 strlen(PresentMess) +
							 3;
					     i++) {
						NodePrefix[i] = ' ';
					}
				} else {
					printf("├─%s", PresentMess);
					printf("─");
					strcat(NodePrefix, "│");
					for (i = LastLength + 3;
					     i < LastLength +
							 strlen(PresentMess) +
							 5;
					     i++) {
						NodePrefix[i] = ' ';
					}
				}

				NodePrefix[i] = '\0';
				DrawNode(PresentNode->ChildBranch);
				NodePrefix[LastLength] = '\0';
				DrawNode(PresentNode->BrotherBranch);
			} else { //no child
				if (DoNotCompactCall == 1) {
					if (UseASCIICall == 1) {
						printf("|-%s", PresentMess);
					} else {
						printf("├─%s", PresentMess);
					}
					printf("\n");
					DrawNode(PresentNode->BrotherBranch);
				} else {
					count = 1;
					count = SameTogether(PresentNode,
							     PresentMess);
					if (count != 1) {
						if (ShowPIDCall) {
							if (PresentNode
								    ->IsThreadGroup)
								sprintf(PresentMess,
									"%d*[{%s}(%d)]",
									count,
									PresentNode
										->ProcessName,
									PresentNode
										->PID);
							else
								sprintf(PresentMess,
									"%d*[%s(%d)]",
									count,
									PresentNode
										->ProcessName,
									PresentNode
										->PID);
						} else {
							if (PresentNode
								    ->IsThreadGroup)
								sprintf(PresentMess,
									"%d*[{%s}]",
									count,
									PresentNode
										->ProcessName);
							else
								sprintf(PresentMess,
									"%d*[%s]",
									count,
									PresentNode
										->ProcessName);
						}
					}

					if (SameFactor == 1 ||
					    SameFactor == 2) {
						if (UseASCIICall == 1) {
							printf("|-%s",
							       PresentMess);
						} else {
							printf("├─%s",
							       PresentMess);
						}
						printf("\n");
						if (count != 1) {
							for (k = 0;
							     k < count - 1;
							     k++) {
								PresentNode =
									PresentNode
										->BrotherBranch;
							}
						}
						DrawNode(
							PresentNode
								->BrotherBranch);
					} else if (SameFactor == 3) {
						if (UseASCIICall == 1) {
							printf("`-%s",
							       PresentMess);
						} else {
							printf("└─%s",
							       PresentMess);
						}
						printf("\n");
						if (count != 1) {
							for (k = 0;
							     k < count - 1;
							     k++) {
								PresentNode =
									PresentNode
										->BrotherBranch;
							}
						}
					}
				}
			}
		}
	}
}

//to print out tree
void DrawTree()
{
	NodePrefix[0] = '\0';
	if (Root.ChildBranch != NULL) {
		DrawNode(Root.ChildBranch);
	}
}

//to build node
void BuildNode(ProcessInfo *ParentNode)
{
	int Piterator;
	ProcessInfo *CPiterator;
	for (Piterator = 0; Piterator < ProcessCount; Piterator++) {
		if ((ProcessInfoPool[Piterator].PID == 2) ||
		    (ProcessInfoPool[Piterator].IsVisited))
			continue;
		if (ProcessInfoPool[Piterator].PPID == ParentNode->PID) {
			if (NumericSortCall) {
				CPiterator = ParentNode->ChildBranch;
				if (CPiterator == NULL) {
					ProcessInfoPool[Piterator]
						.IsFirstBranch = 1;
					ProcessInfoPool[Piterator]
						.BrotherBranch = NULL;
					ParentNode->ChildBranch =
						&(ProcessInfoPool[Piterator]);
				}
				while (CPiterator != NULL) {
					if (ProcessInfoPool[Piterator].PID >
					    CPiterator->PID) {
						if (CPiterator->BrotherBranch !=
						    NULL) {
							if (ProcessInfoPool
								    [Piterator]
									    .PID <=
							    CPiterator
								    ->BrotherBranch
								    ->PID) {
								ProcessInfoPool[Piterator]
									.BrotherBranch =
									CPiterator
										->BrotherBranch;
								CPiterator
									->BrotherBranch =
									&(ProcessInfoPool
										  [Piterator]);
								break;
							} else {
								CPiterator =
									CPiterator
										->BrotherBranch;
								continue;
							}
						} else {
							CPiterator
								->BrotherBranch = &(
								ProcessInfoPool
									[Piterator]);
							break;
						}
					} else {
						ProcessInfoPool[Piterator]
							.IsFirstBranch = 1;
						ProcessInfoPool[Piterator]
							.BrotherBranch =
							CPiterator;
						CPiterator->IsFirstBranch = 0;
						ParentNode->ChildBranch =
							&(ProcessInfoPool
								  [Piterator]);
						break;
					}
				}
				BuildNode(&(ProcessInfoPool[Piterator]));
			} else {
				ProcessInfoPool[Piterator].BrotherBranch =
					ParentNode->ChildBranch;
				if (ProcessInfoPool[Piterator].BrotherBranch !=
				    NULL)
					ProcessInfoPool[Piterator]
						.BrotherBranch->IsFirstBranch =
						0;
				ProcessInfoPool[Piterator].IsFirstBranch = 1;
				ParentNode->ChildBranch =
					&(ProcessInfoPool[Piterator]);
				BuildNode(ParentNode->ChildBranch);
			}
			ProcessInfoPool[Piterator].IsVisited = 1;
		}
	}
}

//to build tree
void BuildTree()
{
	Root.TGID = 0;
	Root.PID = 0;
	Root.PPID = 0;
	Root.IsThreadGroup = 0;
	Root.ChildBranch = NULL;
	Root.BrotherBranch = NULL;
	BuildNode(&Root);
}

//to scan task
void ScanTask(int SecondListCount, int *PCiterator)
{
	int Head, Tail, Len, SecondListIterator;
	char MessPath[100];
	char TaskPath[100];
	char PresentMess[200];
	char MessID[200];
	FILE *File;
	for (SecondListIterator = 0; SecondListIterator < SecondListCount;
	     (*PCiterator)++, SecondListIterator++) {
		strcpy(MessPath, "/proc/");
		strcat(MessPath, (SecondList[SecondListIterator])->d_name);
		strcat(MessPath, "/status");

		File = fopen(MessPath, "r");
		ProcessInfoPool[*PCiterator].ChildBranch = NULL;
		ProcessInfoPool[*PCiterator].BrotherBranch = NULL;
		ProcessInfoPool[*PCiterator].IsFirstBranch = 0;
		ProcessInfoPool[*PCiterator].IsThreadGroup = 0;
		ProcessInfoPool[*PCiterator].IsVisited = 0;
		while (!feof(File)) {
			ReadFile(File, PresentMess, 200);
			if (strncmp(PresentMess, "Pid", 3) == 0) {
				Len = strlen(PresentMess) + 1; // '\0'
				Head = strlen("Pid:\t");
				for (Tail = 0; Tail < Len - Head; Tail++) {
					MessID[Tail] = PresentMess[Head + Tail];
				}
				ProcessInfoPool[*PCiterator].PID = atoi(MessID);
			} else if (strncmp(PresentMess, "PPid", 4) == 0) {
				Len = strlen(PresentMess) + 1; // '\0'
				Head = strlen("PPid:\t");
				for (Tail = 0; Tail < Len - Head; Tail++) {
					MessID[Tail] = PresentMess[Head + Tail];
				}
				ProcessInfoPool[*PCiterator].PPID =
					atoi(MessID);
			} else if (strncmp(PresentMess, "Name", 4) == 0) {
				Len = strlen(PresentMess) + 1; // '\0'
				Head = strlen("Name:\t");
				for (Tail = 0; Tail < Len - Head; Tail++) {
					MessID[Tail] = PresentMess[Head + Tail];
				}
				strcpy(ProcessInfoPool[*PCiterator].ProcessName,
				       MessID);
			} else if (strncmp(PresentMess, "Tgid", 3) == 0) {
				Len = strlen(PresentMess) + 1; // '\0'
				Head = strlen("Tgid:\t");
				for (Tail = 0; Tail < Len - Head; Tail++) {
					MessID[Tail] = PresentMess[Head + Tail];
				}
				ProcessInfoPool[*PCiterator].TGID =
					atoi(MessID);
			}
		}
		if (ProcessInfoPool[*PCiterator].TGID !=
		    ProcessInfoPool[*PCiterator].PID) {
			ProcessInfoPool[*PCiterator].IsThreadGroup = 1;
			ProcessInfoPool[*PCiterator].PPID =
				ProcessInfoPool[*PCiterator].TGID;
		}
	}
}

//to scan process
void ScanProcess()
{
	int PCiterator, Head, Tail, Len, FirstListIterator, SecondListCount;
	char MessPath[100];
	char TaskPath[100];
	char PresentMess[200];
	char MessID[200];

	FILE *File;
	for (PCiterator = 0, FirstListIterator = 0;
	     FirstListIterator < ProcessCount;
	     PCiterator++, FirstListIterator++) {
		strcpy(MessPath, "/proc/");
		strcat(MessPath, FirstList[FirstListIterator]->d_name);
		strcat(MessPath, "/status");
		strcpy(FilterNeq, FirstList[FirstListIterator]->d_name);
		strcpy(TaskPath, "/proc/");
		strcat(TaskPath, FirstList[FirstListIterator]->d_name);
		strcat(TaskPath, "/task");
		SecondListCount =
			scandir(TaskPath, &SecondList, ScanDir, alphasort);
		if (SecondListCount != 0) {
			ScanTask(SecondListCount, &PCiterator);
			free(SecondList);
		}

		File = fopen(MessPath, "r");
		ProcessInfoPool[PCiterator].ChildBranch = NULL;
		ProcessInfoPool[PCiterator].BrotherBranch = NULL;
		ProcessInfoPool[PCiterator].IsFirstBranch = 0;
		ProcessInfoPool[PCiterator].IsThreadGroup = 0;
		ProcessInfoPool[PCiterator].IsVisited = 0;
		while (!feof(File)) {
			ReadFile(File, PresentMess, 200);
			if (strncmp(PresentMess, "Pid", 3) == 0) {
				Len = strlen(PresentMess) + 1; // Include '\0'
				Head = strlen("Pid:\t");
				for (Tail = 0; Tail < Len - Head; Tail++) {
					MessID[Tail] = PresentMess[Head + Tail];
				}
				ProcessInfoPool[PCiterator].PID = atoi(MessID);
			} else if (strncmp(PresentMess, "PPid", 4) == 0) {
				Len = strlen(PresentMess) + 1; // Include '\0'
				Head = strlen("PPid:\t");
				for (Tail = 0; Tail < Len - Head; Tail++) {
					MessID[Tail] = PresentMess[Head + Tail];
				}
				ProcessInfoPool[PCiterator].PPID = atoi(MessID);
			} else if (strncmp(PresentMess, "Name", 4) == 0) {
				Len = strlen(PresentMess) + 1; // Include '\0'
				Head = strlen("Name:\t");
				for (Tail = 0; Tail < Len - Head; Tail++) {
					MessID[Tail] = PresentMess[Head + Tail];
				}
				strcpy(ProcessInfoPool[PCiterator].ProcessName,
				       MessID);
			} else if (strncmp(PresentMess, "Tgid", 3) == 0) {
				Len = strlen(PresentMess) + 1; // Include '\0'
				Head = strlen("Tgid:\t");
				for (Tail = 0; Tail < Len - Head; Tail++) {
					MessID[Tail] = PresentMess[Head + Tail];
				}
				ProcessInfoPool[PCiterator].TGID = atoi(MessID);
			}
		}
	}
	ProcessCount = PCiterator;
}

int main(int argc, char **argv)
{
	int i;
	ShowPIDCall = 0;
	NumericSortCall =
		1; //a tolerant fault to make the compact work properly
	NoTreePrintCall = 0;
	UseASCIICall = 0;
	DoNotCompactCall = 0;
	FilterNeq[0] = '\0';
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-A") == 0) {
			UseASCIICall = 1;
		} else if (strcmp(argv[i], "-c") == 0) {
			DoNotCompactCall = 1;
		} else if (strcmp(argv[i], "-n") == 0) {
			NumericSortCall = 1;
		} else if (strcmp(argv[i], "-p") == 0) {
			ShowPIDCall = 1;
		} else if (strcmp(argv[i], "-V") == 0) {
			printf("pstree (PSmisc) 22.21\nCopyright (C) 1993-2009 Werner Almesberger and Craig Small\n");
			NoTreePrintCall = 1;
		} else {
			printf("Unknown argument: %s\n", argv[i]);
			printf("Usage:\n  -A, --ascii:\t\tUse ASCII line drawing characters.\n  -c, --compact:\tDo not compact identical subtrees.\n  -n, --numeric-sort:\tSort output by PID.\n  -p, --show-pids:\tShow PIDs.\n  -V, --version:\tDisplay version information.\n");
			NoTreePrintCall = 1;
		}
	}
	if (!NoTreePrintCall) {
		ProcessCount = scandir("/proc", &FirstList, ScanDir, alphasort);
		if (ProcessCount < 0)
			perror("FATAL: Procedure scandir failed.\n");
		else {
			ScanProcess();
			BuildTree();
			DrawTree();
			free(FirstList);
		}
	}
	return 0;
}