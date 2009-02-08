#include "ircsocket.h"

/****************************
 *
 * I R C   S O C K E T
 *
 ****************************/

/* Constructor (connect to server) */
IRCSocket::IRCSocket(unsigned int port, const char *server,
	const char *nick, const char *user, const char *real,
	const char *pass, FILE *log)
{
	#ifdef WINDOWS /* see msdn WSAStartup */
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	#endif

	/* no links and commands in queue  on startup - hopefully */
	links = 0;
	cmds = 0;

	/* no queue, no links */
	abp = 0;
	atp = 0;
	cbp = 0;
	ctp = 0;

	/* yet no */
	connected = 0;
	reconnecting = 0;
	connecting = 0;
	/* be careful! just telling wheter auth() has been called */
	authed = 0;

	/* no great sleep_time needed */
	sleep_time = 1;

	/* set defaults */
	_IRCPORT = port;
	strlcpy(_IRCSERV, server, sizeof(_IRCSERV));
// TODO server defaults on connect
	strlcpy(_IRCNICK, nick, sizeof(_IRCNICK));
	strlcpy(_IRCUSER, user, sizeof(_IRCUSER));
	strlcpy(_IRCREAL, real, sizeof(_IRCREAL));
	strlcpy(_IRCPASS, pass, sizeof(_IRCPASS));
	_DBGSTR = log;
	_DBGSPACE = 15;
	_DBGLEVEL = 3;
	_DBGRECON = 1;

	#ifdef WINDOWS
	err = WSAStartup(wVersionRequested, &wsaData);

	if(err != 0)
		debug(3, "IRCSocket", "WSAStartup failed. (%d)\n", err);
	#endif
}

/* Destructor (disconnect from server) */
IRCSocket::~IRCSocket()
{
	/* disconnect from IRC server by sending QUIT */
	if(connected)
		disconnect_server("");

	/* close socket */
	#ifdef WINDOWS
	shutdown(sock, 2);

	WSACleanup();
	#else
	close(sock);
	#endif
}

/*
 * add new command on IRC socket to queue
 *
 * buf      : parameter the function is given
 * function : function to be called
 */
void
IRCSocket::add_cmd(const char *buf,
	int (IRCSocket::*function)(const char *buf))
{
	/* buffer legnth */
	int l;

	/* pointing to new cmd in queue */
	irc_queue_cmd *ntp;

	/* struct with command and arguments */
	ntp = new irc_queue_cmd;

	l = strlen(buf) + 1;

	/* new command on top */
	ntp->function = function;
	ntp->buf = new char[l];
	/* should be string + \0 */
	strlcpy(ntp->buf, buf, l);
	ntp->next = 0;

	/* new top/bottom */
	if(cmds > 0) {
		ctp->next = ntp;
		ctp = ctp->next;
	} else {
		ctp = ntp;
		cbp = ctp;
	}

	/* print notice (command and args) */
	debug(1, "add_cmd", "New command in queue."
		"(%i, %s, %i)\n", cmds, buf, function);

	/* one more */
	cmds++;
}

/* remove current command from queue */
void
IRCSocket::del_cmd(void)
{
	char *buf;
	int (IRCSocket::*function)(const char *buf);
	/* NEW botton item */
	irc_queue_cmd *nbp;

	debug(0, "del_cmd", "Removing command.\n");

	/*
	 * DOES command exist? (already checked when called by
	 * call_cmd()!)
	 */
	if(cmds > 0) {
		nbp = cbp->next;
		buf = cbp->buf;
		function = cbp->function;

		debug(1, "del_cmd", "Will be removed. (%s, %i)\n",
			buf, function);

		/* NOW removing command in queue (1.) */
		delete cbp->buf;
		delete cbp;

		/* setting new bottom (may not exist) */
		cbp = nbp;

		/* one less */
		cmds--;
	} else
		/* no match found */
		debug(1, "del_cmd", "No more commands in"
			" queue.\n");
}

