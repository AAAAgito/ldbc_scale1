#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>

const static std::string PATH = "/proc";

struct Process_info {
	/* data */
	std::string name;
	__pid_t pid;
	__pid_t ppid;
};

int pid_count = 0;
std::vector<Process_info> process_infos;

struct P_node {
	__pid_t pid;
	std::string name;
	int count = 1;
	std::vector<P_node> children;
};

enum Mode {
	DEFAULT,
	SHOW_PID,
	DISABLE_COMPACTION,
	SORT_BY_PID,
	DISPLAY_VERSION
};

__pid_t get_ppid_name(__pid_t pid, std::string path, std::string &name)
{
	std::string pid_str = std::to_string(pid);
	std::string process_stat_path = path;
	std::ifstream stat_file;
	process_stat_path = process_stat_path + "/" + pid_str + "/stat";
	stat_file.open(process_stat_path, std::ios::in);
	if (stat_file.is_open()) {
		char buf[256] = { 0 };
		stat_file.getline(buf, sizeof(buf));
		std::string info = buf;
		name = info.substr(info.find("(") + 1,
				   info.find(")") - info.find("(") - 1);
		int start_pos = info.find(")") + 4;
		int length = info.size() - start_pos;
		std::string ppid = info.substr(start_pos, length);
		ppid = ppid.substr(0, ppid.find(" "));
		int int_ppid = atoi(ppid.c_str());
		stat_file.close();
		return int_ppid;
	} else {
		printf("ERROR: fail to open file");
		exit(1);
	}
}

void get_process_info()
{
	DIR *pid_dir = opendir(PATH.c_str());
	int pid;

	struct dirent *dir_item;
	while ((dir_item = readdir(pid_dir)) != NULL) {
		pid = atoi(dir_item->d_name);
		if (pid == 0) {
			continue;
		}
		Process_info pid_info;
		pid_info.pid = pid;
		pid_info.ppid = get_ppid_name(pid, PATH, pid_info.name);
		process_infos.push_back(pid_info);
		pid_count++;
		// Similarly, Get the threads' information in
		// /proc/pid/task/thread_pid/stat.
		std::string task_path = PATH + "/" + dir_item->d_name + "/task";
		DIR *task_dir = opendir(task_path.c_str());

		struct dirent *task_item;
		while ((task_item = readdir(task_dir)) != NULL) {
			int task_pid = atoi(task_item->d_name);

			if (task_pid == pid || task_pid == 0)
				continue;
			Process_info thread_info;
			thread_info.pid = task_pid;
			thread_info.ppid = get_ppid_name(task_pid, task_path,
							 thread_info.name);
			thread_info.ppid = pid_info.pid;
			thread_info.name = "{" + thread_info.name + "}";
			process_infos.push_back(thread_info);
		}
	}
}

void build_tree(P_node &root)
{
	__pid_t pid = root.pid;
	for (int i = 0; i < process_infos.size(); i++) {
		Process_info p_info = process_infos[i];
		__pid_t parent_pid = p_info.ppid;
		if (parent_pid ==
		    pid) // Check whether the current node is the parent of
		// the one we searched.
		{
			// If it is, push it to the children vector of the current node.
			P_node child_p_node;
			child_p_node.pid = p_info.pid;
			child_p_node.name = p_info.name;
			root.children.push_back(child_p_node);
		}
	}
	if (root.children.size() != 0) // Has child process
	{
		for (int i = 0; i < root.children.size(); i++) {
			build_tree(root.children[i]);
		}
	}
}

void build_tree_with_pid(P_node &root)
{
	__pid_t pid = root.pid;
	root.name = root.name + "(" + std::to_string(root.pid) + ")";
	for (int i = 0; i < process_infos.size(); i++) {
		Process_info p_info = process_infos[i];
		__pid_t parent_pid = p_info.ppid;
		if (parent_pid ==
		    pid) // Check whether the current node is the parent of
		// the one we searched.
		{
			// If it is, push it to the children vector of the current node.
			P_node child_p_node;
			child_p_node.pid = p_info.pid;
			child_p_node.name = p_info.name;
			root.children.push_back(child_p_node);
		}
	}
	if (root.children.size() != 0) // Has child process
	{
		for (int i = 0; i < root.children.size(); i++) {
			build_tree_with_pid(root.children[i]);
		}
	}
}

