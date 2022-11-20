#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOL int
#define TRUE 1
#define FALSE 0

typedef struct TNode {
	unsigned int TGID;
	unsigned int PID;
	unsigned int PPID;
	char ProcessName[200];
	struct TNode *ChildProcess;
	struct TNode *PeerProcess;
	BOOL checkFirstChild;
	BOOL InThreadGroup;
	BOOL HasBeenConsidered;
} Node;

char p_argument;
char no_execute;

struct dirent **Names;
struct dirent **TaskNames;
int pCount;
int first_int = 0;

char preSpace[1000];

char fx_select_aug[100];

Node NodePool[2000];
Node Root;

int select_aug(const struct dirent *Directory)
{
	int i;
	if (!strcmp(Directory->d_name, fx_select_aug))
		return 0;
	for (i = 0; i < strlen(Directory->d_name); i++) {
		if (Directory->d_name[i] < '0' || Directory->d_name[i] > '9')
			return 0;
	}
	return 1;
}

int FX_READ(FILE *fp, char s[], int lim)
{
	int c, i;
	i = 0;
	while ((c = fgetc(fp)) != EOF && c != '\n' && i < lim - 1)
		s[i++] = c;
	s[i] = '\0';
	return i;
}

void buildNode(Node *ParentNode)
{
	int ProcessIterator;
	Node *ChildProcessIterator;
	for (ProcessIterator = 0; ProcessIterator < pCount; ProcessIterator++) {
		if (NodePool[ProcessIterator].PID == 2)
			continue;
		if (NodePool[ProcessIterator].HasBeenConsidered)
			continue;
		if (NodePool[ProcessIterator].PPID == ParentNode->PID) {
			NodePool[ProcessIterator].PeerProcess =
				ParentNode->ChildProcess;
			if (NodePool[ProcessIterator].PeerProcess != NULL)
				NodePool[ProcessIterator]
					.PeerProcess->checkFirstChild = FALSE;
			NodePool[ProcessIterator].checkFirstChild = TRUE;
			ParentNode->ChildProcess = &(NodePool[ProcessIterator]);
			buildNode(ParentNode->ChildProcess);
			NodePool[ProcessIterator].HasBeenConsidered = TRUE;
		}
	}
}

void PrintNode(Node *CurrentNode)
{
	int LastpreSpacelength = strlen(preSpace);
	char buffer[100];
	int i;
	if (p_argument) {
		if (CurrentNode->InThreadGroup)
			sprintf(buffer, "{%s}(%d)", CurrentNode->ProcessName,
				CurrentNode->PID);
		else
			sprintf(buffer, "%s(%d)", CurrentNode->ProcessName,
				CurrentNode->PID);
	} else {
		if (CurrentNode->InThreadGroup)
			sprintf(buffer, "{%s}", CurrentNode->ProcessName);
		else
			sprintf(buffer, "%s", CurrentNode->ProcessName);
	}
	if (CurrentNode->checkFirstChild) {
		if (CurrentNode->PeerProcess == NULL) {
			if (first_int != 0) {
				first_int++;
				printf("──%s", buffer);
				if (CurrentNode->ChildProcess != NULL) {
					printf("─");
					for (i = LastpreSpacelength;
					     i < LastpreSpacelength +
							 strlen(buffer) + 3;
					     i++) {
						preSpace[i] = ' ';
					}
					preSpace[i] = '\0';
					PrintNode(CurrentNode->ChildProcess);
					preSpace[LastpreSpacelength] = '\0';
				} else
					printf("\n");
			} else {
				first_int++;
				printf("%s", buffer);
				if (CurrentNode->ChildProcess != NULL) {
					printf("─");
					for (i = LastpreSpacelength;
					     i < LastpreSpacelength +
							 strlen(buffer) + 1;
					     i++) {
						preSpace[i] = ' ';
					}
					preSpace[i] = '\0';
					PrintNode(CurrentNode->ChildProcess);
					preSpace[LastpreSpacelength] = '\0';
				} else
					printf("\n");
			}
		} else {
			printf("┬─%s", buffer);
			if (CurrentNode->ChildProcess != NULL) {
				printf("─");
				strcat(preSpace, "│");
				for (i = LastpreSpacelength + 3;
				     i <
				     LastpreSpacelength + strlen(buffer) + 5;
				     i++) {
					preSpace[i] = ' ';
				}
				preSpace[i] = '\0';
				PrintNode(CurrentNode->ChildProcess);
				preSpace[LastpreSpacelength] = '\0';
			} else
				printf("\n");

			PrintNode(CurrentNode->PeerProcess);
		}
	} else {
		printf("%s", preSpace);
		if (CurrentNode->PeerProcess == NULL) {
			printf("└─%s", buffer);
			if (CurrentNode->ChildProcess != NULL) {
				printf("─");
				for (i = LastpreSpacelength;
				     i <
				     LastpreSpacelength + strlen(buffer) + 3;
				     i++) {
					preSpace[i] = ' ';
				}
				preSpace[i] = '\0';
				PrintNode(CurrentNode->ChildProcess);
				preSpace[LastpreSpacelength] = '\0';
			} else
				printf("\n");
		} else {
			printf("├─%s", buffer);
			if (CurrentNode->ChildProcess != NULL) {
				printf("─");
				strcat(preSpace, "│");
				for (i = LastpreSpacelength + 3;
				     i <
				     LastpreSpacelength + strlen(buffer) + 5;
				     i++) {
					preSpace[i] = ' ';
				}
				preSpace[i] = '\0';
				PrintNode(CurrentNode->ChildProcess);
				preSpace[LastpreSpacelength] = '\0';
			} else
				printf("\n");
			PrintNode(CurrentNode->PeerProcess);
		}
	}
}

