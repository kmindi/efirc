#include "ircinterface.h"

/****************************
 *
 * I O
 *
 ****************************/

int
IRCInterface::irc_write_message_f(int debug_level, const char *caller_name, const char *message_format, ...)
{
	/* return if level is below wanted level */
	if(debug_level < _DBGLEVEL)
		return 1;

	int s, l;
	char lc;
	char timestamp[9];
	char *nfmt;
	time_t rt;
	/* struct containing sec, min, hr,... */
	tm *lt;
	/* for more on va_* see below */
	va_list ap;
	va_start(ap, message_format);

	/* get local time */
	time(&rt);
	lt = localtime(&rt);

	/* set up level-indicator */
	switch(debug_level) {
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

	/* set up timestamp (%H:%M:%S) */
	strftime(timestamp, 9, "%H:%M:%S", lt);

	/*
	 * Length has to consider the separators `|' and `\t' as well as
	 * the debug level number. Therefore 4 bytes extra.
	 */
	l = strlen(caller_name) + strlen(message_format) +
		strlen(timestamp) + 4;

	/* remember NUL termination, too */
	nfmt = new char[++l];
	snprintf(nfmt, l, "%s|%c|%s\t%s", timestamp, lc,
	   caller_name, message_format);

	s = vfprintf(_DBGSTR, nfmt, ap);

	va_end(ap);

	/* flush stream */
	fflush(_DBGSTR);

	delete[] nfmt;

	return s;
}

