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

	int i, status, spaces, length;
	char level_ind;
	char timestamp[9];
	char *space, *nfmt;
	time_t raw_time;
	/* struct containing sec, min, hr,... */
	tm *local_time;
	/* for more on va_* see below */
	va_list ap;
	va_start(ap, fmt);

	/* get local time */
	time(&raw_time);
	local_time = localtime(&raw_time);

	/* set up level-indicator */
	if(level == 0)
		level_ind = '.';
	else if(level == 1)
		level_ind = 'i';
	else if(level == 2)
		level_ind = 'i';
	else if(level == 3)
		level_ind = '!';
	else
		level_ind = '?';

	/* set up space */
	spaces = _DBGSPACE - strlen(sender);

	if(spaces < 0)
		spaces = 0;

	space = new char[spaces + 1];
	memset(space, '\0', spaces + 1);

	/* fill space with spaces :) */
	for(i = 0; i < spaces; i++)
		strncat(space, " ", 1);

	/* set up timestamp (%H:%M:%S) */
	strftime(timestamp, 9, "%H:%M:%S", local_time);

	/* set up logline */
	length = strlen(space) + strlen(sender) + strlen(fmt) +
	   strlen(timestamp) + 3;
	nfmt = new char[length + 1];
	snprintf(nfmt, length + 1, "%s|%c|%s%s%s", timestamp, level_ind,
	   sender, space, fmt);

	status = vfprintf(_DBGSTR, nfmt, ap);

	va_end(ap);

	/* flush stream */
	fflush(_DBGSTR);

	delete nfmt;
	delete space;

	return status;
}