int main(int argc, char **argv)
{
	int i;
	p_argument = 0;
	no_execute = 0;
	fx_select_aug[0] = '\0';
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0) {
			p_argument = 1;
		} else if (strcmp(argv[i], "-V") == 0 ||
			   strcmp(argv[i], "--version") == 0) {
			printf("pstree (PSmisc) 22.21\nCopyright (C) 1993-2009 Werner "
			       "Almesberger and Craig Small\n\nPSmisc comes with ABSOLUTELY NO "
			       "WARRANTY.\nThis is free software, and you are welcome to "
			       "redistribute it under\nthe terms of the GNU General Public "
			       "License.\nFor more information about these matters, see the "
			       "files named COPYING.\n");
			no_execute = 1;
		}
	}
	if (!no_execute) {
		pCount = scandir("/proc", &Names, select_aug, alphasort);

		if (pCount < 0)
			perror("FATAL: Procedure scandir failed.\n");
		else {
			int fx_pCountIterator, fx_start, fx_end, fx_length,
				fx_NamesIterator, TaskNamesCount;
			char fx_Path[100];
			char fx_TaskPath[100];
			char fx_buffer[200];
			char fx_IDBuffer[200];

			FILE *fxtcl; // wo tai cai le
			for (fx_pCountIterator = 0, fx_NamesIterator = 0;
			     fx_NamesIterator < pCount;
			     fx_pCountIterator++, fx_NamesIterator++) {
				strcpy(fx_Path, "/proc/");
				strcat(fx_Path,
				       Names[fx_NamesIterator]->d_name);
				strcat(fx_Path, "/status");

				strcpy(fx_select_aug,
				       Names[fx_NamesIterator]->d_name);
				strcpy(fx_TaskPath, "/proc/");
				strcat(fx_TaskPath,
				       Names[fx_NamesIterator]->d_name);
				strcat(fx_TaskPath, "/task");
				TaskNamesCount =
					scandir(fx_TaskPath, &TaskNames,
						select_aug, alphasort);
				if (TaskNamesCount != 0) {
					int start, end, length,
						TaskNamesIterator;
					char Path[100];
					char TaskPath[100];
					char buffer[200];
					char IDBuffer[200];
					FILE *tcl; // tai cai le
					for (TaskNamesIterator = 0;
					     TaskNamesIterator < TaskNamesCount;
					     (fx_pCountIterator)++,
					    TaskNamesIterator++) {
						strcpy(Path, "/proc/");
						strcat(Path,
						       (TaskNames[TaskNamesIterator])
							       ->d_name);
						strcat(Path, "/status");

						tcl = fopen(Path, "r");
						NodePool[fx_pCountIterator]
							.ChildProcess = NULL;
						NodePool[fx_pCountIterator]
							.PeerProcess = NULL;
						NodePool[fx_pCountIterator]
							.checkFirstChild =
							FALSE;
						NodePool[fx_pCountIterator]
							.InThreadGroup = FALSE;
						NodePool[fx_pCountIterator]
							.HasBeenConsidered =
							FALSE;
						while (!feof(tcl)) {
							FX_READ(tcl, buffer,
								200);
							if (strncmp(buffer,
								    "Pid",
								    3) == 0) {
								length =
									strlen(buffer) +
									1; // Include '\0'
								start = strlen(
									"Pid:\t");
								for (end = 0;
								     end <
								     length -
									     start;
								     end++) {
									IDBuffer[end] = buffer
										[start +
										 end];
								}
								NodePool[fx_pCountIterator]
									.PID = atoi(
									IDBuffer);
							} else if (strncmp(buffer,
									   "PPid",
									   4) ==
								   0) {
								length =
									strlen(buffer) +
									1; // Include '\0'
								start = strlen(
									"PPid:\t");
								for (end = 0;
								     end <
								     length -
									     start;
								     end++) {
									IDBuffer[end] = buffer
										[start +
										 end];
								}
								NodePool[fx_pCountIterator]
									.PPID = atoi(
									IDBuffer);
							} else if (strncmp(buffer,
									   "Name",
									   4) ==
								   0) {
								length =
									strlen(buffer) +
									1; // Include '\0'
								start = strlen(
									"Name:\t");
								for (end = 0;
								     end <
								     length -
									     start;
								     end++) {
									IDBuffer[end] = buffer
										[start +
										 end];
								}
								strcpy(NodePool[fx_pCountIterator]
									       .ProcessName,
								       IDBuffer);
							} else if (strncmp(buffer,
									   "Tgid",
									   3) ==
								   0) {
								length =
									strlen(buffer) +
									1; // Include '\0'
								start = strlen(
									"Tgid:\t");
								for (end = 0;
								     end <
								     length -
									     start;
								     end++) {
									IDBuffer[end] = buffer
										[start +
										 end];
								}
								NodePool[fx_pCountIterator]
									.TGID = atoi(
									IDBuffer);
							}
						}
						if (NodePool[fx_pCountIterator]
							    .TGID !=
						    NodePool[fx_pCountIterator]
							    .PID) {
							NodePool[fx_pCountIterator]
								.InThreadGroup =
								TRUE;
							NodePool[fx_pCountIterator]
								.PPID =
								NodePool[fx_pCountIterator]
									.TGID;
						}
					}
					free(TaskNames);
				}

				fxtcl = fopen(fx_Path, "r");
				NodePool[fx_pCountIterator].ChildProcess = NULL;
				NodePool[fx_pCountIterator].PeerProcess = NULL;
				NodePool[fx_pCountIterator].checkFirstChild =
					FALSE;
				NodePool[fx_pCountIterator].InThreadGroup =
					FALSE;
				NodePool[fx_pCountIterator].HasBeenConsidered =
					FALSE;
				while (!feof(fxtcl)) {
					FX_READ(fxtcl, fx_buffer, 200);
					if (strncmp(fx_buffer, "Pid", 3) == 0) {
						fx_length = strlen(fx_buffer) +
							    1; // Include '\0'
						fx_start = strlen("Pid:\t");
						for (fx_end = 0;
						     fx_end <
						     fx_length - fx_start;
						     fx_end++) {
							fx_IDBuffer[fx_end] =
								fx_buffer[fx_start +
									  fx_end];
						}
						NodePool[fx_pCountIterator].PID =
							atoi(fx_IDBuffer);
					} else if (strncmp(fx_buffer, "PPid",
							   4) == 0) {
						fx_length = strlen(fx_buffer) +
							    1; // Include '\0'
						fx_start = strlen("PPid:\t");
						for (fx_end = 0;
						     fx_end <
						     fx_length - fx_start;
						     fx_end++) {
							fx_IDBuffer[fx_end] =
								fx_buffer[fx_start +
									  fx_end];
						}
						NodePool[fx_pCountIterator]
							.PPID =
							atoi(fx_IDBuffer);
					} else if (strncmp(fx_buffer, "Name",
							   4) == 0) {
						fx_length = strlen(fx_buffer) +
							    1; // Include '\0'
						fx_start = strlen("Name:\t");
						for (fx_end = 0;
						     fx_end <
						     fx_length - fx_start;
						     fx_end++) {
							fx_IDBuffer[fx_end] =
								fx_buffer[fx_start +
									  fx_end];
						}
						strcpy(NodePool[fx_pCountIterator]
							       .ProcessName,
						       fx_IDBuffer);
					} else if (strncmp(fx_buffer, "Tgid",
							   3) == 0) {
						fx_length = strlen(fx_buffer) +
							    1; // Include '\0'
						fx_start = strlen("Tgid:\t");
						for (fx_end = 0;
						     fx_end <
						     fx_length - fx_start;
						     fx_end++) {
							fx_IDBuffer[fx_end] =
								fx_buffer[fx_start +
									  fx_end];
						}
						NodePool[fx_pCountIterator]
							.TGID =
							atoi(fx_IDBuffer);
					}
				}
			}
			pCount = fx_pCountIterator;
			Root.TGID = 0;
			Root.PID = 0;
			Root.PPID = 0;
			Root.InThreadGroup = FALSE;
			Root.ChildProcess = NULL;
			Root.PeerProcess = NULL;
			buildNode(&Root);
			preSpace[0] = '\0';
			if (Root.ChildProcess != NULL) {
				PrintNode(Root.ChildProcess);
			}
			free(Names);
		}
	}
	return 0;
}
