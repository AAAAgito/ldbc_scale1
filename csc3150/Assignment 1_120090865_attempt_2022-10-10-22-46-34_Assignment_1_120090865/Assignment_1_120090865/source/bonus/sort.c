#include "basic.h"
#include "sort.h"
#include <stdlib.h>

int smaller(char *a, char *b)
{
	int len_a = strlen(a), len_b = strlen(b);
	int len = MIN(len_a, len_b), i = 0;
	while (i < len) {
		if (a[i] > b[i]) {
			return false;
		}
		if (b[i] > a[i]) {
			return true;
		}
		++i;
	}
	return len_a < len_b;
}

int larger(char *a, char *b)
{
	int len_a = strlen(a), len_b = strlen(b);
	int len = MIN(len_a, len_b), i = 0;
	while (i < len) {
		if (a[i] > b[i]) {
			return true;
		}
		if (b[i] > a[i]) {
			return false;
		}
		++i;
	}
	return len_a > len_b;
}

void swap_son(Process *ptr, int i, int j)
{
	Process *temp = malloc(sizeof(Process));
	SWAP(ptr->son[i], ptr->son[j], temp);
	free(temp);
}

void quick_sort_normal(Process *ptr, int l, int r)
{
	if (l >= r) {
		return;
	}

	int i = l - 1, j = r + 1;
	char *x = ptr->son[l + r >> 1]->name;
	while (i < j) {
		do {
			i++;
		} while (smaller(ptr->son[i]->name, x));
		do {
			j--;
		} while (larger(ptr->son[j]->name, x));
		if (i < j) {
			swap_son(ptr, i, j);
		}
	}
	quick_sort_normal(ptr, l, j);
	quick_sort_normal(ptr, j + 1, r);
}

void quick_sort_pid(Process *ptr, int l, int r)
{
	if (l >= r) {
		return;
	}

	int i = l - 1, j = r + 1;
	int x = ptr->son[l + r >> 1]->pid;
	while (i < j) {
		do {
			i++;
		} while ((ptr->son[i]->pid < x));
		do {
			j--;
		} while ((ptr->son[j]->pid > x));
		if (i < j) {
			swap_son(ptr, i, j);
		}
	}
	quick_sort_pid(ptr, l, j);
	quick_sort_pid(ptr, j + 1, r);
}