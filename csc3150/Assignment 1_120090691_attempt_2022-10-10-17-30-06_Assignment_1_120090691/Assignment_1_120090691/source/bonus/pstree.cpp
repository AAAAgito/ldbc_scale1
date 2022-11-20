#include <algorithm>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <map>
#include <vector>

using namespace std;
#define MAX_NAME_LEN 128
#define HASH_MOD 19260817

struct node {
	int pid; // PID of a process
	vector<node *> child; // Child process
	char name[MAX_NAME_LEN] = { '\0' };
	node *father = NULL; // Father of the current
	int hash_value = 0;
	bool is_link = true;
};

map<int, node> Nodelist; // Map pid -> node we define
vector<int> Placeholder; // Store the # of spaces in each hierachy
bool link_enabler[63]; // whether to enable '|' to link each level
bool is_ASCII; //print if to use ASCII to printout

inline bool cmp(node *a, node *b)
{
	int tmp = strcmp(a->name, b->name);
	if (tmp < 0)
		return true;
	else if (tmp == 0)
		return a->hash_value < b->hash_value;
	return false;
}

void add_node(node *new_node, int pid, char *name, int ppid)
{
	(*new_node).pid = pid;
	strcpy((*new_node).name, name);
	(*new_node).father = &Nodelist[ppid];
	new_node->father->child.push_back(new_node);
}

void print_spaces(vector<int> &Placeholder)
{
	for (int i = 0; i < Placeholder.size(); i++) {
		for (int j = 0; j < Placeholder[i]; j++)
			printf(" ");
		if (i != (Placeholder.size() - 1))
			if (link_enabler[i])
				(is_ASCII) ? printf(" | ") : printf(" │ ");
			else
				printf("   ");
	}
}

int char2hash(char *str)
{
	int hash_value = 0, len = 0;
	len = strlen(str);
	for (int i = 0; i < len; i++)
		hash_value = (hash_value + (int)(str[i]) * i) % HASH_MOD;
	return hash_value;
}
char *int2char(int x)
{
	static char str[MAX_NAME_LEN];
	int cnt = 0;
	while (x != 0) {
		str[cnt++] = x % 10 + '0';
		x /= 10;
	}
	char tmp = '\0';

	for (int i = cnt / 2 - 1; i >= 0; i--) {
		tmp = str[i];
		str[i] = str[cnt - i - 1];
		str[cnt - i - 1] = tmp;
	}

	return str;
}

int pre_hashmap(node &current, int depth)
{
	if (current.child.empty()) // leaf
		return current.hash_value =
			       depth * char2hash(current.name) % HASH_MOD;
	else if (current.child.size() == 1) // one child
	{
		current.is_link &= current.child[0]->is_link;
		return current.hash_value =
			       (depth * char2hash(current.name) +
				pre_hashmap(*(current.child[0]), depth + 1)) %
			       HASH_MOD;
	} else {
		current.is_link = false;
		int hash_value = depth * char2hash(current.name) % HASH_MOD;
		for (int i = 0; i < current.child.size(); i++)
			hash_value =
				(hash_value +
				 pre_hashmap(*(current.child[i]), depth + 1)) %
				HASH_MOD;
		return current.hash_value = hash_value;
	}
}

bool alter_output(int count, bool not_init)
{
	// count: how mant duplicate ones
	// not_init: if is the start of the duplicated process
	if (not_init) // not the start
		return true;
	else if (count == 1) // the start, but only one
		return false;
	printf("*%d[", count); // combine
	return true;
}

