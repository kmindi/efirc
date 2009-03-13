#include "ircinterface.h"

/****************************
 *
 * I R C   S O C K E T
 *
 ****************************/

/* Constructor (connect to server) */
IRCInterface::IRCInterface(unsigned int port, const char *server,
	const char *nick, const char *user, const char *real,
	const char *pass, const char *log_path, int log_level)
{
	#ifdef WIN32 /* see msdn WSAStartup */
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	#endif

	/* no links and commands in queue  on startup - hopefully */
	links = 0;
	cmds = 0;

	/* no queue, no links */
	abp = NULL;
	atp = NULL;
	cbp = NULL;
	ctp = NULL;

	/* function to be called when no matching link can be found */
	default_link_function = NULL;

	/* yet no */
	connected = 0;
	disconnected = 0;
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
	_DBGLEVEL = log_level;
	_DBGRECON = 1;

	_DBGSTR = fopen(log_path, "w");
	if(_DBGSTR == NULL) {
		_DBGSTR = stdout;

		irc_write_message_f(3, "IRCInterface", "Couldn't open stream. (%s)\n",
			strerror(errno));
	}

	#ifdef WIN32
	err = WSAStartup(wVersionRequested, &wsaData);

	if(err != 0)
		irc_write_message_f(3, "IRCInterface", "WSAStartup failed. (%d)\n",
			err);
	#endif
}

/* Destructor (disconnect from server) */
IRCInterface::~IRCInterface()
{
	/* disconnect from IRC server by sending QUIT */
	if(connected)
		irc_disconnect_server();

	/* close socket */
	#ifdef WIN32
	if(shutdown(sock, 2) == SOCKET_ERROR)
		irc_write_message_f(3, "~IRCInterface", "Couldn't close"
			" socket. (%s)\n", WSAGetLastError());

	WSACleanup();
	#else
	if(close(sock) == -1)
		irc_write_message_f(3, "~IRCInterface", "Couldn't close"
			" socket. (%s)\n", strerror(errno));
	#endif

	if(fclose(_DBGSTR) == -1)
		irc_write_message_f(3, "~IRCInterface", "Couldn't close"
			" stream. (%s)\n", strerror(errno));
}

/*
 * add new command on IRC socket to queue
 *
 * buf      : parameter the function is given
 * function : function to be called
 */
void
IRCInterface::irc_add_command_queue_entry(int (IRCInterface::*function)(const char *),
	const char *function_parameter)
{
	/* buffer legnth */
	int l;

	/* pointing to new cmd in queue */
	irc_queue_cmd *ntp;

	/* struct with command and arguments */
	ntp = new irc_queue_cmd;

	l = strlen(function_parameter) + 1;

	/* new command on top */
	ntp->function = function;
	ntp->buf = new char[l];
	/* should be string + \0 */
	strlcpy(ntp->buf, function_parameter, l);
	ntp->next = NULL;

	/* new top/bottom */
	if(cmds > 0) {
		ctp->next = ntp;
		ctp = ctp->next;
	} else {
		ctp = ntp;
		cbp = ctp;
	}

	/* print notice (command and args) */
	irc_write_message_f(1, "irc_add_command_queue_entry", "New command in queue."
		"(%i, %s, %i)\n", cmds, function_parameter, function);

	/* one more */
	cmds++;
}

/* remove current command from queue */
void
IRCInterface::irc_delete_command_queue_entry(void)
{
	char *buf;
	int (IRCInterface::*function)(const char *buf);
	/* NEW botton item */
	irc_queue_cmd *nbp;

	irc_write_message_f(0, "irc_delete_command_queue_entry", "Removing command.\n");

	/*
	 * DOES command exist? (already checked when called by
	 * call_cmd()!)
	 */
	if(cmds > 0) {
		nbp = cbp->next;
		buf = cbp->buf;
		function = cbp->function;

		irc_write_message_f(1, "irc_delete_command_queue_entry", "Will be removed. (%s, %i)\n",
			buf, function);

		/* NOW removing command in queue (1.) */
		delete[] cbp->buf;
		delete cbp;

		/* setting new bottom (may not exist) */
		cbp = nbp;

		/* one less */
		cmds--;
	} else
		/* no match found */
		irc_write_message_f(1, "irc_delete_command_queue_entry", "No more commands in"
			" queue.\n");
}