/* call current command in queue */
void
IRCSocket::call_cmd(void)
{
	int (IRCSocket::*function)(const char *buf), s;
	const char *buf;

	while(1) {
		debug(0, "call_cmd", "Calling command.\n");

		/* any commands left in queue? */
		if(cmds > 0) {
			buf = cbp->buf;
			function = cbp->function;

			/* actual call, preserve return code */
			s = (this->*function)(buf);

			/*
			 * now, after successfull run, remove it from
			 * the queue
			 */
			if(s > -1) {
				debug(1, "call_cmd", "Called."
					" (%s, %i)\n", buf, function);

				del_cmd();
			} else { /* trying reconnect */
				if(!_DBGRECON) {
					debug(3, "call_cmd",
						"Reconnecting"
						" disabled.\n");
					break;
				}

				debug(3, "call_cmd", "Requesting"
					" reconnect.\n");

				reconnect();
			}
		} else
			/* minor log-level */
			debug(0, "call_cmd", "No more commands in"
				" queue.\n");

		/* don't hurry */
		#ifdef WINDOWS
		Sleep(10);
		#else
		usleep(10);
		#endif
	}
}

/*
 * link event and function
 *
 * cmd      : IRC command to wait for
 * function : will be called when command
 *            arrives
 */
void
IRCSocket::add_link(const char *cmd,
	void (*function)(const irc_msg_data*, void*))
{
	/* command length */
	int l;

	/* new link on top */
	irc_act_link *ntp;

	ntp = new struct irc_act_link;

	l = strlen(cmd) + 1;

	ntp->function = function;
	ntp->cmd = new char[l];
	strlcpy(ntp->cmd, cmd, l);
	ntp->next = 0;

	/* pointer to bottom/top */
	if(links > 0) {
		atp->next = ntp;
		atp = atp->next;
	} else {
		atp = ntp;
		abp = atp;
	}

	/* maybe some debugging needed...this time we trust you */
	debug(1, "add_link", "New link added. (%i, %s, %i)\n",
		links, cmd, function);

	/* one more link */
	links++;
}

/* unlink */
void
IRCSocket::del_link(const char *cmd,
	void (*function)(const irc_msg_data*, void*))
{
	unsigned int i;
	irc_act_link *cp;
	irc_act_link *pp;
	irc_act_link *np;

	debug(0, "del_link", "Deleting link. (%s, %i)\n",
		cmd, function);

	/* start at botton */
	cp = abp;
// TODO there may be more than one matching link (see also act_link()
	/* link will be removed, but first search for it */
	for(i = 0; i < links; i++) {
		if(!strcmp(cp->cmd, cmd) && cp->function == function) {
			np = cp->next;

			debug(1, "del_link", "Will be deleted."
				" (%i)\n", i);

			delete cp->cmd;
			delete cp;

			if(i > 0)
				pp->next = np;
			else
				abp = np;

			/* one less */
			links--;
			/* break out */
			return;
		}

		/* previous link */
		pp = cp;
		/* next on */
		cp = cp->next;
	}

	/* no match found */
	debug(3, "del_link", "No such link.\n");
}

/* call function for event */
void
IRCSocket::act_link(const irc_msg_data *msg_data)
{
	unsigned int i;
	irc_act_link *cp;

	debug(0, "act_link", "Activating link. (%s)\n",
		msg_data->cmd);

	/* beginning at bottom */
	cp = abp;

	for(i = 0; i < links; i++) {
		/* found match? - act link */
		if(!strcmp(cp->cmd, msg_data->cmd)) {
			/*
			 * call function with specified parameters, such
			 * as pointer to us
			 */
			cp->function(msg_data, this);

			debug(1, "act_link", "Activated. (%i)\n",
				cp->function);
		}

		/* going on */
		cp = cp->next;
	}

	/* carefully */
	debug(1, "act_link", "No (more?) links.\n");
}

/* cut IRC reply */
char *
IRCSocket::parse(char *msg)
{
	/* substring */
	char *sstr;
	/* working with it */
	char *wstr;

	wstr = (char *)msg;

	debug(0, "parse", "Splitting reply into messages.\n");

	do {
		/* the raw message may needs to be split up */
		sstr = strstr(wstr, "\r\n");

		/* it needs to */
		if(sstr != NULL) {
			/*
			 * cut message (null-terminated at first
			 * occurrence)
			 */
			*sstr = '\0';

			debug(1, "parse", "Found relevant"
				" messagepart.\n");

			parse_msg(wstr);

			/* increment pointer for another search */
			wstr = sstr + 2;
		} else
			debug(1, "parse", "No (further?) relevant"
				" messagepart.\n");
	} while(sstr != NULL);

	/* return adress in memory where ignored message part starts */
	return wstr;
}

