#include "nxlib.h"
#include <string.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

Status
XStringListToTextProperty(char **argv, int argc, XTextProperty * ret)
{
	char *buffer;
	int count = 0, i;
	XTextProperty proto;

	for (i = 0; i < argc; i++)
		count += (argv[i] ? strlen(argv[i]) : 0) + 1;

	proto.encoding = XA_STRING;
	proto.format = 8;
	proto.value = 0;

	if (count > 0) {
		proto.nitems = count - 1;

		buffer = (char *) Xmalloc(count);
		if (!buffer)
			return 0;

		proto.value = buffer;

		for (i = 0; i < argc; i++) {
			if (argv[i]) {
				strcpy(buffer, argv[i]);
				buffer += strlen(argv[i] + 1);
			} else {
				*buffer++ = '\0';
			}
		}
	} else {
		proto.nitems = 0;
		if (!(proto.value = (char *) Xmalloc(1)))
			return 0;

		*proto.value = '\0';
	}

	memcpy(ret, &proto, sizeof(*ret));
	return 1;
}
