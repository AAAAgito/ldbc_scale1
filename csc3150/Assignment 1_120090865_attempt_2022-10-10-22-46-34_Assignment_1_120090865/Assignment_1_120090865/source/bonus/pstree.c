#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>
#include "string.h"
#include "basic.h"
#include "get.h"
#include "sort.h"
#include "build.h"

int main(int argc, char *argv[])
{
	int start = 1, type = 0;
	Process *root = malloc(sizeof(Process));
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-V") || !strcmp(argv[i], "--version")) {
			printf("pstree 1.0\nCopyright (C) 2022 Yang Zhao\n");
			return 0;
		} else if (!strcmp(argv[i], "-n") ||
			   !strcmp(argv[i], "--numeric-sort")) {
			sort_by_pid = true;
		} else if (!strcmp(argv[i], "-p") ||
			   !strcmp(argv[i], "--show-pids")) {
			output_pid = true;
		} else if (!strcmp(argv[i], "-c")) {
			no_simple = true;
		} else if (!strcmp(argv[i], "-h")) {
			highlight = true;
		} else if (!strcmp(argv[i], "-A")) {
			is_ascci = true;
		} else if (!strcmp(argv[i], "-U")) {
			is_unicode = true;
		} else if (isnumber(argv[i])) //  pstree PID
		{
			if (isnumber(argv[i])) {
				output_pid = true;
				start = atoi(argv[i]);
			}
		} else {
			printf("pstree: invalid option -- %s\n", argv[i]);
			printf("Usage: pstree [ -c ] [ -h ] [ -n ] [ -p ] [ -V ] [ -A ] [ -U ] [ PID ]\n");
			return -1;
		}
	}
	gain(root, start);
	buildTree(root, type, true);
	free(root);
	return 0;
}