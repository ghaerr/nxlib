#include "nxlib.h"
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PIXEL_SIZE	14	/* default size for scalable fonts*/

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

char *
_nxFindX11Font(const char *xfontname)
{
	int fcount, i, f;
	char *ret;
	char buffer[128];

	if (!xfontname)
		return 0;

	if (!_nxfontcount)
		_nxSetDefaultFontDir();

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

		for (i = 0; i < fcount; i++) {
			char *file = buffer, *font = 0;

			fgets(buffer, 128, fontdir);

			/* Remove the end 'o line */
			buffer[strlen(buffer) - 1] = '\0';

			/* Find the field seperator */

			font = strchr(buffer, ' ');
			*font++ = '\0';

			//printf("checking '%s' '%s'\n", xfontname, font);
			if (strcmp(xfontname, font) == 0) {
				ret = (char *) Xmalloc(strlen(_nxfontlist[f]) +
						      strlen(file) + 2);
				sprintf(ret, "%s/%s", _nxfontlist[f], file);

				fclose(fontdir);
				return ret;
			}
		}

		/* not found, try <prefix.pcf> */
		fseek(fontdir, 0L, SEEK_SET);
		fgets(buffer, 128, fontdir);
		for (i = 0; i < fcount; i++) {
			char *file = buffer, *font = 0;

			fgets(buffer, 128, fontdir);

			/* Remove the end 'o line */
			buffer[strlen(buffer) - 1] = '\0';

			/* Find the field seperator */

			font = strchr(buffer, ' ');
			*font++ = '\0';

			//printf("chk2 '%s' '%s'\n", xfontname, file);
			if (prefix(xfontname, file)) {
				ret = (char *) Xmalloc(strlen(_nxfontlist[f]) +
						      strlen(file) + 2);
				sprintf(ret, "%s/%s", _nxfontlist[f], file);

				fclose(fontdir);
				return ret;
			}
		}

		if (fontdir)
			fclose(fontdir);
	}
	return 0;
}

static int
any(int c, const char *str)
{
	while (*str)
		if (*str++ == c)
			return 1;
	return 0;
}

Font
XLoadFont(Display * dpy, _Xconst char *name)
{
	GR_FONT_ID	font = 0;
	char *		fontname;
	int		defpixelsize = DEFAULT_PIXEL_SIZE;
	char **		fontlist = NULL;

	/* Check for wildcards in font 'name' (the X11 font string). */
	if (any('*', name) || any('?', name)) {
		int 	count;

		/* pick first sorted return value for now...*/
		fontlist = XListFonts(dpy, name, 100000, &count);
		if (fontlist)
			fontname = fontlist[0];
	} else
		fontname = (char *)name;

	/* See if 'fontname' is stored in 'X11/fonts.dir' file */
	fontname = _nxFindX11Font(fontname);

	/*
	 * If the font is still not found, then parse again 'name',
	 * but with a scalable pixel size value ('*').
  	 */
	if (fontname == NULL) {
		int	i;
		int	dashCounter = 0;
		int	count;
		char	*fontName = (char*)Xmalloc(strlen(name));
		char	*strPtr;

		if (fontName == NULL)
			return 0;

		strcpy(fontName, name);
		for (i = 0; i < strlen(fontName); i++) {
			if (fontName[i] == '-') {
				dashCounter++;
				if ( (dashCounter == 6) && (i + 2) <= strlen(fontName) ) {
					fontName[i + 1] = '*';
					fontName[i + 2] = '\0';
				}
			}
		}

		/* Search again in 'X11/fonts.dir'. */
		if (fontlist)
			XFreeFontNames(fontlist);
		fontlist = XListFonts(dpy, fontName, 100000, &count);
		if (fontlist) {
			fontname = fontlist[0];
			fontname = _nxFindX11Font(fontname);
		}
		Xfree(fontName);

		/* Try to get the requested pixel size for the font. */
		strPtr = strstr(name, "--");
		if (strPtr != NULL)
			sscanf(strPtr, "--%d", &defpixelsize);
	}

	/* if not found, try 6x13 for "fixed"*/
	if (!fontname && !strcmp(name, "fixed"))
		fontname = _nxFindX11Font("6x13");

	/* found font, load into server*/
	if (fontname)
		font = GrCreateFont(fontname, defpixelsize, NULL);
printf("XLoadFont('%s') = '%s' size %d [%d]\n", name, fontname, defpixelsize, font);

	if (fontname)
		Xfree(fontname);
	if (fontlist)
		XFreeFontNames(fontlist);
	return font;
}

/* Stub out XCreateFontSet - but return the right values so we don't 
   freak out GTK 
*/
XFontSet
XCreateFontSet(Display *display, _Xconst char *base_font_name_list, 
	char ***missing_charset_list_return, int *missing_charset_count_return,
	char **def_string_return)
{
	*missing_charset_list_return = NULL;
	*missing_charset_count_return = 0;
	return NULL;
}
