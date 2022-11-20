#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
struct Process {
	int pid;
	string name;
	int ppid;
	list<Process *> children;
};
typedef struct Process Pr;

const int MAX_NUM = 10000;
Pr p_array[MAX_NUM];
int count = 0; //the total number of process

void getAProcess(Pr &p, string path)
{
	string prepath("/proc/");
	string flopath("/status");
	string abspath(prepath + path + flopath);
	ifstream finpath(abspath.c_str());
	string temp;
	while (finpath >> temp) {
		if (temp == "Name:")
			finpath >> p.name;
		else if (temp == "Pid:")
			finpath >> p.pid;
		else if (temp == "PPid:")
			finpath >> p.ppid;
	}
	finpath.close();
}

int processfilter(const struct dirent *dir)
{
	const char *s = dir->d_name;
	for (int i = 0; s[i] != '\0'; i++) {
		if (s[i] < '0' || s[i] > '9')
			return 0;
	}
	return 1;
}

void read()
{
	struct dirent **eps;
	count = scandir("/proc", &eps, processfilter, alphasort);
	if (count >= 0) {
		for (int i = 0; i < count; i++) {
			getAProcess(p_array[i], eps[i]->d_name);
		}
	}
}

Pr *findByPid(int pid)
{
	for (int i = 0; i < count; i++) {
		if (p_array[i].pid == pid)
			return &p_array[i];
	}
	return NULL;
}

void setChild()
{
	for (int i = 0; i < count; i++) {
		if (p_array[i].ppid > 0) {
			(findByPid(p_array[i].ppid)->children)
				.push_back(
					&p_array[i]); // 给Pr的children数组添加children
		}
	}
}

void printTree(Pr *start, int *pre = NULL, int prenum = 0, int offset = 0)
{
	cout << start->name;
	if (start->children.size() == 1) { // children为1的情况
		cout << "───";
		printTree(start->children.front(), pre, prenum,
			  offset + start->name.length() + 3);
	} else if (start->children.size() != 0) { // children 大于 0
		for (list<Pr *>::iterator it = start->children.begin();
		     it != start->children.end(); it++) {
			int *t = new int[prenum + 1];
			for (int i = 0; i < prenum; i++)
				t[i] = pre[i];
			t[prenum] = offset + start->name.length() + 1;
			if (*it ==
			    start->children.front()) { //如果是第一个children
				cout << "─┬─";
				printTree(*it, t, prenum + 1,
					  offset + start->name.length() + 3);
			} else if (*it !=
				   start->children.back()) { //中间的children
				int index = 0;
				for (int i = 0;
				     i < offset + start->name.length(); i++) {
					if (index < prenum && i == pre[index]) {
						cout << "│";
						index++;
					} else
						cout << ' ';
				}
				cout << " ├─";
				printTree(*it, t, prenum + 1,
					  offset + start->name.length() + 3);
			} else { //最后的children
				int index = 0;
				for (int i = 0;
				     i < offset + start->name.length(); i++) {
					if (index < prenum && i == pre[index]) {
						cout << "│";
						index++;
					} else
						cout << ' ';
				}
				cout << " └─";
				printTree(*it, pre, prenum,
					  offset + start->name.length() + 3);
			}
			if (t != NULL) {
				delete[] t;
				t = NULL;
			}
		}
	} else { // children数量等于0
		cout << endl;
	}
}

void printTree_p(Pr *start, int *pre = NULL, int prenum = 0, int offset = 0)
{
	ostringstream os;
	os << start->pid;
	int int_len = os.str().size() + 2;
	cout << start->name << "(" << start->pid << ")";
	if (start->children.size() == 1) { // children为1的情况
		cout << "───";
		printTree_p(start->children.front(), pre, prenum,
			    offset + start->name.length() + int_len + 3);
	} else if (start->children.size() != 0) { // children 大于 0
		for (list<Pr *>::iterator it = start->children.begin();
		     it != start->children.end(); it++) {
			int *t = new int[prenum + 1];
			for (int i = 0; i < prenum; i++)
				t[i] = pre[i];
			t[prenum] = offset + start->name.length() + int_len + 1;
			if (*it ==
			    start->children.front()) { //如果是第一个children
				cout << "─┬─";
				printTree_p(*it, t, prenum + 1,
					    offset + start->name.length() +
						    int_len + 3);
			} else if (*it !=
				   start->children.back()) { //中间的children
				int index = 0;
				for (int i = 0;
				     i <
				     offset + start->name.length() + int_len;
				     i++) {
					if (index < prenum && i == pre[index]) {
						cout << "│";
						index++;
					} else
						cout << ' ';
				}
				cout << " ├─";
				printTree_p(*it, t, prenum + 1,
					    offset + start->name.length() +
						    int_len + 3);
			} else { //最后的children
				int index = 0;
				for (int i = 0;
				     i <
				     offset + start->name.length() + int_len;
				     i++) {
					if (index < prenum && i == pre[index]) {
						cout << "│";
						index++;
					} else
						cout << ' ';
				}
				cout << " └─";
				printTree_p(*it, pre, prenum,
					    offset + start->name.length() +
						    int_len + 3);
			}
			if (t != NULL) {
				delete[] t;
				t = NULL;
			}
		}
	} else { // children数量等于0
		cout << endl;
	}
}

int main(int argc, char *argv[])
{
	read();
	setChild();
	if (argc <= 1) {
		printTree(findByPid(1));
	} else if (argc > 1) {
		printTree_p(findByPid(1));
	} 
	return 0;
}