/* parse IRC reply */
void
IRCSocket::parse_msg(char *msg)
{
	int i, l;
	char *w, *s;

	/* struct containing sender, command and parameters */
	irc_msg_data *msg_data = new irc_msg_data;

	w = (char *)msg;

	debug(3, "parse_msg", "Parsing message. (%s)\n", msg);

	if(*w == ':') {
		debug(1, "parse_msg", "Is server or user"
			" command.\n");

		w++;

		cpsub(&msg_data->sender, &w, ' ');

		debug(1, "parse_msg", "Sender is: %s\n",
			msg_data->sender);

		cpsub(&msg_data->cmd, &w, ' ');

		debug(1, "parse_msg", "Command is: %s\n",
			msg_data->cmd);

		cpsub(&msg_data->params, &w, '\0');

		debug(1, "parse_msg", "Parameters are: %s\n",
			msg_data->params);

		debug(1, "parse_msg", "Parsing sender.\n");

		w = msg_data->sender;
		cpsub(&msg_data->nick, &w, '!');
		if(w == NULL) { /* is server command */
			msg_data->nick = new char[1];
			msg_data->user = new char[1];
			msg_data->host = new char[1];

			*msg_data->nick = '\0';
			*msg_data->user = '\0';
			*msg_data->host = '\0';

			debug(1, "parse_msg", "Server is: %s\n",
				msg_data->sender);
		} else {
			debug(1, "parse_msg", "Nickname is: %s\n",
				msg_data->nick);

			cpsub(&msg_data->user, &w, '@');

			debug(1, "parse_msg", "Username is: %s\n",
				msg_data->user);

			cpsub(&msg_data->host, &w, '\0');

			debug(1, "parse_msg", "Hostname is: %s\n",
				msg_data->host);
		}
	} else {
		debug(1, "parse_msg", "Is server message.\n");

// TODO server we're really connected to
		l = strlen("SERVER") + 1;
		msg_data->sender = new char[l];
		strlcpy(msg_data->sender, "SERVER", l);

		cpsub(&msg_data->cmd, &w, ' ');

		debug(1, "parse_msg", "Command is: %s\n",
			msg_data->cmd);

		cpsub(&msg_data->params, &w, '\0');

		debug(1, "parse_msg", "Parameters are: %s\n",
			msg_data->params);

		msg_data->host = new char[1];
		msg_data->nick = new char[1];
		msg_data->user = new char[1];

		*msg_data->host = '\0';
		*msg_data->nick = '\0';
		*msg_data->user = '\0';
	}

	/* number of parameters */
	msg_data->params_i = 0;
	w = msg_data->params;

	debug(1, "parse_msg", "Parsing parameters."
		" (%s)\n", msg_data->params);

	/* get the number of parameters not containing text */
	do {
		/* from now there will be text */
		if(*w == ':') {
			/* for string set text-bit and stop here */
			msg_data->params_i++;
			break;
		} else if((s = strstr(w, " ")) != NULL) {
			msg_data->params_i++;
			w = s + 1;
		} else if(msg_data->params_i == 0) /* one parameter */
			msg_data->params_i++;
	} while(s != NULL);

	/* allocate memory for array with index fields */
	msg_data->params_a = new char *[msg_data->params_i];

	w = msg_data->params;
	for(i = 0; i < msg_data->params_i; i++) {
		/*
		 * check whether this parameter is a string and skip ':'
		 * before a string
		 */
		if(*w != ':')
			s = strsep(&w, " ");
		else
			s = w++;

		l = strlen(s) + 1;

		msg_data->params_a[i] = new char[l];
		strlcpy(msg_data->params_a[i], s, l);
	}

	while(w != msg_data->params)
		if(*--w == '\0') *w = ' ';

	/* pass msg data and call event matching functions */
	act_link(msg_data);

	/* free allocated mem */
	delete msg_data->sender;
	delete msg_data->cmd;
	delete msg_data->params;
	for(i = 0; i < msg_data->params_i; i++)
		delete msg_data->params_a[i];
	delete[] msg_data->params_a;
	delete msg_data->host;
	delete msg_data->nick;
	delete msg_data->user;
	delete msg_data;
}

void
IRCSocket::cpsub(char **dst, char **src, char sep)
{
	int l;
	char *t;
	char delim[2];

	snprintf(delim, sizeof(delim), "%c", sep);

	t = strsep(src, delim);

	if((*src != NULL || sep == '\0') && t != NULL) {
		l = strlen(t) + 1;
		*dst = new char[l];
		strlcpy(*dst, t, l);

		if(sep != '\0') *(--*src)++ = sep;
	} else {
		*dst = new char[1];
		**dst = '\0';
	}
}

