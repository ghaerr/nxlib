#include "nxlib.h"
#include <stdlib.h>

int
XSetClipOrigin(Display *display, GC gc, int clip_x_origin, int clip_y_origin)
{
	GrSetGCClipOrigin(gc->gid, clip_x_origin, clip_y_origin);
	return 1;
}

int
XSetClipMask(Display *display, GC gc, Pixmap mask)
{
	GR_REGION_ID	r;
	GR_WINDOW_INFO	info;
	static GR_GC_ID	last_gc = 0;
	static GR_REGION_ID last_region = 0;

	if (mask == None) {
		GrSetGCRegion(gc->gid, 0);
		return 1;
	}
printf("XSetClipMask %d\n", (int)mask);

	/* avoid memory leak by deleting last region before new alloc*/
	if (last_gc == gc->gid && last_region != 0) {
		/* FIXME should keep last region in GC struct private data*/
		GrDestroyRegion(last_region);
		last_region = 0;
	}

	GrGetWindowInfo(mask, &info);
	r = GrNewRegionFromPixmap(mask, 0, 0, info.width, info.height);
	GrSetGCRegion(gc->gid, r);

	last_gc = gc->gid;
	last_region = r;
	/*GrDestroyRegion(r);*/		/* can't destroy region here...*/
	return 1;
}

int
XSetClipRectangles(Display *display, GC gc, int clip_x_origin, int clip_y_origin,
	XRectangle *rectangles, int n, int ordering)
{
	GR_REGION_ID	r;
	static GR_GC_ID	last_gc = 0;
	static GR_REGION_ID last_region = 0;

	GrSetGCClipOrigin(gc->gid, clip_x_origin, clip_y_origin);

	if (n == 0) {
		/* FIXME need to disable all output here...*/
		printf("XSetClipRectangles called with n=0\n");
		return 1;
	}

	/* avoid memory leak by deleting last region before new alloc*/
	if (last_gc == gc->gid && last_region != 0) {
		/* FIXME should keep last region in GC struct private data*/
		GrDestroyRegion(last_region);
		last_region = 0;
	}

	r = GrNewRegion();
	while (--n >= 0) {
		GR_RECT rc;

		rc.x = rectangles->x;
		rc.y = rectangles->y;
		rc.width = rectangles->width;
		rc.height = rectangles->height;
		GrUnionRectWithRegion(r, &rc);
		++rectangles;
	}
	GrSetGCRegion(gc->gid, r);

	last_gc = gc->gid;
	last_region = r;
	/*GrDestroyRegion(r);*/		/* can't destroy region here...*/
	return 1;
}
