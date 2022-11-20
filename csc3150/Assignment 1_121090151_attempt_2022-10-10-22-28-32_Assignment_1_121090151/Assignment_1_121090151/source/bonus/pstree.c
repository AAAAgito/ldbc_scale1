#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
	int Pid;
	int Ppid;
	int NStgid;
	char Name[128];
} proc;

proc g_proc;

static inline char *next_line(char *str)
{
	int i = 0;
	while (str[i] != '\n') {
		if (str[i] == 0) {
			return str + i;
		}
		i++;
	}
	i++;
	return str + i;
}
proc *get_proc(int pid)
{
	char filename[128];
	char buff[1024];
	char key[16];
	char val0[16], val1[16], val2[16], val3[16];
	int str_len;
	FILE *fp;
	int i, count;
	int ret;
	proc *p = &g_proc;
	struct passwd *pws;
	p->Name[0] = '\0';
	sprintf(filename, "/proc/%d/status", pid);
	fp = fopen(filename, "r");
	if (fp != NULL) {
		str_len = fread(buff, 1, 1024, fp);
		if (str_len > 0) {
			char *next, *curr;
			curr = buff;
			next = next_line(curr);
			count = 0;
			while (next != curr) {
				*(next - 1) = 0;
				ret = sscanf(curr, " %s %s %s %s %s ", key,
					     val0, val1, val2, val3);
				if (strcmp(key, "Pid:") == 0) {
					p->Pid = atoi(val0);
					count++;
				} else if (strcmp(key, "Name:") == 0) {
					strcpy(p->Name, val0);
					if (p->Name[0] == 't')
						;
					count++;
				} else if (strcmp(key, "PPid:") == 0) {
					p->Ppid = atoi(val0);
					count++;
				} else if (strcmp(key, "NStgid:") == 0) {
					p->NStgid = atoi(val0);
					count++;
				} else if (count >= 4) {
					break;
				}

				curr = next;
				next = next_line(curr);
			}
			fclose(fp);
		}
	} else
		return NULL;
	return p;
}
int flag_A, flag_p, flag_U, s = 1, flag_c;
typedef struct {
	int ppid;
	int num;
	int NStgid;
	char filename[128];
} node;
node tree[60005];
int id[60005], num = 0;
struct edge {
	int to, nxt;
} e[60005];
int sz[60005], head[60005], st[10], top;
int cnt = 0;
void add(int a, int b)
{
	e[++cnt].to = b;
	e[cnt].nxt = head[a];
	head[a] = cnt;
}
int ok[60005];
int son[60005];
void print_line()
{
	if (flag_A > flag_U)
		printf("-");
	else
		printf("─");
}
void print_down()
{
	if (flag_A > flag_U)
		printf("|");
	else
		printf("│");
}
void print_line_down()
{
	if (flag_A > flag_U)
		printf("+-");
	else
		printf("┬─");
}
void print_down_line()
{
	if (flag_A > flag_U)
		printf("`-");
	else
		printf("└─");
}
void print_cross()
{
	if (flag_A > flag_U)
		printf("|-");
	else
		printf("├─");
}
void dfs(int u, int length)
{
	for (int i = 0; tree[u].filename[i] != '\0'; i++) {
		if (tree[u].filename[i] != '$')
			printf("%c", tree[u].filename[i]);
		else
			print_line();
	}
	sz[u] = 1;
	if (head[u])
		print_line();
	else
		puts("");
	int cur_len = length + strlen(tree[u].filename) + 1;
	for (int i = head[u]; i; i = e[i].nxt) {
		int v = e[i].to;
		if (i == head[u]) {
			if (e[i].nxt)
				print_line_down();
			else
				print_line(), print_line();
			if (e[i].nxt)
				ok[cur_len] = 1;
			dfs(v, cur_len + 2);

		} else {
			for (int j = 0; j < cur_len; j++) {
				if (ok[j])
					print_down();
				else
					printf(" ");
			}
			if (e[i].nxt == 0)
				print_down_line(), ok[cur_len] = 0;
			else
				print_cross();
			dfs(v, cur_len + 2);
		}
		sz[u] += sz[v];
	}
	ok[cur_len] = 0;
}
int same(int i, int j)
{
	for (int k = 0; k < 128; k++) {
		if (tree[i].filename[k] == '\0' && tree[j].filename[k] == '\0')
			return 1;
		if (tree[i].filename[k] == '\0' && tree[j].filename[k] == '\0')
			return 0;
		if (tree[i].filename[k] != tree[j].filename[k])
			return 0;
	}
}
int is_line[60005];
int digit(int u)
{
	int res = 1;
	while (u / 10)
		res++, u /= 10;
	return res;
}
void pre_dfs(int u)
{
	is_line[u] = 1;
	for (int i = head[u]; i; i = e[i].nxt) {
		int v = e[i].to;
		pre_dfs(v);
	}
	for (int i = head[u]; i; i = e[i].nxt) {
		int v = e[i].to;
		if (is_line[v]) {
			int count = 1, pre = i;
			for (int j = e[i].nxt; j; j = e[j].nxt) {
				int vv = e[j].to;
				if (!is_line[vv]) {
					pre = j;
					continue;
				}
				if (same(v, vv))
					count++, e[pre].nxt = e[j].nxt;
				else
					pre = j;
			}
			if (count != 1) {
				int step = digit(count) + 2;
				for (int j = 127 - step; j >= 0; j--)
					tree[v].filename[j + step] =
						tree[v].filename[j];
				int pos = step;
				while (tree[v].filename[pos] != '\0')
					pos++;
				tree[v].filename[pos] = ']';
				tree[v].filename[pos + 1] = '\0';
				tree[v].filename[--step] = '[';
				tree[v].filename[--step] = '*';
				while (step) {
					tree[v].filename[--step] =
						(char)('0' + count % 10);
					count /= 10;
				}
			}
		}
	}
	int i = head[u];
	if (i == 0)
		return;
	if (e[i].nxt != 0 || !is_line[e[i].to])
		is_line[u] = 0;
	else {
		int pos = 0, v = e[i].to;
		while (tree[u].filename[pos] != '\0')
			pos++;
		for (int j = 0; j <= 2; j++)
			tree[u].filename[pos++] = '$';
		for (int posv = 0; tree[v].filename[posv] != '\0'; posv++) {
			tree[u].filename[pos + posv] = tree[v].filename[posv];
			if (tree[v].filename[posv + 1] == '\0')
				tree[u].filename[pos + posv + 1] = '\0';
		}
		head[u] = 0;
	}
}
void check(char *argv)
{
	int n = strlen(argv);
	if (n == 2) {
		if (argv[1] == 'A')
			flag_A = 1;
		if (argv[1] == 'p')
			flag_p = 1;
		if (argv[1] == 'U')
			flag_U = 1;
		if (argv[1] == 'c')
			flag_c = 1;
	}
	for (int i = 0; i < n; i++) {
		if (argv[i] < '0' || argv[i] > '9')
			return;
	}
	s = 0;
	for (int i = 0; i < n; i++)
		s = s * 10 + argv[i] - '0';
}
void main(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		check(argv[i]);
	}
	int i = 0;
	int ret;
	proc *p;
	char buff[2048];
	while (i <= 0x7fff) {
		p = get_proc(++i);
		if (p) {
			int cur_pid = g_proc.Pid;
			id[++num] = cur_pid;
			tree[g_proc.Pid].ppid = g_proc.Ppid;
			tree[g_proc.Pid].NStgid = g_proc.NStgid;
			for (int j = 0; j < 128; j++)
				tree[cur_pid].filename[j] = g_proc.Name[j];
			tree[cur_pid].num = 1;
		}
	}
	for (int i = 1; i <= num; i++) {
		int cur_id = id[i];
		if (tree[cur_id].NStgid == cur_id)
			add(tree[cur_id].ppid, cur_id);
		else {
			for (int j = 126; j >= 0; j--)
				tree[cur_id].filename[j + 1] =
					tree[cur_id].filename[j];
			int pos = 1;
			while (tree[cur_id].filename[pos] != '\0')
				pos++;
			tree[cur_id].filename[pos] = '}';
			tree[cur_id].filename[pos + 1] = '\0';
			tree[cur_id].filename[0] = '{';
			add(tree[cur_id].NStgid, cur_id);
		}
		if (flag_p) {
			int pos = 0;
			while (tree[cur_id].filename[pos] != '\0')
				pos++;
			tree[cur_id].filename[pos++] = '(';
			int tmp = cur_id;
			top = 0;
			while (tmp) {
				st[++top] = tmp % 10;
				tmp /= 10;
			}
			while (top)
				tree[cur_id].filename[pos++] =
					(char)('0' + st[top--]);
			tree[cur_id].filename[pos++] = ')';
			tree[cur_id].filename[pos] = '\0';
		}
	}
	if (flag_c == 0)
		pre_dfs(s);
	dfs(s, 0);
	return;
}