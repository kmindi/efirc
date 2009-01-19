// TODO errorhandler for ^C
//      malloc()-use
//      high load when "couldn't create socket"

#ifndef __IRCSOCKET_H__
#define __IRCSOCKET_H__

#ifdef WINDOWS
#include <windows.h>
/*	$OpenBSD: strlcpy.c,v 1.11 2006/05/05 15:27:38 millert Exp $	*/

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <string.h>

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t
strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

/*	$OpenBSD: strlcat.c,v 1.13 2005/08/08 08:05:37 espie Exp $	*/

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <string.h>

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
size_t
strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));	/* count does not include NUL */
}

/*	$OpenBSD: strsep.c,v 1.6 2005/08/08 08:05:37 espie Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>

/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.  
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */
char *
strsep(char **stringp, const char *delim)
{
	char *s;
	const char *spanp;
	int c, sc;
	char *tok;

	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}
#else
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <time.h>

#define W_BUFSIZE 512
#define R_BUFSIZE 512 // see RFC 1459
#define HOSTSIZE 256

/*
 * struct containg message meta information:
 *
 * sender : may be server or user
 * cmd    : IRC-command
 * params : parameters to command
 */
struct
irc_msg_data
{
	char *sender;
	char *cmd;
	char *params;
	char *host;
	char *nick;
	char *user;
	char **params_a;
	unsigned int params_i;
};

/*
 * The IRCSocket provides funtions to connect and disconnect
 * an IRC Server, send and recv raw messages and parse them.
 */
class IRCSocket
{
	/*
	 * struct containing link information
	 *
	 * cmd      : IRC event waited for
	 * function : function to call
	 */
	struct
	irc_act_link
	{
		char *cmd;
		void (*function)(const irc_msg_data*, void*);
		irc_act_link *next;
	};

	/*
	 * struct containing queue cmd information
	 *
	 * buf      : buf to pass to function
	 * function : function to call
	 */
	struct
	irc_queue_cmd
	{
		char *buf;
		int (IRCSocket::*function)(const char *);
		irc_queue_cmd *next;
	};

	public:
		IRCSocket(unsigned int, const char *, const char *,
			const char *, const char *, const char *,
			FILE *);
		~IRCSocket(void);

		unsigned int connected;
		FILE *_DBGSTR;
		unsigned int _DBGSPACE;
		unsigned int _DBGLEVEL;
		unsigned int _DBGRECON;
		unsigned int _IRCPORT;
		char _IRCSERV[HOSTSIZE];
		char _IRCNICK[W_BUFSIZE];
		char _IRCUSER[W_BUFSIZE];
		char _IRCREAL[W_BUFSIZE];
		char _IRCPASS[W_BUFSIZE];

		void add_link(const char *,
			void (*function)(const irc_msg_data*, void*));
		void del_link(const char *,
			void (*function)(const irc_msg_data*, void*));
		void act_link(const irc_msg_data*);

		void add_cmd(const char *,
			int (IRCSocket::*function)(const char *));
		void del_cmd(void);
		void call_cmd(void);

		void reconnect();

		void connect_server(unsigned int, const char *);
		void disconnect_server(const char *);

		void recv_raw(void);

		void auth(const char *, const char *, const char *,
			const char *);
		void send_join(const char *);
		void send_quit(const char *);
		void send_pass(const char *);
		void send_nick(const char *);
		void send_user(const char *, const char *);
		void send_notice(const char *, const char *);
		void send_privmsg(const char *, const char *);
		void send_pong(const char *);
		void send_version(const char *);
		void send_topic(const char *);
		void send_topic(const char *, const char *);
		void send_part(const char *);
		void send_whois(const char *);
		void send_away(void);
		void send_away(const char *);

	private:
		int sock;
		char* VERSION[W_BUFSIZE];
		unsigned int links;
		unsigned int cmds;
		unsigned int reconnecting;
		unsigned int connecting;
		unsigned int authed;
		unsigned int sleep_time;
		irc_act_link *abp;
		irc_act_link *atp;
		irc_queue_cmd *cbp;
		irc_queue_cmd *ctp;

		int debug(unsigned int, const char *, const char *,
			...);
		int sock_send(const char *);
		int sock_recv(char *);

		void send_raw(const char *, ...);
		char *parse(char *);
		void parse_msg(char *);
		void cpsub(char **, char **, char);
};

#endif

