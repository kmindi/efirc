// TODO errorhandler for ^C
//      malloc()-use
//      high load when "couldn't create socket"
//      trigger call_cmd() when added cmd

#ifndef _IRCSOCKET_H_
#define	_IRCSOCKET_H_

#ifdef WIN32
#include <Winsock2.h>
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

#if defined WIN32 || defined linux
#include "sstring.h"
#endif

#include <stdio.h>
#include <time.h>

#define	W_BUFSIZE 512
#define	R_BUFSIZE 512 /* see RFC 1459 */
#define	HOSTSIZE 256

/*
 * struct containg message meta information:
 *
 * sender : may be server or user
 * cmd    : IRC-command
 * params : parameters to command
 */
struct irc_msg_data {
	char *sender;
	char *cmd;
	char *params;
	char *host;
	char *nick;
	char *user;
	char **params_a;
	int params_i;
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
	struct irc_act_link {
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
	struct irc_queue_cmd {
		char *buf;
		int (IRCSocket::*function)(const char *);
		irc_queue_cmd *next;
	};

	public:
		IRCSocket(unsigned int, const char *, const char *,
			const char *, const char *, const char *,
			const char *, int = 3);
		~IRCSocket(void);

		int connected;
		FILE *_DBGSTR;
		int _DBGLEVEL;
		int _DBGRECON;
		unsigned int _IRCPORT;
		size_t _DBGSPACE;
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

		void reconnect(void);

		void connect_server(unsigned int, const char *);
		void connect_server(void);
		void disconnect_server(const char *);

		void recv_raw(void);

		void auth(const char *, const char *, const char *,
			const char *);
		void auth(void);
		void send_join(const char *);
		void send_quit(const char *);
		void send_pass(const char *);
		void send_nick(const char *);
		void send_user(const char *, const char *);
		void send_notice(const char *, const char *);
		void send_privmsg(const char *, const char *);
		void send_pong(const char *);
		void send_ctcp_version(const char *, const char *);
		void send_ctcp_clientinfo(const char *, const char *);
		void send_ctcp_finger(const char *, const char *);
		void send_ctcp_source(const char *, const char *);
		void send_ctcp_userinfo(const char *, const char *);
		void send_ctcp_ping(const char *, const char *);
		void send_ctcp_time(const char *, const char *);
		void send_ctcp_errmsg(const char *, const char *);
		void send_ctcp(const char *, const char *,
			const char *);
		void send_topic(const char *);
		void send_topic(const char *, const char *);
		void send_part(const char *);
		void send_whois(const char *);
		void send_away(void);
		void send_away(const char *);
		void send_invite(const char *, const char *);

	private:
		int sock;
		int reconnecting;
		int connecting;
		int authed;
		int sleep_time;
		size_t links;
		size_t cmds;
		irc_act_link *abp;
		irc_act_link *atp;
		irc_queue_cmd *cbp;
		irc_queue_cmd *ctp;

		int debug(int, const char *, const char *, ...);
		int sock_send(const char *);
		int sock_recv(char *);

		void send_raw(const char *, ...);
		char *parse(char *);
		void parse_msg(char *);
		void cpsub(char **, char **, char);
};

#endif /* _IRCSOCKET_H_ */

