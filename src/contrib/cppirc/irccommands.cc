#include "ircinterface.h"

/****************************
 *
 * I R C   C O M M A N D S
 *
 ****************************/

/* auth new IRC user */
void
IRCInterface::irc_auth_client(const char *nickname, const char *user_name, const char *real_name,
	const char *password)
{
	if(authed) {
		irc_write_message_f(3, "irc_auth_client", "Already authed.\n");
		return;
	}

	authed = 1;

	/*
	 * User authentifaction is needed on every connect.
	 * First we need a session password, then a specific
	 * nick and the user- and realname. These data can
	 * be viewed by calling WHOIS.
	 */
	irc_send_pass(password);
	irc_send_nick(nickname);
	irc_send_user(user_name, real_name);
}

void
IRCInterface::irc_auth_client(void)
{
	irc_auth_client(_IRCNICK, _IRCUSER, _IRCREAL, _IRCPASS);
}

/* send final QUIT message to server */
void
IRCInterface::irc_send_quit(const char *quit_message)
{
	irc_send_message_f("QUIT :%s", quit_message);
}

/* send PASS when authing user */
void
IRCInterface::irc_send_pass(const char *password)
{
	irc_send_message_f("PASS %s", password);
}

/* send NICK to change user's nickname */
void
IRCInterface::irc_send_nick(const char *nickname)
{
	irc_send_message_f("NICK %s", nickname);
}

/* send USER command to authenticate user */
void
IRCInterface::irc_send_user(const char *user_name, const char *real_name)
{
	irc_send_message_f("USER %s host serv :%s", user_name, real_name);
}

/* send JOIN command and join channel ;) */
void
IRCInterface::irc_send_join(const char *channel_name)
{
	irc_send_message_f("JOIN %s", channel_name);
}

/* send PONG back */
void
IRCInterface::irc_send_pong(const char *keyword)
{
	irc_send_message_f("PONG %s", keyword);
}

/* CTCP replies */
void
IRCInterface::irc_send_ctcp_version(const char *recipient,
	const char *version)
{
	irc_send_ctcp(recipient, "VERSION", version);
}

void
IRCInterface::irc_send_ctcp_clientinfo(const char *recipient,
	const char *client_info)
{
	irc_send_ctcp(recipient, "CLIENTINFO", client_info);
}

void
IRCInterface::irc_send_ctcp_finger(const char *recipient,
	const char *fingerprint)
{
	irc_send_ctcp(recipient, "FINGER", fingerprint);
}

void
IRCInterface::irc_send_ctcp_source(const char *recipient,
	const char *source_info)
{
	irc_send_ctcp(recipient, "SOURCE", source_info);
}

void
IRCInterface::irc_send_ctcp_userinfo(const char *recipient,
	const char *user_info)
{
	irc_send_ctcp(recipient, "USERINFO", user_info);
}

void
IRCInterface::irc_send_ctcp_ping(const char *recipient, const char *keyword)
{
	irc_send_ctcp(recipient, "PING", keyword);
}

void
IRCInterface::irc_send_ctcp_time(const char *recipient, const char *date_time)
{
	irc_send_ctcp(recipient, "TIME", date_time);
}

void
IRCInterface::irc_send_ctcp_errmsg(const char *recipient,
	const char *error_message)
{
	irc_send_ctcp(recipient, "ERRMSG", error_message);
}

void
IRCInterface::irc_send_ctcp(const char *recipient, const char *request_type,
	const char *reply_text)
{
	char tmp[W_BUFSIZE];

	snprintf(tmp, sizeof(tmp), "\001%s %s\001", request_type, reply_text);
	irc_send_notice(recipient, tmp);
}

/* the NOTICE command */
void
IRCInterface::irc_send_notice(const char *recipient, const char *text)
{
	irc_send_message_f("NOTICE %s :%s", recipient, text);
}

/* the PRIVMSG command */
void
IRCInterface::irc_send_privmsg(const char *recipient, const char *text)
{
	int l;
	char *w, *ap;

	/* handle multiple line messages */
	l = strlen(text) + 1;

	/* working string for strsep() */
	w = strdup(text);

	/*
	 * while substring not NULL split up wstr by separator and send
	 * it
	 */
	for(ap = w; (ap = strsep(&w, "\n")) != NULL;)
		irc_send_message_f("PRIVMSG %s :%s", recipient, ap);

	delete w;
}

/* TOPIC */
void
IRCInterface::irc_send_topic(const char *channel_name)
{
	/* requesting topic */
	irc_send_message_f("TOPIC %s", channel_name);
}

void
IRCInterface::irc_send_topic(const char *channel_name, const char *text)
{
	/* sending new topic */
	irc_send_message_f("TOPIC %s :%s", channel_name, text);
}

/* PART */
void
IRCInterface::irc_send_part(const char *channel_name)
{
	irc_send_message_f("PART %s", channel_name);
}

/* WHOIS */
void
IRCInterface::irc_send_whois(const char *nickname)
{
	irc_send_message_f("WHOIS %s", nickname);
}

/* AWAY */
void
IRCInterface::irc_send_away(void)
{
	irc_send_message_f("AWAY");
}

void
IRCInterface::irc_send_away(const char *away_text)
{
	irc_send_message_f("AWAY :%s", away_text);
}

/* INVITE */
void
IRCInterface::irc_send_invite(const char *nickname, const char *channel_name)
{
	irc_send_message_f("INVITE %s %s", nickname, channel_name);
}

/* KICK */
void
IRCInterface::irc_send_kick(const char *channel_name, const char *nickname)
{
	irc_send_message_f("KICK %s %s", channel_name, nickname);
}

void
IRCInterface::irc_send_kick(const char *channel_name, const char *nickname,
    const char *kick_text)
{
	irc_send_message_f("KICK %s %s :%s", channel_name, nickname,
	    kick_text);
}

/* MODE */
void
IRCInterface::irc_send_mode(const char *target_name, const char *mode_string)
{
	irc_send_message_f("MODE %s %s", target_name, mode_string);
}

void
IRCInterface::irc_send_mode(const char *target_name, const char *mode_string,
    const char *parameters)
{
	irc_send_message_f("MODE %s %s %s", target_name, mode_string,
	    parameters);
}

