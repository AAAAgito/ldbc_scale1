#include "basic.h"
#include "get.h"
#include "stdio.h"
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>

char *getname(char *pid_name, int pid)
{
	char statname[512];
	int tmp[2048];
	sprintf(statname, stat_addr, pid);
	FILE *fp = fopen(statname, "r");
	fscanf(fp, "%d", tmp); // Get pid
	fscanf(fp, "%s", pid_name); // Get name
	if (pid_name[strlen(pid_name) - 1] != ')') {
		FILE *fn = fopen(statname, "r");
		fscanf(fn, "%d", tmp);
		fscanf(fn, "%[^)]", pid_name);
		fclose(fn);
		return pid_name + 2;
	}
	pid_name[strlen(pid_name) - 1] = '\0';
	fclose(fp);
	return pid_name + 1;
}

void *get_name(char *pid_name, int pid, int *tempp)
{
	char statname[512];
	int tmp[2048];
	sprintf(statname, stat_addr, pid);
	FILE *fp = fopen(statname, "r");
	fscanf(fp, "%d", tmp); // Get pid
	fscanf(fp, "%s", pid_name); // Get name
	char temp[2048];
	fscanf(fp, "%s", temp);
	for (int i = 0; i < 2; ++i) {
		fscanf(fp, "%d", &tempp[i]);
	}
}

char tmp[2048] = {};
int count = 1;
int tempp[2048] = {};

void getfromdir(Process *ptr, int tid, int pid)
{
	if (tid != pid) {
		if (output_pid || no_simple) {
			ptr->thr[ptr->nthr] = malloc(sizeof(Process));
			char pid_name[2048];
			sprintf(tmp, "{%s}", getname(pid_name, tid));
			Process *son_thr = ptr->thr[ptr->nthr];
			strcpy(son_thr->name, tmp);
			son_thr->pid = tid;
			son_thr->nthr = son_thr->nson = 0;
		}
		if (highlight) {
			ptr->thr[ptr->nthr] = malloc(sizeof(Process));
			char pid_name[2048];
			if (!strcmp(getname(pid_name, tid), "gmain") &&
			    count > 0) {
				count--;
				get_name(pid_name, tid, tempp);
				sprintf(tmp, "{gmain) S %d %d", tempp[0],
					tempp[1]);
			} else {
				sprintf(tmp, "{%s}", getname(pid_name, tid));
			}
			strcpy(ptr->thr[ptr->nthr]->name, tmp);
			ptr->thr[ptr->nthr]->nson = 0;
			ptr->thr[ptr->nthr]->pid = tid;
			ptr->thr[ptr->nthr]->nthr = 0;
		}
		++ptr->nthr;
	}
	if (NORMAL) {
		ptr->thr[0] = malloc(sizeof(Process));
		sprintf(tmp, "{%s}", ptr->name);
		Process *son_thread = ptr->thr[0];
		strcpy(son_thread->name, tmp);
		son_thread->pid = ptr->nthr;
		son_thread->nthr = son_thread->nson = 0;
	}
}

void scan(Process *ptr, char *taskdirname, int pid)
{
	DIR *dir;
	int tid = 0;
	struct dirent *ent;
	if ((dir = opendir(taskdirname))) {
		while ((ent = readdir(dir))) {
			if (isnumber(fileName)) {
				tid = atoi(fileName);
				getfromdir(ptr, tid, pid);
			}
		}
		closedir(dir);
	}
}

void getbasic(Process *ptr, FILE *fp)
{
	GETPID;
	char temp[2048];
	GETNAME;
	temp[strlen(temp) - 1] = '\0';
	strcpy(ptr->name, temp + 1);
	fscanf(fp, "%s", temp);
	GETPPID;
}

void gain(Process *ptr, int pid)
{
	char statname[fileSize] = {};
	sprintf(statname, stat_addr, pid);
	char taskdirname[fileSize] = {};
	sprintf(taskdirname, task_addr, pid);
	char childfile[fileSize] = {};
	sprintf(childfile, child_addr, pid, pid);

	FILE *fp = fopen(statname, "r");
	getbasic(ptr, fp);

	fp = fopen(childfile, "r");
	ptr->nson = 0;
	int ch;
	while (fileNotEnd) {
		ptr->son[ptr->nson] = malloc(sizeof(Process));
		gain(ptr->son[ptr->nson++], ch);
	}
	ptr->nthr = 0;
	scan(ptr, taskdirname, pid);
	fclose(fp);
}