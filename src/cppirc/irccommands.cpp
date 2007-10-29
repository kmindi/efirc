/****************************
 *
 * I R C   C O M M A N D S
 *
 ****************************/

/* auth new IRC user */
void
IRCSocket::auth(const char *nick, const char *user, const char *real,
   const char *pass)
{
// TODO you aren't allowed to call when already done?
	if(authed)
	{
		debug(3, "auth", "Already authed.\n");
		return;
	}

	authed = 1;

	/*
	 * User authentifaction is needed on every connect.
	 * First we need a session password, then a specific
	 * nick and the user- and realname. These data can
	 * be viewed by calling WHOIS.
	 */
	send_pass(pass);
	send_nick(nick);
	send_user(user, real);
}

/* send final QUIT message to server */
void
IRCSocket::send_quit(const char *quitmsg)
{
	send_raw("QUIT :%s", quitmsg);
}

/* send PASS when authing user */
void
IRCSocket::send_pass(const char *pass)
{
	send_raw("PASS %s", pass);
}

/* send NICK to change user's nickname */
void
IRCSocket::send_nick(const char *nick)
{
	send_raw("NICK %s", nick);

// TODO sometime we DO NOT have this nick
	/* new nick requested? */
	if(strcmp(_IRCNICK, nick))
	{
		strncpy(_IRCNICK, nick, W_BUFSIZE - 1);
		_IRCNICK[W_BUFSIZE - 1] = '\0';
	}
}

/* send USER command to authenticate user */
void
IRCSocket::send_user(const char *user, const char *real)
{
	send_raw("USER %s host serv :%s", user, real);
}

/* send JOIN command and join channel ;) */
void
IRCSocket::send_join(const char *chan)
{
	send_raw("JOIN %s", chan);
}

/* send PONG back */
void
IRCSocket::send_pong(const char *key)
{
	send_raw("PONG %s", key);
}

/* CTCP - our client version */
void
IRCSocket::send_version(const char *recipient)
{
	char tmp[W_BUFSIZE];

	snprintf(tmp, W_BUFSIZE, "\001VERSION\001 %s", VERSION);
	send_notice(recipient, tmp);
}

/* the NOTICE command */
void
IRCSocket::send_notice(const char *recipient, const char *text)
{
	send_raw("NOTICE %s :%s", recipient, text);
}

/* the PRIVMSG command */
void
IRCSocket::send_privmsg(const char *recipient, const char *text)
{
	int length;
	char *wstr;
	char *substr;
	char separator[2];

	/* handle multiple line messages */
	length = strlen(text);

	/* working string for strtok() */
	wstr = new char[length + 1];

	strncpy(wstr, text, length + 1);
	strncpy(separator, "\n", 2);

	/* while substr not NULL split up wstr by separator and send it */
	for(substr = strtok(wstr, separator); substr;
	   substr = strtok(NULL, separator))
		send_raw("PRIVMSG %s :%s", recipient, substr);

	delete wstr;
}

/* TOPIC */
void
IRCSocket::send_topic(const char *chan)
{
	/* requesting topic */
	send_raw("TOPIC %s", chan);
}

void
IRCSocket::send_topic(const char *chan, const char *text)
{
	/* sending new topic */
	send_raw("TOPIC %s :%s", chan, text);
}

/* PART */
void
IRCSocket::send_part(const char *chan)
{
	send_raw("PART %s", chan);
}

/* WHOIS */
void
IRCSocket::send_whois(const char *nick)
{
	send_raw("WHOIS %s", nick);
}

/* AWAY */
void
IRCSocket::send_away(void)
{
	/* unmark as away */
	send_raw("AWAY");
}

void
IRCSocket::send_away( const char *text)
{
	/* mark as away with awaymessage */
	send_raw("AWAY %s", text);
}