/* call current command in queue */
void
IRCInterface::irc_call_command_queue_entries(void)
{
	int (IRCInterface::*function)(const char *buf), s;
	const char *buf;

	irc_write_message_f(0, "irc_call_command_queue_entry", "Calling commands.\n");

	while(!disconnected || cmds > 0) {
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
				irc_write_message_f(1, "irc_call_command_queue_entry", "Called."
					" (%s, %i)\n", buf, function);

				irc_delete_command_queue_entry();
			} else { /* trying reconnect */
				if(!_DBGRECON) {
					irc_write_message_f(3, "irc_call_command_queue_entry",
						"Reconnecting"
						" disabled.\n");
					break;
				}

				irc_write_message_f(3, "irc_call_command_queue_entry", "Requesting"
					" reconnect.\n");

				irc_reconnect_server();
			}
		}

		/* don't hurry */
		#ifdef WIN32
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
IRCInterface::irc_add_link_queue_entry(const char *irc_command,
	void (*function)(const irc_msg_data *, void *))
{
	/* command length */
	int l;

	/* new link on top */
	irc_act_link *ntp;

	ntp = new struct irc_act_link;

	l = strlen(irc_command) + 1;

	ntp->function = function;
	ntp->cmd = new char[l];
	strlcpy(ntp->cmd, irc_command, l);
	ntp->next = NULL;

	/* pointer to bottom/top */
	if(links > 0) {
		atp->next = ntp;
		atp = atp->next;
	} else {
		atp = ntp;
		abp = atp;
	}

	/* maybe some debugging needed...this time we trust you */
	irc_write_message_f(1, "irc_add_link_queue_entry", "New link added. (%i, %s, %i)\n",
		links, irc_command, function);

	/* one more link */
	links++;
}

/* unlink */
void
IRCInterface::irc_delete_link_queue_entry(const char *irc_command,
	void (*function)(const irc_msg_data *, void *))
{
	irc_act_link *cp;
	irc_act_link *pp;
	irc_act_link *np;

	irc_write_message_f(0, "irc_delete_link_queue_entry", "Deleting link. (%s, %i)\n",
		irc_command, function);

	/* start at botton, let pp not be uninitialized */
	cp = abp;
	pp = cp;
// TODO there may be more than one matching link (see also act_link()
	/* link will be removed, but first search for it */
	while(cp != NULL) {
		if(!strcmp(cp->cmd, irc_command) && cp->function == function) {
			np = cp->next;

			irc_write_message_f(1, "irc_delete_link_queue_entry", "Will be deleted.\n");

			delete cp->cmd;
			delete cp;

			if(pp != abp)
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
	irc_write_message_f(3, "irc_delete_link_queue_entry", "No such link.\n");
}

/* call function for event */
void
IRCInterface::irc_call_link_queue_entry(const irc_msg_data *irc_message_data)
{
	int s;
	irc_act_link *cp;

	irc_write_message_f(0, "irc_call_link_queue_entry", "Activating link. (%s)\n",
		irc_message_data->cmd);

	/* no matching function found yet */
	s = 0;
	/* beginning at bottom */
	cp = abp;

	while(cp != NULL) {
		/* found match? - act link */
		if(!strcmp(cp->cmd, irc_message_data->cmd)) {
			s = 1;

			/*
			 * call function with specified parameters, such
			 * as pointer to us
			 */
			cp->function(irc_message_data, this);

			irc_write_message_f(1, "irc_call_link_queue_entry", "Activated. (%i)\n",
				cp->function);
		}

		/* going on */
		cp = cp->next;
	}

	if(s == 0 && default_link_function != NULL)
	{
		default_link_function(irc_message_data, this);

		irc_write_message_f(1, "irc_call_link_queue_entry", "Activated default. (%i)\n",
			default_link_function);
	}

	/* carefully */
	irc_write_message_f(1, "irc_call_link_queue_entry", "No (more?) links.\n");
}

void
IRCInterface::irc_set_default_link_function(void (*function)(
const irc_msg_data *, void *))
{
	default_link_function = function;
}

/* cut IRC reply */
char *
IRCInterface::irc_parse_server_message(const char *server_message)
{
	/* substring */
	char *sstr;
	/* working with it */
	char *wstr;

	wstr = (char *)server_message;

	irc_write_message_f(0, "irc_parse_server_message", "Splitting reply into messages.\n");

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

			irc_write_message_f(1, "irc_parse_server_message", "Found relevant"
				" messagepart.\n");

			irc_parse_irc_message(wstr);

			/* increment pointer for another search */
			wstr = sstr + 2;
		} else
			irc_write_message_f(1, "irc_parse_server_message", "No (further?) relevant"
				" messagepart.\n");
	} while(sstr != NULL);

	/* return adress in memory where ignored message part starts */
	return wstr;
}

