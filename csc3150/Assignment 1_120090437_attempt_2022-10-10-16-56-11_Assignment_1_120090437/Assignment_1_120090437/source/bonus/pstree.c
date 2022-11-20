#include <stdio.h> /* for printf */
#include <stdlib.h> /* for exit */
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

struct pidnum {
	int pidnum;
	int ppidnum;
	char pidname[50];
	char isThread;
};

struct pidnum procPids[500];
int proNum = 0;

// typedef struct proc{
//   struct proc *parent;
//   struct proc *child;
//   struct proc *next;
//   pid_t pid;
//   char comm[50];
//   char isThread;
// }PROC;

// typedef struct child{
//   const PROC *proc;
//   struct child *next;
// }CHILD;

void readPsInfo()
{
	DIR *dir;
	struct dirent *ptr;
	// char pid[20];
	int pid;
	char pidchar[20];
	char filename[50];
	memset(filename, '\0', sizeof(filename));
	char strLine[50];
	char pName[50];
	char *pos;
	int ppid;

	//read process status
	dir = opendir("/proc");
	if (dir == NULL) {
		printf("cannot open /proc");
		perror("/proc");
		exit(1);
	}

	while ((ptr = readdir(dir)) != NULL) { //read each pid status
		// strcpy(pid,ptr->d_name);
		pid = atoi(ptr->d_name);

		if (pid) {
			//strcpy(filename, getProcPath(pid));
			//filename[50] = getProcPath(&pid);
			strcat(filename, "/proc/");
			sprintf(pidchar, "%d", pid);
			//printf("%s\n",pidchar);
			strcat(filename, pidchar);
			strcat(filename, "/stat");

			// printf("%s\n",filename);
			// break;

			FILE *fp = fopen(filename, "r");
			// printf("%d",(fp!=0));
			// break;
			if (fp != 0) {
				// printf("inside if\n");
				fgets(strLine, 50, fp);
				sscanf(strLine, "%*[^(](%[^)]", pName);
				pos = strrchr(strLine, ')') +
				      4; //pointer to ppid
				ppid = atoi(pos);

				//new struct, and save into procPids[]
				procPids[proNum].isThread = 0;
				strcpy(procPids[proNum].pidname, pName);
				procPids[proNum].ppidnum = ppid;
				// procPids[proNum++].pidnum = atoi(pid);
				procPids[proNum++].pidnum = pid;

				//read thread status
				char threadname[50];
				memset(threadname, '\0', sizeof(threadname));
				strcat(threadname, "/proc/");
				strcat(threadname, pidchar);
				strcat(threadname, "/task");

				// printf("%s\n", threadname);

				DIR *threaddir;
				struct dirent *threadptr;
				char tpid[20];
				int tid;
				char sLine[50];
				char tName[50];
				char tComm[52];

				threaddir = opendir(threadname);
				if (threaddir == NULL) {
					perror(threadname);
					exit(1);
				}

				while ((threadptr = readdir(threaddir)) !=
				       NULL) {
					tid = atoi(threadptr->d_name);
					if (tid && (tid != pid)) {
						sprintf(tpid, "%d", tid);
						//strcpy(tpid,threadptr->d_name);
						strcat(threadname, "/");
						strcat(threadname, tpid);
						strcat(threadname, "/stat");

						FILE *fpp =
							fopen(threadname, "r");
						if (fpp) {
							// printf("%s\n", threadname);

							fgets(sLine, 50, fpp);
							sscanf(sLine,
							       "%*[^(](%[^)]",
							       tName);
							//ppid of child thread is pid

							//new thread struct, and save into procPids[]
							procPids[proNum]
								.isThread = 1;
							sprintf(tComm, "{%s}",
								tName);
							strcpy(procPids[proNum]
								       .pidname,
							       tComm);
							// procPids[proNum].ppidnum = atoi(pid);
							// procPids[proNum++].pidnum = atoi(tpid);
							procPids[proNum]
								.ppidnum = pid;
							procPids[proNum++]
								.pidnum = tid;

							fclose(fpp);
						} else {
							printf("open file %s error!\n",
							       threadname);
							continue;
						}
						memset(threadname, '\0',
						       sizeof(threadname));
						strcat(threadname, "/proc/");
						strcat(threadname, pidchar);
						strcat(threadname, "/task");
					}
				}

				fclose(fp);

				// strcpy(procPids[proNum].pidname, pName);
				// procPids[proNum].ppidnum = ppid;
			} else {
				printf("open file %s error!\n", filename);
				continue;
			}

			// procPids[proNum].isThread = NULL;
			// procPids[proNum++].pidnum = atoi(pid);
			memset(filename, '\0', sizeof(filename));
		}
	}
	//printf("%d",procPids[0].pidnum);
}

