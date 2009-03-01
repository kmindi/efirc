#include "ircinterface.h"

/****************************
 *
 * I R C   C O M M A N D S
 *
 ****************************/

/* auth new IRC user */
void
IRCInterface::auth(const char *nick, const char *user, const char *real,
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

void
IRCInterface::auth(void)
{
	auth(_IRCNICK, _IRCUSER, _IRCREAL, _IRCPASS);
}

/* send final QUIT message to server */
void
IRCInterface::send_quit(const char *quitmsg)
{
	send_raw("QUIT :%s", quitmsg);
}

/* send PASS when authing user */
void
IRCInterface::send_pass(const char *pass)
{
	send_raw("PASS %s", pass);
}

/* send NICK to change user's nickname */
void
IRCInterface::send_nick(const char *nick)
{
	send_raw("NICK %s", nick);

// TODO sometime we DO NOT have this nick
	/* new nick requested? */
	if(strcmp(_IRCNICK, nick))
		strlcpy(_IRCNICK, nick, sizeof(_IRCNICK));
}

/* send USER command to authenticate user */
void
IRCInterface::send_user(const char *user, const char *real)
{
	send_raw("USER %s host serv :%s", user, real);
}

/* send JOIN command and join channel ;) */
void
IRCInterface::send_join(const char *chan)
{
	send_raw("JOIN %s", chan);
}

/* send PONG back */
void
IRCInterface::send_pong(const char *key)
{
	send_raw("PONG %s", key);
}

/* CTCP replies */
void
IRCInterface::send_ctcp_version(const char *recipient,
	const char *version)
{
	send_ctcp(recipient, "VERSION", version);
}

void
IRCInterface::send_ctcp_clientinfo(const char *recipient,
	const char *clientinfo)
{
	send_ctcp(recipient, "CLIENTINFO", clientinfo);
}

void
IRCInterface::send_ctcp_finger(const char *recipient,
	const char *finger)
{
	send_ctcp(recipient, "FINGER", finger);
}

void
IRCInterface::send_ctcp_source(const char *recipient,
	const char *source)
{
	send_ctcp(recipient, "SOURCE", source);
}

void
IRCInterface::send_ctcp_userinfo(const char *recipient,
	const char *userinfo)
{
	send_ctcp(recipient, "USERINFO", userinfo);
}

void
IRCInterface::send_ctcp_ping(const char *recipient, const char *ping)
{
	send_ctcp(recipient, "PING", ping);
}

void
IRCInterface::send_ctcp_time(const char *recipient, const char *time)
{
	send_ctcp(recipient, "TIME", time);
}

void
IRCInterface::send_ctcp_errmsg(const char *recipient,
	const char *errmsg)
{
	send_ctcp(recipient, "ERRMSG", errmsg);
}

void
IRCInterface::send_ctcp(const char *recipient, const char *type,
	const char *reply)
{
	char tmp[W_BUFSIZE];

	snprintf(tmp, sizeof(tmp), "\001%s %s\001", type, reply);
	send_notice(recipient, tmp);
}

/* the NOTICE command */
void
IRCInterface::send_notice(const char *recipient, const char *text)
{
	send_raw("NOTICE %s :%s", recipient, text);
}

/* the PRIVMSG command */
void
IRCInterface::send_privmsg(const char *recipient, const char *text)
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
IRCInterface::send_topic(const char *chan)
{
	/* requesting topic */
	send_raw("TOPIC %s", chan);
}

void
IRCInterface::send_topic(const char *chan, const char *text)
{
	/* sending new topic */
	send_raw("TOPIC %s :%s", chan, text);
}

/* PART */
void
IRCInterface::send_part(const char *chan)
{
	send_raw("PART %s", chan);
}

/* WHOIS */
void
IRCInterface::send_whois(const char *nick)
{
	send_raw("WHOIS %s", nick);
}

/* AWAY */
void
IRCInterface::send_away(void)
{
	send_raw("AWAY");
}

void
IRCInterface::send_away(const char *msg)
{
	send_raw("AWAY :%s", msg);
}

/* INVITE */
void
IRCInterface::send_invite(const char *nick, const char *chan)
{
	send_raw("INVITE %s %s", nick, chan);
}

