#ifndef _GET_H
#define _GET_H

extern char *getname(char *pid_name, int pid);

extern void *get_name(char *pid_name, int pid, int *tempp);

extern char tmp[2048];
extern int count;
extern int tempp[2048];

extern void getfromdir(Process *ptr, int tid, int pid);

extern void scan(Process *ptr, char *taskdirname, int pid);

extern void gain(Process *ptr, int pid);

#endif