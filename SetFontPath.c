#include "nxlib.h"
#include "Xatom.h"
#include <string.h>

static char **
_nxGetFontDir(int *count)
{
	int i;
	char **list;

	if (!_nxfontcount)
		_nxSetDefaultFontDir();

	list = (char **) Xcalloc(_nxfontcount+1, sizeof(char *));

	for (i = 0; i < _nxfontcount; i++)
		list[i] = strdup(_nxfontlist[i]);
	*count = _nxfontcount;

	return list;
}

char **
XGetFontPath(Display * display, int *npaths_return)
{
	return _nxGetFontDir(npaths_return);
}

int
XSetFontPath(Display * display, char **directories, int ndirs)
{
	_nxSetFontDir(directories, ndirs);
	return 1;
}

int
XFreeFontPath(char **list)
{
	_nxFreeFontDir(list);
	return 1;
}
