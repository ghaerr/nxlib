#include "nxlib.h"

int
XMapRaised (Display *dpy, Window w)
{
	printf("MapRaised %d\n", w);
	GrMapWindow(w);
	GrRaiseWindow(w);
	return 1;
}
