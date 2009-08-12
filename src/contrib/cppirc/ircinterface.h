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

#include "sstring.h"

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
 * The IRCInterface provides funtions to connect and disconnect
 * an IRC Server, send and recv raw messages and parse them.
 */
class IRCInterface
{
	/*
	 * struct containing link information
	 *
	 * cmd      : IRC event waited for
	 * function : function to call
	 */
	struct irc_act_link {
		char *cmd;
		void (*function)(const irc_msg_data *, void *);
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
		int (IRCInterface::*function)(const char *);
		irc_queue_cmd *next;
	};

	public:
		IRCInterface(unsigned int, const char *, const char *,
			const char *, const char *, const char *,
			const char *, int = 3);
		~IRCInterface(void);

		int connected;
		int disconnected;
		FILE *_DBGSTR;
		int _DBGLEVEL;
		int _DBGRECON;
		unsigned int _IRCPORT;
		char _IRCSERV[HOSTSIZE];
		char _IRCNICK[W_BUFSIZE];
		char _IRCUSER[W_BUFSIZE];
		char _IRCREAL[W_BUFSIZE];
		char _IRCPASS[W_BUFSIZE];

		void irc_add_link_queue_entry(const char *,
			void (*)(const irc_msg_data *, void *));
		void irc_delete_link_queue_entry(const char *,
			void (*)(const irc_msg_data *, void *));
		void irc_call_link_queue_entry(const irc_msg_data *);

		void irc_set_default_link_function(void (*)(
			const irc_msg_data *, void *));

		void irc_add_command_queue_entry(int (IRCInterface::*)(const char *),
			const char *);
		void irc_delete_command_queue_entry(void);
		void irc_call_command_queue_entries(void);

		void irc_reconnect_server(void);

		void irc_connect_server(const char *, unsigned int);
		void irc_connect_server(void);
		void irc_disconnect_server(const char *);
		void irc_disconnect_server(void);

		void irc_receive_messages(void);

		void irc_auth_client(const char *, const char *, const char *,
			const char *);
		void irc_auth_client(void);
		void irc_send_join(const char *);
		void irc_send_quit(const char *);
		void irc_send_pass(const char *);
		void irc_send_nick(const char *);
		void irc_send_user(const char *, const char *);
		void irc_send_notice(const char *, const char *);
		void irc_send_privmsg(const char *, const char *);
		void irc_send_pong(const char *);
		void irc_send_ctcp_version(const char *, const char *);
		void irc_send_ctcp_clientinfo(const char *, const char *);
		void irc_send_ctcp_finger(const char *, const char *);
		void irc_send_ctcp_source(const char *, const char *);
		void irc_send_ctcp_userinfo(const char *, const char *);
		void irc_send_ctcp_ping(const char *, const char *);
		void irc_send_ctcp_time(const char *, const char *);
		void irc_send_ctcp_errmsg(const char *, const char *);
		void irc_send_ctcp(const char *, const char *,
			const char *);
		void irc_send_topic(const char *);
		void irc_send_topic(const char *, const char *);
		void irc_send_part(const char *);
		void irc_send_whois(const char *);
		void irc_send_away(void);
		void irc_send_away(const char *);
		void irc_send_invite(const char *, const char *);
		void irc_send_kick(const char *, const char *);
		void irc_send_kick(const char *, const char *, const char *);
		void irc_send_mode(const char *);
		void irc_send_mode(const char *, const char *);
		void irc_send_mode(const char *, const char *, const char *);
        void irc_send_list(void);
        void irc_send_list(const char *);
        void irc_send_who(const char *);

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

		void (*default_link_function)(const irc_msg_data *,
			void *);

		int irc_write_message_f(int, const char *, const char *, ...);
		int irc_send_message(const char *);
		int irc_receive_message(char *, size_t);

		void irc_send_message_f(const char *, ...);
		char *irc_split_server_message(char *);
		void irc_parse_irc_message(char *);
};

#endif /* _IRCSOCKET_H_ */

