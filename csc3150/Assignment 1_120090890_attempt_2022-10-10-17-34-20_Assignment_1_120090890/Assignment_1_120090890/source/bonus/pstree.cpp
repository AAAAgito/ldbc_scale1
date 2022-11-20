#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <dirent.h>
using namespace std;

struct T_h {
	string name;
	int pid;
	int ppid;
	list<T_h *> ch;
};

typedef struct T_h T;

int total = 0; //the total number of thread
T t_list[10000]; //the array to store thread

int threadfilter(const struct dirent *dir)
{
	const char *s = dir->d_name;
	for (int j = 0; s[j] != '\0'; j++) {
		if (s[j] < '0')
			return 0;
		else if (s[j] > '9') {
			return 0;
		}
	}
	return 1;
}

T *fPid(int p)
{
	int j;
	for (j = 0; j < total; j++) {
		if (t_list[j].pid == p)
			return &t_list[j];
	}
	return NULL;
}

void getThread(T &t, string path)
{
	string prep = "/proc/";
	string flop = "/status";
	string absp = (prep + path + flop);
	ifstream f(absp.c_str());
	string str;
	while (f >> str) {
		if (str == "PPid:")
			f >> t.ppid;
		else if (str == "Name:")
			f >> t.name;
		else if (str == "Pid:")
			f >> t.pid;
	}
	f.close();
}

void pTree(T *s, int *pre = NULL, int prenums = 0, int offset = 0)
{
	cout << s->name;

	if (s->ch.size() != 0 && s->ch.size() != 1) {
		for (list<T *>::iterator node = s->ch.begin();
		     node != s->ch.end(); node++) {
			int *q = new int[prenums + 1];
			int p;
			for (p = 0; p < prenums; p++) {
				q[p] = pre[p];
			}
			q[prenums] = s->name.size() + offset + 1;
			int ind1 = 0;
			if (*node != s->ch.back() && *node != s->ch.front()) {
				for (int i = 0; i < s->name.size() + offset;
				     i++) {
					if (ind1 < prenums && i == pre[ind1]) {
						printf("│");
						ind1++;

					} else {
						printf(" ");
					}
				}
				printf(" ├─");
				pTree(*node, q, prenums + 1,
				      s->name.size() + offset + 3);
			} else if (*node == s->ch.front()) {
				printf("─┬─");
				pTree(*node, q, prenums + 1,
				      s->name.size() + offset + 3);
			} else {
				int ind2 = 0;
				for (int i = 0; i < offset + s->name.size();
				     i++) {
					if (ind2 < prenums && i == pre[ind2]) {
						printf("│");
						ind2++;
					} else {
						printf(" ");
					}
				}
				printf(" └─");
				pTree(*node, pre, prenums,
				      s->name.size() + offset + 3);
			}
			if (q != NULL) {
				delete[] q;
				q = NULL;
			}
		}
	} else if (s->ch.size() == 1) {
		printf("───");
		pTree(s->ch.front(), pre, prenums, s->name.size() + offset + 3);
	} else {
		printf("\n");
	}
}

int main()
{
	struct dirent **dir;
	total = scandir("/proc", &dir, threadfilter, alphasort);
	if (total >= 0) {
		int m, i;
		for (m = 0; m < total; m++) {
			getThread(t_list[m], dir[m]->d_name);
		}
		for (i = 0; i < total; i++) {
			if (t_list[i].ppid > 0) {
				(fPid(t_list[i].ppid)->ch).push_back(&t_list[i]);
			}
		}
	}
	pTree(fPid(1));
	return 0;
}