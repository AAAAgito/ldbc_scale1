#ifndef _BUILD_H
#define _BUILD_H

extern char screen[512];
extern int stack[512];
extern int head;
extern int isroot;

extern void showRoot(Process *ptr);

extern void sshow(Process *ptr);

extern void sshow(Process *ptr);

extern void showLine(Process *ptr, int type);

extern void sortProcess(Process *ptr);

extern void dfs(Process *ptr);

extern void buildTree(Process *ptr, int type, int isproc);

#endif