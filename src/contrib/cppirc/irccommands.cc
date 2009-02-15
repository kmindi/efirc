#include "ircsocket.h"

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
	if(authed) {
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
		strlcpy(_IRCNICK, nick, sizeof(_IRCNICK));
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

/* CTCP replies */
void
IRCSocket::send_ctcp_version(const char *recipient, const char *version)
{
	send_ctcp(recipient, "VERSION", version);
}

void
IRCSocket::send_ctcp_clientinfo(const char *recipient,
	const char *clientinfo)
{
	send_ctcp(recipient, "CLIENTINFO", clientinfo);
}

void
IRCSocket::send_ctcp_finger(const char *recipient, const char *finger)
{
	send_ctcp(recipient, "FINGER", finger);
}

void
IRCSocket::send_ctcp_source(const char *recipient, const char *source)
{
	send_ctcp(recipient, "SOURCE", source);
}

void
IRCSocket::send_ctcp_userinfo(const char *recipient,
	const char *userinfo)
{
	send_ctcp(recipient, "USERINFO", userinfo);
}

void
IRCSocket::send_ctcp_ping(const char *recipient, const char *ping)
{
	send_ctcp(recipient, "PING", ping);
}

void
IRCSocket::send_ctcp_time(const char *recipient, const char *time)
{
	send_ctcp(recipient, "TIME", time);
}

void
IRCSocket::send_ctcp_errmsg(const char *recipient, const char *errmsg)
{
	send_ctcp(recipient, "ERRMSG", errmsg);
}

void
IRCSocket::send_ctcp(const char *recipient, const char *type,
	const char *reply)
{
	char tmp[W_BUFSIZE];

	snprintf(tmp, sizeof(tmp), "\001%s %s\001", type, reply);
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
	int l;
	char *w, *ap;

	/* handle multiple line messages */
	l = strlen(text) + 1;

	/* working string for strsep() */
	w = new char[l];
	strlcpy(w, text, l);

	/*
	 * while substring not NULL split up wstr by separator and send
	 * it
	 */
	for(ap = w; (ap = strsep(&w, "\n")) != NULL;)
		send_raw("PRIVMSG %s :%s", recipient, ap);

	delete w;
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
	send_raw("AWAY");
}

void
IRCSocket::send_away(const char *msg)
{
	send_raw("AWAY :%s", msg);
}

/* INVITE */
void
IRCSocket::send_invite(const char *nick, const char *chan)
{
	send_raw("INVITE %s %s", nick, chan);
}

