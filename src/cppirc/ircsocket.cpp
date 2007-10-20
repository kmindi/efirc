#include "cppirc/ircsocket.h"
#include "irccommands.cpp"
#include "tcpip.cpp"
#include "io.cpp"

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
	strncpy(_IRCSERV, server, W_BUFSIZE - 1);
	_IRCSERV[W_BUFSIZE - 1] = '\0';
	strncpy(_IRCNICK, nick,   W_BUFSIZE - 1);
	_IRCNICK[W_BUFSIZE - 1] = '\0';
	strncpy(_IRCUSER, user,   W_BUFSIZE - 1);
	_IRCUSER[W_BUFSIZE - 1] = '\0';
	strncpy(_IRCREAL, real,   W_BUFSIZE - 1);
	_IRCREAL[W_BUFSIZE - 1] = '\0';
	strncpy(_IRCPASS, pass,   W_BUFSIZE - 1);
	_IRCPASS[W_BUFSIZE - 1] = '\0';
	_DBGSTR = log;
	_DBGSPACE = 15;
	_DBGLEVEL = 3;
	_DBGRECON = 1;
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
	#else
	close(sock);
	#endif
}

/*
 * add new command on IRC socket to queue
 *
 * function : command on socket
 */
void
IRCSocket::add_cmd(int (IRCSocket::*function)(const char *buf),
   const char *buf)
{
	/* buffer legnth */
	int length;

	/* pointing to new cmd in queue */
	irc_queue_cmd *ntp;

	/* struct with command and arguments */
	ntp = new irc_queue_cmd;

	length = strlen(buf);

	/* new command on top */
	ntp->function = function;
	ntp->buf = new char[length + 1];
	/* should be string + \0 */
	strncpy(ntp->buf, buf, length + 1);
	ntp->next = 0;

	/* new top/bottom */
	if(cmds > 0)
	{
		ctp->next = ntp;
		ctp = ctp->next;
	}
	else
	{
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
void IRCSocket::del_cmd(void)
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
	if(cmds > 0)
	{
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
	}
	else
		/* no match found */
		debug(1, "del_cmd", "No more command in"
		   " queue.\n");
}

/* call current command in queue */
void IRCSocket::call_cmd(void)
{
	int (IRCSocket::*function)(const char *buf), status;
	const char *buf;

	while(1)
	{
		debug(0, "call_cmd", "Calling command.\n");

		/* any commands left in queue? */
		if(cmds > 0)
		{
			/*
			 * if we do NOT care called function
			 * will call reconnect() every single
			 * time
			 */
			if(!reconnecting)
			{
				buf = cbp->buf;
				function = cbp->function;

				/* actual call, preserve return code */
				status = (this->*function)(buf);

				/* now, after successfull run, remove it from the queue */
				if(status > -1)
				{
					debug(1, "call_cmd", "Called. (%s, %i)\n",
					   buf, function);

					del_cmd();
				}
				/* trying reconnect */
				else
				{
					debug(3, "call_cmd", "Requesting "
					   "reconnect.\n");

					reconnect();
				}
			}
		}
		else
			/* minor log-level */
			debug(0, "call_cmd", "No more command in"
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
	int length;

	/* new link on top */
	irc_act_link *ntp;

	ntp = new struct irc_act_link;

	length = strlen(cmd);

	ntp->function = function;
	ntp->cmd = new char[length + 1];
	strncpy(ntp->cmd, cmd, length + 1);
	ntp->next = 0;

	/* pointer to bottom/top */
	if(links > 0)
	{
		atp->next = ntp;
		atp = atp->next;
	}
	else
	{
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
	for(i = 0; i < links; i++)
	{
		if(!strcmp(cp->cmd, cmd) && cp->function == function)
		{
			np = cp->next;

			debug(1, "del_link", "Will be deleted. (%i)\n", i);

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

	for(i = 0; i < links; i++)
	{
		/* found match? - act link */
		if(!strcmp(cp->cmd, msg_data->cmd))
		{
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
IRCSocket::parse(const char *raw_msg)
{
	/* substring */
	char *sstr;
	/* working with it */
	char *wstr;

	wstr = (char *)raw_msg;

	debug(0, "parse", "Splitting reply into messages.\n");

	do
	{
		/* the raw message may needs to be split up */
		sstr = strstr(wstr, "\r\n");

		/* it needs to */
		if(sstr != NULL)
		{
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
		}
		else
			debug(1, "parse", "No (further?) relevant"
			   " messagepart.\n");
	} while(sstr != NULL);

	/* return adress in memory where ignored message part starts */
	return wstr;
}

/* parse IRC reply */
void
IRCSocket::parse_msg(const char *raw_msg)
{
	int i, params, length, text, p_length;
	/* se substring */
	char *sstr;
	char *wstr;

	/* number of parameters (excluding `\s:.*$') */
	params = 0;
	/* length of resulting parameters join */
	p_length = 0;
	/* specifies wheter a string is found in parameters-list */
	text = 0;

	/* struct containing sender, command and parameters */
	irc_msg_data *msg_data = new irc_msg_data;

	debug(3, "parse_msg", "Parsing message. (%s)\n",
	   raw_msg);

	/* found user/server command */
	if(raw_msg[0] == ':')
	{
		debug(1, "parse_msg", "Is server or user"
		   " command.\n");

		raw_msg++;

		/* sender */
		sstr = strstr(raw_msg, " ");
		if(sstr != NULL)
		{
			*sstr = '\0';

			length = strlen(raw_msg);

			msg_data->sender = new char[length + 1];
			strncpy(msg_data->sender, raw_msg, length + 1);

			debug(1, "parse_msg", "Found sender."
			   " (%s)\n", msg_data->sender);

			raw_msg = sstr + 1;
		}
		else
		{
			length = strlen("");

			msg_data->sender = new char[length + 1];
			strncpy(msg_data->sender, "", length + 1);

			debug(1, "parse_msg", "No sender.\n");
		}

		/* command */
		sstr = strstr(raw_msg, " ");
		if(sstr != NULL)
		{
			*sstr = '\0';

			length = strlen(raw_msg);

			msg_data->cmd = new char[length + 1];
			strncpy(msg_data->cmd, raw_msg, length + 1);

			debug(1, "parse_msg", "Found command."
			   " (%s)\n", msg_data->cmd);

			raw_msg = sstr + 1;
		}
		else
		{
			length = strlen("");

			msg_data->cmd = new char[length + 1];
			strncpy(msg_data->cmd, "", length + 1);

			debug(1, "parse_msg", "No command.\n");
		}

		length = strlen(raw_msg);

		/* parameters */
		msg_data->params = new char[length + 1];
		strncpy(msg_data->params, raw_msg, length + 1);

		debug(1, "parse_msg", "Found parameters."
		   " (%s)\n", msg_data->params);

		/* nice format */
		/* for sender */
		wstr = msg_data->sender;

		debug(1, "parse_msg", "Parsing sender."
		   " (%s)\n", wstr);

		sstr = strstr(wstr, "!");
		if(sstr != NULL)
		{
			*sstr = '\0';

			length = strlen(wstr);

			msg_data->nick = new char[length + 1];
			strncpy(msg_data->nick, wstr, length + 1);
			wstr = sstr + 1;

			debug(1, "parse_msg", "Found nickname."
			   " (%s)\n", msg_data->nick);

			sstr += 1;
		}
		else
		{
			length = strlen("");

			msg_data->nick = new char[length + 1];
			strncpy(msg_data->nick, "", length + 1);

			debug(1, "parse_msg", "No nick.\n");
		}

		sstr = strstr(wstr, "@");
		if(sstr != NULL)
		{
			*sstr = '\0';

			length = strlen(wstr);

			msg_data->user = new char[length + 1];
			strncpy(msg_data->user, wstr, length + 1);
			wstr = sstr + 1;

			debug(1, "parse_msg", "Found username."
			   " (%s)\n", msg_data->user);

			sstr += 1;
		}
		else
		{
			length = strlen("");

			msg_data->user = new char[length + 1];
			strncpy(msg_data->user, "", length + 1);

			debug(1, "parse_msg", "No user.\n");
		}

		length = strlen(wstr);

		msg_data->host = new char[length + 1];
		strncpy(msg_data->host, wstr, length + 1);

		debug(1, "parse_msg", "Found host."
		   " (%s)\n", msg_data->host);

		/* not usable anymore - set to "USER" */
		delete msg_data->sender;
		length = strlen(msg_data->nick) + strlen(msg_data->user) +
		   strlen(msg_data->host);
		msg_data->sender = new char[length + 1];
		snprintf(msg_data->sender, length + 1, "%s%s%s",
		   msg_data->nick, msg_data->user, msg_data->host);
	}
	/* we have a server message */
	else
	{
		debug(1, "parse_msg", "Is server message.\n");

		length = strlen("SERVER");

// TODO server we're really connected to
		msg_data->sender = new char[length + 1];
		strncpy(msg_data->sender, "SERVER", length + 1);

		/* command */
		sstr = strstr(raw_msg, " ");
		if(sstr != NULL)
		{
			*sstr = '\0';

			length = strlen(raw_msg);

			msg_data->cmd = new char[length + 1];
			strncpy(msg_data->cmd, raw_msg, length + 1);

			debug(1, "parse_msg", "Found command."
			   " (%s)\n", msg_data->cmd);

			raw_msg = sstr + 1;
		}
		else
		{
			length = strlen("");

			msg_data->cmd = new char[length + 1];
			strncpy(msg_data->cmd, "", length + 1);

			debug(1, "parse_msg", "No command.\n");
		}

		length = strlen(raw_msg);

		/* parameters */
		msg_data->params = new char[length + 1];
		strncpy(msg_data->params, raw_msg, length + 1);

		debug(1, "parse_msg", "Found parameters."
		   " (%s)\n", msg_data->params);

		/* safely :/ */
		length = strlen("");
		msg_data->host = new char[length + 1];
		strncpy(msg_data->host, "", length + 1);
		length = strlen("");
		msg_data->nick = new char[length + 1];
		strncpy(msg_data->nick, "", length + 1);
		length = strlen("");
		msg_data->user = new char[length + 1];
		strncpy(msg_data->user, "", length + 1);
	}

	/* for params */
	wstr = msg_data->params;

	debug(1, "parse_msg", "Parsing parameters."
	   " (%s)\n", wstr);

	/* get number of nontext params */
	do
	{
		/* found parameter */
		if(*wstr == ':')
		{
			/* for string set text-bit and stop here */
			params++;
			text = 1;
			break;
		}
		else if((sstr = strstr(wstr, " ")) != NULL)
		{
			params++;
			wstr = sstr + 1;
		}
		/* only one paramter */
		else if(params == 0)
			params++;
	} while(sstr != NULL);

	/* allocate memory for array with index fields */
	msg_data->params_a = new char *[params];

	/* jump back */
	wstr = msg_data->params;

	/* fill in array */
	for(i = 0; i < params; i++)
	{
		/* no string */
		if(*wstr != ':')
		{
			sstr = strstr(wstr, " ");

			/* only one paramter */
			if(sstr != NULL)
				*sstr = '\0';
		}
		/* string - skip ':' */
		else
			wstr++;

		length = strlen(wstr);

		msg_data->params_a[i] = new char[length + 1];
		strncpy(msg_data->params_a[i], wstr, length + 1);

		wstr = sstr + 1;
	}

	/* parameters are rebuilt */
	delete msg_data->params;

	/* get full length including separators */
	for(i = 0; i < params; i++)
		p_length += strlen(msg_data->params_a[i]) + 1;

	/* allocate and clear (?) */
	msg_data->params = new char[p_length + 1];
	memset(msg_data->params, '\0', p_length + 1);

	/* join parameters */
	for(i = 0; i < params; i++)
	{
		/*
		 * if last parameter is a string
		 * prefix it with a ':'
		 */
		if(text && i == params - 1)
			strncat(msg_data->params, ":", 1);

		strncat(msg_data->params, msg_data->params_a[i],
		   strlen(msg_data->params_a[i]));

		/*
		 * if not last parameter cat space
		 * as seperator
		 */
		if(i != params - 1)
			strncat(msg_data->params, " ", 1);
	}

	/* pass msg data and call event matching functions */
	act_link(msg_data);

	/* free allocated mem */
	delete msg_data->sender;
	delete msg_data->cmd;
	delete msg_data->params;
	for(i = 0; i < params; i++)
		delete msg_data->params_a[i];
	delete[] msg_data->params_a;
	delete msg_data->host;
	delete msg_data->nick;
	delete msg_data->user;
	delete msg_data;
}

