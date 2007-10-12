// TODO errorhandler for ^C
//      malloc()-use
//      strtok()
//      strncpy/snprintf/strncat (really written -> error)
//      start function
//      sizeof

#ifndef __IRCSOCKET_H__
#define __IRCSOCKET_H__

#ifdef WINDOWS
#include <windows.h>
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
#define R_BUFSIZE 512

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
		   const char *, const char *, const char *, FILE *);
		~IRCSocket(void);

		unsigned int connected;
		FILE *_DBGSTR;
		unsigned int _DBGSPACE;
		unsigned int _DBGLEVEL;
		unsigned int _DBGRECON;
		unsigned int _IRCPORT;
		char _IRCSERV[W_BUFSIZE];
		char _IRCNICK[W_BUFSIZE];
		char _IRCUSER[W_BUFSIZE];
		char _IRCREAL[W_BUFSIZE];
		char _IRCPASS[W_BUFSIZE];

		void add_link(const char *,
		   void (*function)(const irc_msg_data*, void*));
		void del_link(const char *,
		   void (*function)(const irc_msg_data*, void*));
		void act_link(const irc_msg_data*);

		void add_cmd(int (IRCSocket::*function)(const char *),
		   const char *);
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

		int debug(unsigned int, const char *, const char *, ...);
		int sock_send(const char *);
		int sock_recv(char *);

		void send_raw(char *, ...);
		char *parse(const char *);
		void parse_msg(const char *);
};

#endif