void print_out(node &current, vector<int> &Placeholder, bool is_leaf, int depth,
	       bool grouping = false)
{
	int new_placeholder = strlen(current.name); //# of spaces on this level
	Placeholder.push_back(new_placeholder);
	link_enabler[depth] = true;

	if (!is_leaf) // not leaf:
	{
		printf("%s", current.name);
		sort(current.child.begin(), current.child.end(), cmp);
		int child_size =
			current.child.size(); // Get child size of current node
		vector<pair<int, int> >
			unique_stat; // Count how many & which is next unique child
		unique_stat.clear();

		int cur = 0; // Mark where is the last unique child
		int cur_cnt = 1; // Count number of duplicated children
		// unique_stat.push_back(pair<int, int>(0, 0));

		for (int i = 1; i < child_size; i++) {
			if (current.child[i]->hash_value !=
				    current.child[i - 1]
					    ->hash_value || // new unique
			    (!current.child[i]->is_link)) // not link
			{
				unique_stat.push_back(
					pair<int, int>(cur, cur_cnt));
				cur = i;
				cur_cnt = 1;
			} else {
				cur++;
				cur_cnt++;
			}
		}
		unique_stat.push_back(pair<int, int>(
			cur, cur_cnt)); // push_back the remaining one

		if (unique_stat.size() > 1) // not single line output
		{
			vector<pair<int, int> >::iterator it =
				unique_stat.begin();
			(is_ASCII) ? printf("-+-") : printf("─┬─");
			print_out(*(current.child[0]), Placeholder,
				  current.child[0]->child.empty(), depth + 1,
				  alter_output(it->second, grouping));
			for (++it; (it + 1) != unique_stat.end(); it++) {
				print_spaces(Placeholder);
				(is_ASCII) ? printf(" |-") : printf(" ├─");
				print_out(
					*(current.child[it->first]),
					Placeholder,
					current.child[it->first]->child.empty(),
					depth + 1,
					alter_output(it->second, grouping));
			}
			link_enabler[depth] = false;
			print_spaces(Placeholder);
			(is_ASCII) ? printf(" `-") : printf(" └─");
			print_out(*(current.child[it->first]), Placeholder,
				  current.child[it->first]->child.empty(),
				  depth + 1,
				  alter_output(it->second, grouping));
		} else { // Is a link
			link_enabler[depth] = false;
			(is_ASCII) ? printf("---") : printf("───");
			print_out(*(current.child[0]), Placeholder,
				  current.child[0]->child.empty(), depth + 1,
				  alter_output(unique_stat.begin()->second,
					       grouping));
		}
	} else // is leaf
	{
		char tmp[MAX_NAME_LEN] = { '\0' }; // store (x??)
		if (grouping) // Multiple dupliacted processes
			printf("%s]\n", current.name);
		else
			printf("%s\n", current.name);
	}
	Placeholder.pop_back();
}
bool is_digit(char str[])
{
	int len = strlen(str);
	for (int i = 0; i < len; i++)
		if (str[i] > '9' || str[i] < '0')
			return false;
	return true;
}

void read_thread(node *current, int process_pid)
{
	DIR *d;
	struct dirent *file;
	char dir_path[MAX_NAME_LEN];
	sprintf(dir_path, "/proc/%d/task", process_pid);
	if (!(d = opendir(dir_path))) {
		perror("Error in opendir !!!/n");
		exit(-2);
	}

	char *pid_c =
		int2char(process_pid); // Convert process_pid(Int) to *Char
	while ((file = readdir(d)) != NULL) {
		char file_path[MAX_NAME_LEN];
		if (!strcmp(file->d_name, pid_c) ||
		    !is_digit(file->d_name)) // self or not number
			continue;
		// strcat(file_path,dir_path);
		sprintf(file_path, "%s/%s/stat", dir_path, file->d_name);

		FILE *f;
		f = fopen(file_path, "r");
		int pid;
		char name[MAX_NAME_LEN] = { '\0' };
		char unused[MAX_NAME_LEN] = { '\0' };
		fscanf(f, "%d %s %s", &pid, name,
		       unused); // get info from ~/stat

		node new_node;
		Nodelist.insert(pair<int, node>(pid, new_node));
		name[0] = '{';
		name[strlen(name) - 1] = '}';
		add_node(&Nodelist[pid], pid, name, process_pid); // add process
		fclose(f);
	}
}
int main(int argc, char *argv[])
{
	if (argc > 1 && !strcmp(argv[1], "-A"))
		is_ASCII = true;
	else
		is_ASCII = false;

	DIR *d;
	struct dirent *file;
	const char *dir_path = "/proc/";
	if (!(d = opendir(dir_path))) {
		perror("Error in opendir !!!/n");
		return -1;
	}

	node head;
	head.pid = 0;
	Nodelist.insert(pair<int, node>(0, head));

	while ((file = readdir(d)) != NULL) {
		char stat_path[MAX_NAME_LEN] = { '\0' };
		if (!is_digit(file->d_name)) // not-number
			continue;

		strcat(stat_path, dir_path);
		strcat(stat_path, file->d_name);
		strcat(stat_path, "/stat"); // combine path for the ~/stat file

		FILE *f;
		f = fopen(stat_path, "r");
		int pid, ppid;
		char name[MAX_NAME_LEN] = { '\0' };
		char unused[MAX_NAME_LEN] = { '\0' };
		fscanf(f, "%d %s %s %d", &pid, name, unused,
		       &ppid); // get info from ~/stat

		node new_node;
		Nodelist.insert(pair<int, node>(pid, new_node));
		char copied_name[MAX_NAME_LEN] = { '\0' };
		strncpy(copied_name, name + 1,
			strlen(name) - 2); // remove '(' & ')'
		// printf("%d: %s\n",pid,copied_name);
		add_node(&Nodelist[pid], pid, copied_name, ppid); // add process
		read_thread(&Nodelist[pid], pid);
		fclose(f);
	}
	closedir(d);
	pre_hashmap(Nodelist[1], 0);
	print_out(Nodelist[1], Placeholder, 0, false);
	return 0;
}
