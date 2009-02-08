#include "ircsocket.h"

/****************************
 *
 * I O
 *
 ****************************/

int
IRCSocket::debug(unsigned int level, const char *sender,
	const char *fmt, ...)
{
	/* return if level is below wanted level */
	if(level < _DBGLEVEL)
		return 1;

	int i, s, l;
	char lc;
	char timestamp[9];
	char *d, *nfmt;
	time_t rt;
	/* struct containing sec, min, hr,... */
	tm *lt;
	/* for more on va_* see below */
	va_list ap;
	va_start(ap, fmt);

	/* get local time */
	time(&rt);
	lt = localtime(&rt);

	/* set up level-indicator */
	switch(level) {
		case 0:
			lc = '.';
		case 1:
			lc = 'i';
		case 2:
			lc = 'i';
		case 3:
			lc = '!';
		default:
			lc = '?';
	}

	/* set up space */
	l = _DBGSPACE - strlen(sender) + 1;

	if(l < 0)
		l = 0;

	d = new char[l];
	/* fill space with spaces :) */
	memset(d, ' ', l);
	d[l - 1] = '\0';

	/* set up timestamp (%H:%M:%S) */
	strftime(timestamp, 9, "%H:%M:%S", lt);

	/* set up logline */
	l = strlen(d) + strlen(sender) + strlen(fmt) +
		strlen(timestamp) + 4;
	nfmt = new char[l];
	snprintf(nfmt, l, "%s|%c|%s%s%s", timestamp, lc,
	   sender, d, fmt);

	s = vfprintf(_DBGSTR, nfmt, ap);

	va_end(ap);

	/* flush stream */
	fflush(_DBGSTR);

	delete nfmt;
	delete d;

	return s;
}

