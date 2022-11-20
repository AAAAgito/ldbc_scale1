#include "basic.h"
#include "get.h"
#include "sort.h"
#include <stdio.h>

char screen[512] = "";
int stack[512];
int head = 0;
int isroot = true;

void showRoot(Process *ptr)
{
	switch (output_pid) {
	case true:
		sprintf(tmp, "%s(%d)", ptr->name, ptr->pid);
		break;

	default:
		sprintf(tmp, "%s", ptr->name);
		break;
	}
	stack[++head] = strlen(tmp) + 1;
	isroot = false;
}

void zshow(Process *ptr)
{
	switch (output_pid) {
	case true:
		sprintf(tmp, "-%s(%d)", ptr->name, ptr->pid);
		break;

	default:
		sprintf(tmp, "-%s", ptr->name);
		break;
	}
	stack[head + 1] = stack[head] + strlen(tmp) + 1;
	head++;
}

void sshow(Process *ptr)
{
	if (output_pid)
		sprintf(tmp, "%s-%s(%d)", screen, ptr->name, ptr->pid);
	else if (no_simple) {
		sprintf(tmp, "%s-%s", screen, ptr->name);
	} else {
		sprintf(tmp, "%s-%s", screen, ptr->name);
	}
	stack[++head] = strlen(tmp) + 1;
}

void showLine(Process *ptr, int type)
{
	int i = stack[head - 1];
	if (type == -1) {
		screen[i - 1] = ' ';
	}
	while (i < stack[head]) {
		screen[i++] = ' ';
	}
	screen[stack[head]] = '|';

	int muti = ptr->nson + ptr->nthr;

	if (muti == 0) {
		printf("\n");
	} else if (muti == 1) {
		printf("--");
		screen[stack[head]] = ' ';
	} else {
		if (is_ascci) {
			printf("-+");
		} else {
			printf("--");
		}
	}

	screen[++stack[head]] = '\0';
}

void sortProcess(Process *ptr)
{
	if (sort_by_pid) {
		quick_sort_pid(ptr, 0, ptr->nson - 1);
	} else {
		quick_sort_normal(ptr, 0, ptr->nson - 1);
	}
}

void buildTree(Process *ptr, int type, int isproc);
void dfs(Process *ptr)
{
	int i = 0, ith = 0;
	while (i < ptr->nson) {
		ith = i;
		int is_build_son =
			(ptr->nson > 1 && ptr->nthr == 0 && ptr->nson - 1 == i);
		switch (is_build_son) {
		case true:
			ith = -1;

		default:
			buildTree(ptr->son[i++], ith, true);
			break;
		}
	}
	int flag = (output_pid || no_simple || highlight);
	int j = 0;
	switch (flag) {
	case true:
		while (j < ptr->nthr) {
			int ith = j + ptr->nson;
			int is_build_thr = (ptr->nson + ptr->nthr > 1 &&
					    ptr->nthr - 1 + ptr->nson == ith);
			switch (is_build_thr) {
			case true:
				ith = -1;
			default:
				buildTree(ptr->thr[j++], ith, false);
				break;
			}
		}
		break;

	default:
		if (ptr->nthr > 0) {
			int is_list = ptr->thr[0]->pid - 1;
			if (is_list) {
				int num = ptr->thr[0]->pid;
				sprintf(tmp, "%d*[%s]", num, ptr->thr[0]->name);
				strcpy(ptr->thr[0]->name, tmp);
			}
			int ith = 0;
			if (ptr->nson != 0) {
				ith = -1;
			}
			buildTree(ptr->thr[0], ith, false);
		}
		break;
	}
}

void buildTree(Process *ptr, int type, int isproc)
{
	if (isroot) {
		showRoot(ptr);
	} else {
		if (type == 0) { // first of one subtree
			zshow(ptr);
		} else if (type == -1) {
			if (is_ascci) {
				screen[stack[head] - 1] = '`';
			} else {
				screen[stack[head] - 1] = '|';
			}
			sshow(ptr);
		} else {
			sshow(ptr);
		}
	}

	printf("%s", tmp);
	showLine(ptr, type);

	if (isproc) {
		sortProcess(ptr);
		sortProcess(ptr);
		dfs(ptr);
	}
	head--;
	screen[stack[head]] = '\0';
}