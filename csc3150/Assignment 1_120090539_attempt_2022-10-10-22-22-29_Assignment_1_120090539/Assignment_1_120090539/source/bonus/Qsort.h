#ifndef _QSORT_H_
#define _QSORT_H_

#include "DuLinkList.h"

void swap(int *a, int *b);

int partition(DuLinkList_ArrNode *arr, int left, int right);

void _Qsort(DuLinkList_ArrNode *arr, int left, int right);

void Qsort(DuLinkList *list, int size);

#endif // _QSORT_H_
