#include "nxlib.h"

int
XMapWindow (Display *dpy, Window w)
{
	printf("Mapping window %d\n", w);
	GrMapWindow(w);
	return 1;
}
