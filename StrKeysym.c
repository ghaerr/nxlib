#include "nxlib.h"
#include <string.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include "keysymstr.h"

typedef struct _keysym_map {
	GR_KEY nxKey;
	KeySym xKey;
} KeySymMap;

static KeySymMap _kmap[] = {
	{MWKEY_RIGHT, XK_Right},
	{MWKEY_LEFT, XK_Left},
	{MWKEY_UP, XK_Up},
	{MWKEY_DOWN, XK_Down},
	{MWKEY_PAGEDOWN, XK_Page_Down},
	{MWKEY_PAGEUP, XK_Page_Up},
	{MWKEY_INSERT, XK_Insert},
	{MWKEY_DELETE, XK_Delete},
	{MWKEY_HOME, XK_Home},
	{MWKEY_END, XK_End},
	{MWKEY_TAB, XK_Tab},
	{MWKEY_BACKSPACE, XK_BackSpace},
	{MWKEY_ENTER, XK_Return},
	{MWKEY_ESCAPE, XK_Escape},
	{MWKEY_LCTRL, XK_Control_L},
	{MWKEY_RCTRL, XK_Control_R},
	{MWKEY_LSHIFT, XK_Shift_L},
	{MWKEY_RSHIFT, XK_Shift_R},
	{MWKEY_LALT, XK_Alt_L},
	{MWKEY_RALT, XK_Alt_R},
	{MWKEY_LMETA, XK_Meta_L},
	{MWKEY_RMETA, XK_Meta_R},
	{MWKEY_PAUSE, XK_Pause},
	{MWKEY_SYSREQ, XK_Sys_Req},
	{MWKEY_BREAK, XK_Break},
	{MWKEY_NUMLOCK, XK_Num_Lock},
	{MWKEY_CAPSLOCK, XK_Caps_Lock},
	{MWKEY_SCROLLOCK, XK_Scroll_Lock},
	{MWKEY_F1, XK_F1},
	{MWKEY_F2, XK_F2},
	{MWKEY_F3, XK_F3},
	{MWKEY_F4, XK_F4},
	{MWKEY_F5, XK_F5},
	{MWKEY_F6, XK_F6},
	{MWKEY_F7, XK_F7},
	{MWKEY_F8, XK_F8},
	{MWKEY_F9, XK_F9},
	{MWKEY_F10, XK_F10},
	{MWKEY_F11, XK_F11},
	{MWKEY_F12, XK_F12},
	{0xffff, 0xffff}
};


KeySym
XLookupKeysym(register XKeyEvent * event, int col)
{
	int i = 0;

	for (i = 0; _kmap[i].nxKey != 0xffff; i++) {
		if (event->keycode == _kmap[i].nxKey)
			return _kmap[i].xKey;
	}

	return event->keycode;
}

int
XLookupString(register XKeyEvent * event, char *buffer,
	      int nbytes, KeySym * keysym, XComposeStatus * status)
{
	if (nbytes > 0)
		buffer[0] = 0;

	*keysym = XLookupKeysym(event, 0);
	return 0;

}

/* Freeking ugly! */
KeySym
XStringToKeysym(_Xconst char *string)
{
	int i;

	for (i = 0; i < NX_KEYSYMSTR_COUNT; i++)
		if (strcmp(nxKeyStrings[i].str, string) == 0)
			return nxKeyStrings[i].keysym;

	return NoSymbol;
}