int findPid(int ppid, char printedPid[])
{
	return 0;
}

void sortAlpha(int childlist[], int len)
{
	int i, j;
	int t;
	for (i = 0; i < len - 1; i++)
		for (j = i + 1; j < len; j++) {
			// int cmp = strcmp(procPids[*childlist[i]].pidname, procPids[*childlist[j]].pidname);
			// if(cmp > 0){
			//     t = *childlist[i];
			//     *childlist[i] = *childlist[j];
			//     *childlist[j] = t;
			// }
			int cmp = strcmp(procPids[childlist[i]].pidname,
					 procPids[childlist[j]].pidname);
			if (cmp > 0) {
				t = childlist[i];
				childlist[i] = childlist[j];
				childlist[j] = t;
			}
		}
}
char printedPid[500] = { 0 };
typedef struct formt {
	const char *one;
	const char *firChild;
	const char *bran;
	const char *vert;
	const char *last;
	const char *blank;
} FORMAT;

FORMAT form = { "───", "─┬─", " ├─", " │ ", " └─", "   " };

//int printIdx = 0;
int printedNum = 0;
//int indent = 0;

void printTree(int printIdx, int indent)
{
	// printf("1\n");
	printf("%s\n", procPids[printIdx].pidname);
	printf("2\n");
	indent += strlen(procPids[printIdx].pidname);
	int hasChild = 0; //child number
	int childIdx[50]; //all child process index
	int k = 0;
	for (int i = 0; i < 500; i++) {
		if (procPids[i].ppidnum == procPids[printIdx].pidnum) {
			hasChild += 1;
			childIdx[k] = i;
		}
	}
	printf("%d\n", hasChild);
	sortAlpha(childIdx, hasChild);
	// for(int j = 0; j < hasChild; j++){
	//   printTree(childIdx[j],indent);
	// }
	printf("%d %d", childIdx[0], childIdx[1]);
}

int main(int argc, char **argv)
{
	int c;
	int digit_optind = 0;

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{ "version", no_argument, 0, 0 },
			{ "ascii", no_argument, 0, 0 },
			{ "unicode", no_argument, 0, 0 },
			{ "long", no_argument, 0, 0 },
			{ "vt100", no_argument, 0, 0 },
			{ "show-pids", no_argument, 0, 0 },
			{ 0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, "VAUlGp", long_options,
				&option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			switch (option_index) {
			case 0:
				printf("pstree (PSmisc) 23.4\n");
				printf("Copyright (C) 1993-2020 Werner Almesberger and Craig Small\n\n");

				printf("PSmisc comes with ABSOLUTELY NO WARRANTY.\n");
				printf("This is free software, and you are welcome to redistribute it under\n");
				printf("the terms of the GNU General Public License.\n");
				printf("For more information about these matters, see the files named COPYING.\n");
				break;
			case 1: //ascii
				readPsInfo();
				printTree(0, 0);
				break;
			case 2: //unicode
				break;
			case 3: //long
				break;
			case 4: //vt100
				break;
			case 5: //show-pids
				break;
			}
			break;

		case 'A':
			readPsInfo();
			printTree(0, 0);
			break;

		case 'V':
			printf("pstree (PSmisc) 23.4\n");
			printf("Copyright (C) 1993-2020 Werner Almesberger and Craig Small\n\n");

			printf("PSmisc comes with ABSOLUTELY NO WARRANTY.\n");
			printf("This is free software, and you are welcome to redistribute it under\n");
			printf("the terms of the GNU General Public License.\n");
			printf("For more information about these matters, see the files named COPYING.\n");
			break;

		case 'U':
			printf("option unicode\n");
			break;

		case 'l':
			printf("option long\n");
			break;

		case 'G':
			printf("option vt100\n");
			break;

		case 'p':
			printf("option show-pids\n");
			break;

		case '?':
			break;

		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc) {
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}

	exit(EXIT_SUCCESS);
}

// int main()
// {
// 	readPsInfo();
// 	printTree(0, 0);
// 	return 0;
// }