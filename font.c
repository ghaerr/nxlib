#include "nxlib.h"
#include <stdlib.h>
#include <string.h>

char **_nxfontlist = NULL;
int _nxfontcount = 0;

/* default X11 font directory list, add directories if desired*/
static char *NXDIRLIST[] = {
	X11_FONT_DIR1,
	X11_FONT_DIR2,
	X11_FONT_DIR3,
	0
};

FILE *
_nxLoadFontDir(char *str)
{
	char path[256];

	sprintf(path, "%s/fonts.dir", str);
	return fopen(path, "r");
}


FILE *
_nxLoadFontAlias(char *str)
{
	char path[256];

	sprintf(path, "%s/fonts.alias", str);
	return fopen(path, "r");
}


void
_nxSetDefaultFontDir(void)
{
	int ndirs = (sizeof(NXDIRLIST) / sizeof(char *)) - 1;

	_nxFreeFontDir(_nxfontlist);

	_nxSetFontDir(NXDIRLIST, ndirs);
}

void
_nxSetFontDir(char **directories, int ndirs)
{
	int i;

	_nxFreeFontDir(_nxfontlist);

	_nxfontlist = (char **) Xcalloc(ndirs+1, sizeof(char *));
	for (i = 0; i < ndirs; i++)
		_nxfontlist[i] = strdup(directories[i]);

	_nxfontcount = ndirs;
}

void
_nxFreeFontDir(char **list)
{
	int i;

	if (list) {
		for (i = 0; list[i]; i++)
			free(list[i]);
		Xfree(list);
	}
}
