#include <string.h>
#include "nxlib.h"

#define SZHASHTABLE	32
struct window_props {
	Atom property;
	Atom type;
	int format;
	unsigned char *data;
	int nelements;
	int bytes;
	struct window_props *next;
};

struct windows {
	Window w;
	struct window_props *properties;
	struct windows *next;
};
static struct windows *window_list[SZHASHTABLE];

int
XChangeProperty(Display * display, Window w, Atom property,
		Atom type, int format, int mode,
		_Xconst unsigned char *data, int nelements)
{
	struct windows *win = NULL;
	struct window_props *prop = NULL;
	int hash = w % SZHASHTABLE;

	win = window_list[hash];
	if (!win) {
		win = window_list[hash] =
			(struct windows *) Xcalloc(sizeof(struct windows), 1);
	} else {
		struct windows *t;

		for (t=win; ; t=t->next) {
			if (t->w == w) {
				win = t;
				break;
			}
			if (!t->next)
				break;
		}

		if (!win) {
			win = t->next =
				(struct windows *) Xcalloc(sizeof(struct windows), 1);
		}
	}
	win->w = w;

	if (!win->properties) {
		prop = win->properties =
			(struct window_props *) Xcalloc(sizeof(struct window_props), 1);
	} else {
		struct window_props *t;

		for (t=win->properties; ; t=t->next) {
			if (t->property == property) {
				prop = t;
				break;
			}
			if (!t->next)
				break;
		}

		if (!prop) {
			prop = t->next =
				(struct window_props *) Xcalloc(sizeof(struct window_props), 1);
		}
	}

	switch (mode) {
	case PropModeAppend:
	case PropModePrepend:
		if (prop->data) {
			int f8 = prop->format / 8;
			char *p;
			int bytes;

			if (type != prop->type || format != prop->format)
				return 0;

			bytes = (prop->nelements + nelements) * f8;
			p = (char *) Xmalloc(bytes);

			if (mode == PropModeAppend) {
				memcpy(p, prop->data, prop->nelements * f8);
				memcpy(p + (prop->nelements * f8), data,
				       (nelements * f8));
			} else {
				memcpy(p, data, nelements * f8);
				memcpy(p + (nelements * f8), prop->data,
				       (prop->nelements * f8));
			}

			Xfree(prop->data);
			prop->data = (unsigned char *)p;
			prop->nelements += nelements;
			prop->bytes = bytes;
			break;
		}
		/* Fall through */

	case PropModeReplace:
		if (prop->data)
			Xfree(prop->data);
		prop->bytes = nelements * (format / 8);
		prop->data = (unsigned char *) Xmalloc(prop->bytes);
		memcpy(prop->data, data, prop->bytes);

		prop->property = property;
		prop->type = type;
		prop->format = format;
		prop->nelements = nelements;
		break;
	}

	return 1;
}

int
XDeleteProperty(Display *display, Window w, Atom property)
{
	struct windows *win;
	struct window_props *prop;
	int hash = w % SZHASHTABLE;

	for (win = window_list[hash]; win; win = win->next)
		if (win->w == w) {
			struct window_props *prev = NULL;

			for (prop = win->properties; prop; prop = prop->next)
				if (prop->property == property) {
					if (prev)
						prev->next = prop->next;
					else
						win->properties = prop->next;

					if (prop->data)
						Xfree(prop->data);
					Xfree(prop);

					if (win == window_list[hash])
						window_list[hash] = NULL;
					return 1;
				}
		}

	return 1;
}

int
_nxDelAllProperty(Window w)
{
	struct windows *win;
	struct window_props *prop;
	int hash = w % SZHASHTABLE;

	for (win = window_list[hash]; win; win = win->next)
		if (win->w == w) {
			prop = win->properties;
			while (prop) {
				struct window_props *next = prop->next;

				if (prop->data)
					Xfree(prop->data);
				Xfree(prop);
				prop = next;
			}

			Xfree(win);

			if (win == window_list[hash])
				window_list[hash] = NULL;
			return 1;
		}
	return 1;
}

int
XGetWindowProperty(Display * dpy, Window w, Atom property, long offset,
	long len, Bool bDel, Atom req, Atom * type, int *format,
	unsigned long *nitems, unsigned long *bytes, unsigned char **data)
{
	struct windows *win;
	struct window_props *prop;
	int hash = w % SZHASHTABLE;

	for (win = window_list[hash]; win; win = win->next) {
		if (win->w == w) {
			for (prop = win->properties; prop; prop = prop->next) {
				if (prop->property == property) {
					*type = prop->type;
					*format = prop->format;
					*data = prop->data;
					*nitems = prop->nelements;
					*bytes = prop->bytes;
					return 0;
				}
			}
		}
	}
	*type = None;
	*format = 0;
	*data = 0;
	*nitems = 0;
	*bytes = 0;
	return 1;		/* failure */
}