void merge_tree(P_node &root)
{
	// This function is used to merge the processes who have the same name.
	for (int i = 0; i < root.children.size(); i++) {
		std::string name = root.children[i].name;
		for (int j = i + 1; j < root.children.size(); j++) {
			std::string sibling_name = root.children[j].name;
			if (sibling_name == name &&
			    root.children[j].children.empty() &&
			    root.children[i]
				    .children
				    .empty()) // Only merge those node with the same name and
			// in the meantime do not have children.
			{
				root.children[i]
					.count++; // increase the count as this node "eat" another.
				// Then remove the dead node.
				std::swap(*(std::begin(root.children) + j),
					  root.children.back());
				root.children.pop_back();
				// Note that as we swap the last process with the current process, we
				// need to substract j by one such that we don't miss the previous last
				// process.
				j--;
			}
		}
		merge_tree(root.children[i]);
	}
	// In order to make the display make more sense (as far as i'm concerned), I
	// swap the thread to the back of the vector.
	for (int i = 0; i < root.children.size(); i++) {
		std::string name = root.children[i].name;
		int swap_pos = root.children.size() - 1;

		if (name[0] == '{' && i != swap_pos) {
			while (root.children[swap_pos].name[0] == '{' &&
			       swap_pos > i) {
				swap_pos--;
			}
			std::swap(*(std::begin(root.children) + i),
				  *(std::begin(root.children) + swap_pos));
		}
	}
}

void sort_tree_pid(P_node &root)
{
	sort(root.children.begin(), root.children.end(),
	     [](P_node x, P_node y) { return x.pid < y.pid; });
	for (int i = 0; i < root.children.size(); i++) {
		sort_tree_pid(root.children[i]);
	}
}

void print_tree(P_node &root, std::string indent)
{
	int space = 2;
	if (root.pid == 1)
		space = 1;

	if (root.children.empty()) // Leaf Node
	{
		int count = root.count;
		if (count >
		    1) // If there's more than one process with the same name, use
		// num*[name] to display.
		{
			std::cout << count << "*[" << root.name << "]\n";
		} else
			std::cout << root.name << '\n';
	} else {
		std::cout << root.name;
		if (root.children.size() ==
		    1) // there's only one child of the current node.
		{
			for (int i = 0; i < root.name.size() + 3; i++) {
				indent += " ";
			}
			std::cout << "---";
			print_tree(root.children[0], indent);
		} else {
			std::string new_indent = indent;
			for (int i = 0; i < root.name.size() + space; i++) {
				new_indent += " ";
				indent += " ";
			}
			new_indent += "|";
			indent += " ";

			for (int i = 0; i < root.children.size(); i++) {
				if (i == 0) // The first node
				{
					if (i != root.children.size() - 2) {
						std::cout << "-+-";
						print_tree(root.children[i],
							   new_indent);
						std::cout << new_indent;
					} else // if the first node happens to be the second last node, we
					// dont print "|" in the next line.
					{
						std::cout << "-+-";
						print_tree(root.children[i],
							   new_indent);
						std::cout << new_indent.substr(
							0,
							new_indent.size() - 1);
					}
				} else if (i < root.children.size() - 2) {
					std::cout << "-";
					print_tree(root.children[i],
						   new_indent);
					std::cout << new_indent;
				} else if (i ==
					   root.children.size() -
						   2) // if it is the second last node,
				// we dont print "|" here
				{
					std::cout << "-";
					print_tree(root.children[i],
						   new_indent);
					std::cout << new_indent.substr(
						0, new_indent.size() - 1);
				} else {
					// if it's the last line, we don't print the indentation, because the
					// current branch of tree is at the end, as a result, the current
					// indentation will not be suitable for the next line. we pass indent
					// to print_tree because it's the last node of the current brach,
					// there's no need to cover another "|", as "|" indicates that there
					// will be another line right after the current node.
					std::cout
						<< "`-"; // " `-" here also indicates that it's the last
					// node of the current branch.
					print_tree(root.children[i], indent);
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	int opt;
	Mode mode = DEFAULT;

	while ((opt = getopt(argc, argv, "pcnV")) != -1) {
		switch (opt) {
		case 'p':
			mode = SHOW_PID;
			break;
		case 'c':
			mode = DISABLE_COMPACTION;
			break;
		case 'n':
			mode = SORT_BY_PID;
			break;
		case 'V':
			mode = DISPLAY_VERSION;
			break;
		default:
			exit(0);
		}
	}

	get_process_info();
	P_node root;
	root.pid = process_infos[0].pid;
	root.name = process_infos[0].name;
	std::string indent;

	if (mode == DEFAULT) {
		build_tree(root);
		merge_tree(root);
		print_tree(root, indent);
	} else if (mode == SHOW_PID) {
		build_tree_with_pid(root);
		print_tree(root, indent);
	} else if (mode == DISABLE_COMPACTION) {
		build_tree(root);
		print_tree(root, indent);
	} else if (mode == SORT_BY_PID) {
		build_tree(root);
		merge_tree(root);
		sort_tree_pid(root);
		print_tree(root, indent);
	} else if (mode == DISPLAY_VERSION) {
		std::cout
			<< "pstree (PSmisc) MY_VERSION\n"
			<< "Copyright (C) 1993-2009 Werner Almesberger and Craig Small\n\n"
			<< "PSmisc comes with ABSOLUTELY NO WARRANTY.\n"
			<< "This is free software, and you are welcome to redistribute it "
			   "under\n"
			<< "the terms of the GNU General Public License.\n"
			<< "For more information about these matters, see the files named "
			   "COPYING.\n";
	}
}