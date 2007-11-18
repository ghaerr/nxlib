#include "nxlib.h"
#include "Xatom.h"
#include <stdlib.h>
#include <string.h>

static int
min(int a, int b)
{
	return (a < b)? a: b;
}

static int
prefix(const char *prestr, char *allstr)
{
	while (*prestr)
		if (*prestr++ != *allstr++)
			return 0;
	if (*allstr != '.')
		return 0;
	return 1;
}

static int
any(int c, const char *str)
{
	while (*str)
		if (*str++ == c)
			return 1;
	return 0;
}

static char *
_nxFindX11Font(const char *matchstr, int *height)
{
	int fcount, i, f;
	char *ret;
	char buffer[128];

	if (!_nxfontcount)
		_nxSetDefaultFontDir();

	/* return height 0 unless requested scaleable pixel size*/
	*height = 0;

	/* Go through all of the font dirs */
	for (f = 0; f < _nxfontcount; f++) {
		FILE *fontdir = _nxLoadFontDir(_nxfontlist[f]);
		if (!fontdir)
			continue;

		fgets(buffer, 128, fontdir);
		fcount = atoi(buffer);

		if (!fcount) {
			fclose(fontdir);
			continue;
		}

		/* Then through each line in the fonts.dir file for XLFD match*/
		for (i = 0; i < fcount; i++) {
			char *file = buffer, *xlfd;

			fgets(buffer, 128, fontdir);

			/* XLFD is second space separated field*/
			buffer[strlen(buffer) - 1] = '\0';
			xlfd = strchr(buffer, ' ');
			*xlfd++ = '\0';

			/* check for exact XLFD match first*/
			if (strcmp(matchstr, xlfd) == 0) {
return_fontpath:
				/* return full path to font file*/
				ret = (char *) Xmalloc(strlen(_nxfontlist[f]) +
						      strlen(file) + 2);
				sprintf(ret, "%s/%s", _nxfontlist[f], file);

				fclose(fontdir);
				return ret;
			} else {
				/* otherwise check for scaleable font match spec
				 * with passed pixel size, that is:
				 *     match XLFD  "...normal--0-0-0-0-0-..."
				 * with passed     "...normal--12-0-0-0-0-..."
				 * for height 12.
				 */
				int j;
				int reqsize = 0;
				int dashcount = 0;
				int len = min(strlen(xlfd), strlen(matchstr));

				/* match passed scaleable height at '--0-' in XLFD string*/
				for (j = 0; j < len && dashcount < 8; j++) {
					if (xlfd[j] == '-')
						++dashcount;
					if (dashcount == 7 && xlfd[j] == '0' && xlfd[j] != matchstr[j]) {
						int st = j;
						while (matchstr[j] >= '0' && matchstr[j] <= '9') {
							reqsize = reqsize * 10 + (matchstr[j++] - '0');
						}
						if (strcmp(&matchstr[j], &xlfd[st+1]) == 0) {
							*height = reqsize;
							goto return_fontpath;
						}
						break;
					}
				}
			}
		}

		/* Not found, check each line for passed prefix within fontname*/
		fseek(fontdir, 0L, SEEK_SET);
		fgets(buffer, 128, fontdir);
		for (i = 0; i < fcount; i++) {
			char *file = buffer, *p;

			fgets(buffer, 128, fontdir);

			/* fontname is first space separated field*/
			buffer[strlen(buffer) - 1] = '\0';
			p = strchr(buffer, ' ');
			*p = '\0';

			/* prefix allows font.pcf to match font.pcf.gz for example*/
			if (prefix(matchstr, file))
				goto return_fontpath;
		}

		if (fontdir)
			fclose(fontdir);
	}
	return NULL;
}

Font
XLoadFont(Display * dpy, _Xconst char *name)
{
	GR_FONT_ID font = 0;
	char *	   fontname;
	int	   height;

	/* first check for wildcards*/
	if (any('*', name) || any('?', name)) {
		char **fontlist;
		int count;

		/* pick first sorted return value for now...*/
		fontlist = XListFonts(dpy, name, 100000, &count);
		if (fontlist)
			fontname = fontlist[0];
	} else
		fontname = (char *)name;

	/* first try to find XLFD or fontname from X11/fonts.dir file*/
	fontname = _nxFindX11Font(fontname, &height);

	/* if not found, try 6x13 for "fixed"*/
	if (!fontname && !strcmp(name, "fixed"))
		fontname = _nxFindX11Font("6x13", &height);

	/* found font, load into server*/
	if (fontname)
		font = GrCreateFont(fontname, height, NULL);

printf("XLoadFont('%s') = '%s' height %d [%d]\n", name, fontname, height, font);
	if (fontname)
		Xfree(fontname);
	return font;
}

/* Stub out XCreateFontSet*/
XFontSet
XCreateFontSet(Display *display, _Xconst char *base_font_name_list, 
	char ***missing_charset_list_return, int *missing_charset_count_return,
	char **def_string_return)
{
	*missing_charset_list_return = NULL;
	*missing_charset_count_return = 0;
	return NULL;
}

/* Stub out XGetFontProperty*/
Bool
XGetFontProperty(XFontStruct * font, Atom atom, unsigned long *value_return)
{
printf("XGetFontProperty called\n");
	switch (atom) {
	case XA_FONT:			/* 18*/
	case XA_UNDERLINE_POSITION:	/* 51*/
	case XA_UNDERLINE_THICKNESS:	/* 52*/
		break;
	default:
		printf("XGetFontProperty: Unknown FontProperty Atom %d\n",
			(int)atom);
	}
	return 0;
}
