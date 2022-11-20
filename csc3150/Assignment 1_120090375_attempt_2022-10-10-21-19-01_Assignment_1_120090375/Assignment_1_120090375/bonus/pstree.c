#include <dirent.h>
#include <stdio.h>
#include <string.h>

static int isdigitstr(char *str);
char *itoa(int num, char *str, int radix);
int power_op(int a, int b);
int atoi(char str[]);
int traverse_numerical_dir(char target_dir[], int r[], int stat[]);
char *read_name(char *str, char *target);
int is_inside(int stat[], int pid);

struct tree_node {
	char name[255];
	int pid;
	struct tree_node *parent;
	struct tree_node *children[100];
};

void construct_tree(struct tree_node node, int stat[])
{
	char path[100] = "/proc/";
	char temp[100];
	// strcat(path,itoa(node.pid,temp,10));
	strcat(path, "status");
	read_name(path, node.name);
}

int main(void)
{
	char t[10] = "/proc";
	int result[10000];
	int stat[10000];
	traverse_numerical_dir(t, result, stat);
	return 0;
}

///////////////////////////////////////////
//            Implementations            //
//            of    functions            //
///////////////////////////////////////////

static int isdigitstr(char *str)
{
	return (strspn(str, "0123456789") == strlen(str));
}

int traverse_numerical_dir(char target_dir[], int r[], int stat[])
{
	DIR *d;
	struct dirent *dir;
	d = opendir(target_dir);
	int count = 0;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (isdigitstr(dir->d_name)) {
				r[count] = atoi(dir->d_name);
				printf("We have process %d now\n", r[count]);
				stat[count] = 1;
				count += 1;
			}
		}
		closedir(d);
	}
	return count;
}

int power_op(int x, int n)
{
	int res = 1;
	while (n > 0) {
		if (n % 2 == 1) {
			res *= x;
		}
		x *= x;
		n >>= 1;
	}
	return res;
}

int atoi(char num[])
{
	int length = strlen(num);
	int r = 0;
	for (int i = 0; i < length; i++) {
		r += (num[i] - '0') * power_op(10, (length - i - 1));
	}
	return r;
}

char *itoa(int num, char *str, int radix)
{
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum;
	int i = 0, j, k;

	if (radix == 10 && num < 0) {
		unum = (unsigned)-num;
		str[i++] = '-';
	} else
		unum = (unsigned)num;

	do {
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;

	} while (unum);

	str[i] = '\0';

	if (str[0] == '-')
		k = 1;
	else
		k = 0;

	char temp;
	for (j = k; j <= (i - 1) / 2; j++) {
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}

char *read_name(char *str, char *target)
{
	FILE *fp;
	char buff[255];

	fp = fopen(str, "r");
	fscanf(fp, "%s", buff);
	fscanf(fp, "%s", buff);

	printf("2: %s\n", buff);
	strcpy(target, buff);
	//    return buff;
}