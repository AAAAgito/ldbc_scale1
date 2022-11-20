#pragma once
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <pwd.h>

#define NAME_LEN 40
typedef struct _PsTreeNode {
	pid_t pid;
	pid_t ppid;
	int uid;
	int gid;
	char name[40];
	struct _DuLinkList childsList;
} PsTreeNode;

typedef struct _HelperNode {
	//The index of the array==pid
	int flag_activated;
	pid_t ppid; //parent pid
	int uid;
	int gid;
	char name[NAME_LEN];
	DuLNode *heapPointer;
} HelperNode;

#endif // _MAIN_H_
