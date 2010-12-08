#include "nxlib.h"
#include <stdlib.h>
#include <string.h>


char **
XListFonts(Display * display, _Xconst char *pattern, int maxnames,
	int *actual_count_return)
{
	return font_enumfonts((char *)pattern, maxnames, actual_count_return);
}

int
XFreeFontNames(char **list)
{
	font_freefontnames(list);
	return 1;
}
