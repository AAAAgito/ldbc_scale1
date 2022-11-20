#include "basic.h"

int sort_by_pid = false;
int output_pid = false;
int no_simple = false;
int highlight = false;
int is_ascci = true;
int is_unicode = true;

int isnumber(char *s)
{
	int len = strlen(s), i = 0;
	while (i < len) {
		if (!isdigit(s[i++])) {
			return false;
		}
	}
	return true;
}
