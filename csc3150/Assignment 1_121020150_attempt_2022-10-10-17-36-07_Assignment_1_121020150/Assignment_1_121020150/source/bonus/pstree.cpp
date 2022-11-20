/*121020150 Mang Qiuyang option: -U -A -p -c pid*/
#include <bits/stdc++.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
const int N = 100005;
vector<string> split(string s)
{
	string str = "";
	vector<string> res;
	for (int i = 0; i < (int)s.size(); i++) {
		if (s[i] == ' ' || s[i] == '\n' || s[i] < 32 || s[i] > 126) {
			if (str != "")
				res.push_back(str);
			str = "";
		} else
			str = str + s[i];
	}
	if (str != "")
		res.push_back(str);
	return res;
}
inline int string_to_int(string str)
{
	int res = 0;
	for (int i = 0; i < (int)str.size(); i++)
		res = res * 10 + str[i] - 48;
	return res;
}

struct Process {
	int pid, ppid, effective_uid, NStgid;
	string name, user_name, cmdline, exe;
} tmp_process, node[N];

vector<int> edge[N], edge2[N];

inline Process *read_process(int pid)
{
	static char Filename[1024], str[1024];
	Process *p = &tmp_process;
	struct passwd *pws;
	string line;

	sprintf(Filename, "/proc/%d/status", pid);
	ifstream file1(Filename);
	if (!file1.is_open())
		return NULL;

	while (getline(file1, line)) {
		vector<string> cur = split(line);
		if (!(int)cur.size())
			continue;
		assert((int)cur.size() > 0);
		if (cur[0] == "Pid:")
			p->pid = string_to_int(cur[1]);
		if (cur[0] == "Name:")
			p->name = cur[1];
		if (cur[0] == "PPid:")
			p->ppid = string_to_int(cur[1]);
		if (cur[0] == "Uid:") {
			p->effective_uid = string_to_int(cur[2]);
			pws = getpwuid(string_to_int(cur[1]));
			p->user_name = pws->pw_name;
		}
		if (cur[0] == "NStgid:")
			p->NStgid = string_to_int(cur[1]);
	}
	file1.close();
	sprintf(Filename, "/proc/%d/cmdline", pid);
	ifstream file2(Filename);
	if (!file2.is_open())
		return NULL;
	while (getline(file2, line)) {
		vector<string> cur = split(line);
		if ((int)cur.size())
			p->cmdline = cur[0];
	}
	file2.close();

	sprintf(Filename, "/proc/%d/exe", pid);
	readlink(Filename, str, 128);
	p->exe = line;
	return p;
}
map<char, int> COM;
int pid_exist[N], PID = 0;
int is_down[N];
inline void dfs(int u, int depth)
{
	cout << node[u].name;
	if ((int)edge2[u].size())
		cout << (COM['A'] ? "-" : "─");
	else
		cout << "\n";
	int new_depth = depth + node[u].name.size() + 1;
	if ((int)edge2[u].size()) {
		if ((int)edge2[u].size() > 1)
			cout << (COM['A'] ? "+-" : "┬─"),
				is_down[new_depth] = 1;
		else
			cout << (COM['A'] ? "--" : "──");
		dfs(edge2[u][0], new_depth + 2);
	}
	for (int i = 1; i < (int)edge2[u].size(); i++) {
		for (int i = 0; i < new_depth; i++)
			if (is_down[i])
				cout << (COM['A'] ? "|" : "│");
			else
				cout << " ";
		if (i == (int)edge2[u].size() - 1)
			cout << (COM['A'] ? "`-" : "└─"),
				is_down[new_depth] = 0;
		else
			cout << (COM['A'] ? "|-" : "├─");
		dfs(edge2[u][i], new_depth + 2);
	}
	is_down[new_depth] = 0;
}
int tot[N];
inline string dfs0(int u)
{
	map<string, pair<int, int> > mp;
	int count = 0;
	for (auto v : edge[u]) {
		string cur = dfs0(v);
		if (cur == "") {
			count = 2;
			edge2[u].push_back(v);
			continue;
		}
		if (!mp.count(cur)) {
			mp[cur].first = v;
			mp[cur].second = 1;
			edge2[u].push_back(v);
			count++;
			continue;
		} else
			mp[cur].second++;
	}
	if (count > 1) {
		for (auto x : mp) {
			int v = x.second.first;
			tot[v] = x.second.second;
			if (tot[v] > 1)
				node[v].name = to_string(tot[v]) + "*[" +
					       node[v].name + "]";
		}
		return "";
	}
	edge2[u].clear();
	for (auto x : mp)
		tot[x.second.first] = x.second.second;
	for (auto x : mp)
		if (tot[x.second.first] == 1) {
			node[u].name =
				node[u].name + "──" + node[x.second.first].name;
		} else {
			node[u].name = node[u].name + "──" +
				       to_string(tot[x.second.first]) + "*[" +
				       node[x.second.first].name + "]";
		}
	return node[u].name;
}

inline void solve_command(char *argv)
{
	int len = strlen(argv);
	if (len == 2 && argv[0] == '-')
		return (void)(COM[argv[1]] = 1);
	for (int i = 0; i < len; i++)
		if (!isdigit(argv[i]))
			return;
		else
			PID = PID * 10 + argv[i] - 48;
}
int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++)
		solve_command(argv[i]);
	if (COM['U'])
		COM['A'] = 0;
	for (int i = 1; i <= 0x7fff; i++) {
		Process *p = read_process(i);
		if (!p)
			continue;
		pid_exist[i] = 1;
		node[i] = tmp_process;
		if (node[i].ppid > 0) {
			if (node[i].NStgid == node[i].pid)
				edge[node[i].ppid].push_back(i);
			else {
				edge[node[i].NStgid].push_back(i);
				node[i].name = "{" + node[i].name + "}";
			}
		}
		if (COM['p'])
			node[i].name = node[i].name + "(" +
				       to_string(node[i].pid) + ")";
	}
	if (!COM['c']) {
		if (!PID)
			dfs0(1);
		else if (pid_exist[PID])
			dfs0(PID);
	} else {
		for (int i = 1; i <= 0x7fff; i++)
			edge2[i] = edge[i];
	}
	if (!PID)
		dfs(1, 0);
	else if (pid_exist[PID])
		dfs(PID, 0);
	return 0;
}