/* parse IRC reply */
void
IRCInterface::irc_parse_irc_message(char *irc_message)
{
	int i;
	size_t l;
	char *w, *s;

	/* struct containing sender, command and parameters */
	irc_msg_data *msg_data = new irc_msg_data;

	w = (char *)irc_message;

	irc_write_message_f(3, "irc_parse_irc_message", "Parsing message. (%s)\n", irc_message);

	if(*w == ':') {
		irc_write_message_f(1, "irc_parse_irc_message", "Is server or user"
			" command.\n");

		w++;

		strsub(&msg_data->sender, &w, ' ');

		irc_write_message_f(1, "irc_parse_irc_message", "Sender is: %s\n",
			msg_data->sender);

		strsub(&msg_data->cmd, &w, ' ');

		irc_write_message_f(1, "irc_parse_irc_message", "Command is: %s\n",
			msg_data->cmd);

		strsub(&msg_data->params, &w, '\0');

		irc_write_message_f(1, "irc_parse_irc_message", "Parameters are: %s\n",
			msg_data->params);

		irc_write_message_f(1, "irc_parse_irc_message", "Parsing sender.\n");

		w = msg_data->sender;
		strsub(&msg_data->nick, &w, '!');
		if(w == NULL) { /* is server command */
			msg_data->user = new char[1];
			msg_data->host = new char[1];

			*msg_data->user = '\0';
			*msg_data->host = '\0';

			irc_write_message_f(1, "irc_parse_irc_message", "Server is: %s\n",
				msg_data->sender);
		} else {
			irc_write_message_f(1, "irc_parse_irc_message", "Nickname is: %s\n",
				msg_data->nick);

			strsub(&msg_data->user, &w, '@');

			irc_write_message_f(1, "irc_parse_irc_message", "Username is: %s\n",
				msg_data->user);

			strsub(&msg_data->host, &w, '\0');

			irc_write_message_f(1, "irc_parse_irc_message", "Hostname is: %s\n",
				msg_data->host);
		}
	} else {
		irc_write_message_f(1, "irc_parse_irc_message", "Is server message.\n");

// TODO server we're really connected to
		l = strlen("SERVER") + 1;
		msg_data->sender = new char[l];
		strlcpy(msg_data->sender, "SERVER", l);

		strsub(&msg_data->cmd, &w, ' ');

		irc_write_message_f(1, "irc_parse_irc_message", "Command is: %s\n",
			msg_data->cmd);

		strsub(&msg_data->params, &w, '\0');

		irc_write_message_f(1, "irc_parse_irc_message", "Parameters are: %s\n",
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

	irc_write_message_f(1, "irc_parse_irc_message", "Parsing parameters."
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
			s = ++w;

		l = strlen(s) + 1;

		msg_data->params_a[i] = new char[l];
		strlcpy(msg_data->params_a[i], s, l);
	}

	if(w != NULL)
		while(w != msg_data->params)
			if(*--w == '\0') *w = ' ';

	/* pass msg data and call event matching functions */
	irc_call_link_queue_entry(msg_data);

	/* free allocated mem */
	delete[] msg_data->sender;
	delete[] msg_data->cmd;
	delete[] msg_data->params;
	for(i = 0; i < msg_data->params_i; i++)
		delete[] msg_data->params_a[i];
	delete[] msg_data->params_a;
	delete[] msg_data->host;
	delete[] msg_data->nick;
	delete[] msg_data->user;
	delete msg_data;
}

