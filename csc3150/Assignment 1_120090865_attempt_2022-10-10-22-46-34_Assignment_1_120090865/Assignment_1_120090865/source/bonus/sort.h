#ifndef _SORT_H
#define _SORT_H

extern int smaller(char *a, char *b);

extern int larger(char *a, char *b);

extern void swap_son(Process *ptr, int i, int j);

extern void quick_sort_normal(Process *ptr, int l, int r);

extern void quick_sort_pid(Process *ptr, int l, int r);

#endif