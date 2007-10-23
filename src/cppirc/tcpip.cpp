/****************************
 *
 * T C P / I P
 *
 ****************************/

/* connect server:port */
void
IRCSocket::connect_server(unsigned int port, const char *server)
{
	/* dont connect if already connected */
	if(connected)
	{
		debug(3, "connect_server", "Already connected.\n");
		return;
	}
	else if(connecting)
	{
		debug(3, "connect_server", "Already connecting.\n");
		return;
	}

	/* connect() return code */
	int state;

	/* peer address */
	sockaddr_in addr;
	in_addr ip_addr;
	/* host reference */
	hostent *host;
	char *h_name;

	/* now connecting */
	connecting = 1;
	/* probably we aren't connected yet */
	connected = 0;

	/* close socket so that it isn't just overwritten */
	#ifdef WINDOWS
	shutdown(sock, 2);
	#else
	close(sock);
	#endif

	debug(0, "connect_server", "Creating socket."
	   " (IPv4, Stream Socket)\n");

	/*
	 * The socket() system call creates an endpoint for
	 * communication and returns an descriptor. The
	 * following arguments are used.
	 *
	 * PF_INET     : Internet version 4 protocols
	 * SOCK_STREAM : sequenced, reliable, two-way connection
	 *               based bye streams
	 * 0           : Internet protocol, pseudo protocol
	 *               number
	 */
	sock = socket(PF_INET, SOCK_STREAM, 0);

	/*
	 * socket() returns -1 if an error occured, otherwise the
	 * return value is a descriptor referencing the socket.
	 */
	if(sock > -1)
		/* socket created successfully */
		debug(1, "connect_server", "Created socket."
		   " (%i)\n", sock);
	else
	{
		/*
		 * Now an error has occured. Error number is
		 * printed and the function exits with 1.
		 */
		#ifdef WINDOWS
		debug(3, "connect_server", "Couldn't create"
		   " socket. (%d)\n", WSAGetLastError());
		#else
		debug(3, "connect_server", "Couldn't create"
		   " socket. (%s)\n", strerror(errno));
		#endif

		/* leaving function before connecting would be set */
		connecting = 0;

		return;
	}

	/*
	 * Our peer's address is specified in socket's communications
	 * space.
	 */
	addr.sin_family = PF_INET;
	/*
	 * htons() converts peer's 16bit port number to network byte
	 * order
	 */
	addr.sin_port = htons(port);

	/*
	 * server is our peer's hostname and gethostnyname() returns a
	 * pointer to an object describing the host by name or address.
	 */
	host = gethostbyname(server);

	/* check getaddr status */
	if(host != NULL)
		/* got response */
		debug(1, "connect_server", "Got address.\n");
	else
	{
		/* an error ocurred */
		#ifdef WINDOWS
		debug(3, "connect_server", "Failure while"
		   " resolving host. (%d)\n", WSAGetLastError());
		#else
		debug(3, "connect_server", "Failure while"
		   " resolving host. (%s)\n", strerror(errno));
		#endif

		connecting = 0;

		return;
	}

	/* copies host address to peer address */
	memcpy(&addr.sin_addr.s_addr, host->h_addr_list[0],
	   host->h_length);

	/* we need a copy of this */
	ip_addr = addr.sin_addr;
	/* the real hostname of the host we are connecting */
	#ifdef WINDOWS
	//host = gethostbyaddr(&ip_addr, sizeof(ip_addr), PF_INET);
	#else
	host = gethostbyaddr(&ip_addr, sizeof(ip_addr), PF_INET);
	#endif

	/* check gethost status */
	if(host != NULL)
		/* got response */
		debug(1, "connect_server", "Got host.\n");
	else
	{
		/* an error ocurred */
		#ifdef WINDOWS
		debug(3, "connect_server", "Failure while"
		   " resolving address. (%d)\n", WSAGetLastError());
		#else
		debug(3, "connect_server", "Failure while"
		   " resolving address. (%s)\n", strerror(errno));
		#endif

		connecting = 0;

		return;
	}

	h_name = host->h_name;

	debug(0, "connect_server", "Connecting server."
	   " [%s (%s):%i]\n", inet_ntoa(ip_addr), h_name, port);

	/*
	 * connect() initiates a connection on a socket. It attempts to
	 * make a connection to another socket. The function is called
	 * with following arguments.
	 *
	 * sock : our socket descriptor
	 * addr : specifies other socket
	 */
	state = connect(sock, (sockaddr *)&addr, sizeof(addr));

	/* same as above but only 0 on success */
	if(state == 0)
		/* connection attempt has been successfull */
		debug(1, "connect_server", "Connected to"
		   " server.\n");
	else
	{
		/* the attempt hasn't been successfull */
		#ifdef WINDOWS
		debug(3, "connect_server", "Couldn't connect to"
		   " server. (%d)\n", WSAGetLastError());
		#else
		debug(3, "connect_server", "Couldn't connect to"
		   " server. (%s)\n", strerror(errno));
		#endif

		connecting = 0;

		return;
	}

	/* have any error been missed? */
	connected = 1;
	connecting = 0;
}

/* recveive messages from IRC server */
void
IRCSocket::recv_raw(void)
{
	/* length of recvd buffer */
	int b_length, o_length, length;
	/* buffer to put raw message in */
	char buf[R_BUFSIZE];
	char *cbuf;
	char *obuf = new char[0];
	char *nbuf;

	/* does need to have a length of 0 */
	*obuf = '\0';

	debug(0, "recv_raw", "Receiving raw messages.\n");

	/* recv, recv, recv,... */
	while(1)
	{
		/*
		 * clear buffer (shouldn't be needed anymore because of
		 * strncpy() use
		 */
		memset(buf, '\0', R_BUFSIZE);

		/*
		 * read() attempts to read bytes of data from
		 * an object into a buffer.
		 *
		 * socket    : references object
		 * buf       : buffer read into
		 * BUFSIZE   : bytes to read
		 */
		#ifdef WINDOWS
		b_length = recv(sock, buf, R_BUFSIZE - 1, 0);
		#else
		b_length = read(sock, buf, R_BUFSIZE - 1);
		#endif

		/* has server gone away? are we disconnected? */
		if(b_length > 0)
		{
			/* received message, tell length and parse */
			debug(1, "recv_raw", "Received message."
			   " [%s (%i)]\n", "...", b_length);

			o_length = strlen(obuf);
			length = o_length + b_length;

			/*
			 * will be our finally parsed message
			 * and may have to hold a previous ignored
			 * message part
			 */
			nbuf = new char[length + 1];

			/* there has been something ignored - add */
			if(o_length > 0)
				snprintf(nbuf, length + 1, "%s%s", obuf, buf);
			else
				strncpy(nbuf, buf, b_length + 1);

			/* (cut and) parse message(s) */
			cbuf = parse(nbuf);

			delete obuf;
			delete nbuf;

			/* ignored message part */
			length = strlen(cbuf);
			obuf = new char[length + 1];
			strncpy(obuf, cbuf, length + 1);
		}
		else
		{
			#ifdef WINDOWS
			debug(3, "recv_raw", "Couldn't receive"
			   " message. (%d)\n", WSAGetLastError());
			#else
			debug(3, "recv_raw", "Couldn't receive"
			   " message. (%s)\n", strerror(errno));
			#endif

			if(!_DBGRECON)
			{
				debug(3, "recv_raw", "Reconnecting disabled.\n");
				break;
			}

			debug(3, "recv_raw", "Requesting reconnect.\n");

			/* disconnected...reconnect ;) */
			reconnect();
		}
	}
}

/* send raw message */
void
IRCSocket::send_raw(char *fmt, ...)
{
	char tmp[W_BUFSIZE];
// TODO len check
	va_list ap;

	/* the argument list
	 *
	 * our function is called with
	 * varying number of arguments
	 * of varying types. stdarg
	 * provides macros for stepping
	 * through such a list.
	 *
	 * va_start() initializes our
	 * list for subsequent use with
	 * other va_* macros.
	 *
	 * ap - the list of type va_list
	 * fmt - last argument before the
	 *       variable argument list
	 */
	va_start(ap, fmt);

	vsnprintf(tmp, W_BUFSIZE, fmt, ap);

	/* add to queue */
	add_cmd(&IRCSocket::sock_send, tmp);

	/*
	 * normal return from the function
	 * whose variable argument list was
	 * initialized by va_start()
	 */
	va_end(ap);
}

int
IRCSocket::sock_send(const char *buf)
{
	int len, status;
	char tmp[W_BUFSIZE];

	/* length of string to send */
	len = strlen(buf);

	debug(0, "sock_send", "Sending Message (%s)\n", buf);

	/* we have a size maximum in the IRC proto */
	if(len < W_BUFSIZE-3)
	{
		debug(1, "sock_send", "Message size ok."
		   " (%i/%i)\n", len, W_BUFSIZE);

		/* actual message, cat \r\n */
		snprintf(tmp, W_BUFSIZE, "%s\r\n", buf);
		len += 2;
	}
	else
	{
		debug(3, "sock_send", "Message too long."
		   " (%i/%i)\n", len, W_BUFSIZE);
		return -1;
	}

	/*
	 * send() is used to transmit a message
	 * to another socket. the socket must be
	 * connected.
	 *
	 * sock : socket descriptor
	 * 0    : default flags
	 */
	status = send(sock, tmp, len, 0);

	/* check status and return send's return value */
	if(status > -1)
		debug(1, "sock_send", "Sent raw data."
		   " (%i)\n", status);
	else
		#ifdef WINDOWS
		debug(3, "sock_send", "Couldn't send raw data."
		   " (%d)\n", WSAGetLastError());
		#else
		debug(3, "sock_send", "Couldn't send raw data."
		   " (%s)\n", strerror(errno));
		#endif

	return status;
}

int
IRCSocket::sock_recv(char *buf)
{
	int status;

	/*
	 * send() is used to receive a message
	 * from a socket. the socket must be
	 * connected.
	 *
	 * sock : socket descriptor
	 * 0    : default flags
	 */
	status = recv(sock, buf, R_BUFSIZE, 0);

	if(status > -1)
		debug(1, "sock_recv", "Received raw data."
		   " (%i)\n", status);
	else
		#ifdef WINDOWS
		debug(3, "sock_send", "Couldn't receive raw data."
		   " (%d)\n", WSAGetLastError());
		#else
		debug(3, "sock_recv", "Couldn't receive raw data."
		   " (%s)\n", strerror(errno));
		#endif

	return status;
}

void
IRCSocket::disconnect_server(const char *quit_msg)
{
	/* do not reconnect () */
	_DBGRECON = 0;

	debug(1, "disconnect_server", "Disconnecting.\n");

	/* tell IRC server we're disconnecting */
	send_quit(quit_msg);

	/* reset connection state */
	connected = 0;
}

void
IRCSocket::reconnect(void)
{
	/* when someone's connecting wait */
	if(connecting || reconnecting)
	{
		if(connecting)
			debug(3, "reconnect", "Already connecting.\n");
		else if(reconnecting)
			debug(3, "reconnect", "Already reconnecting.\n");

		while(connecting || reconnecting) { /* ... */ }

		/* check wheter connect has been successfull */
		if(connected)
		{
			sleep_time = 1;
			reconnecting = 0;
			return;
		}
	}

	debug(3, "reconnect", "Trying reconnect. (%i)\n", sleep_time);

	reconnecting = 1;

	/* sleep given time before another attempt */
	#ifdef WINDOWS
	Sleep(sleep_time * 1000);
	#else
	sleep(sleep_time);
	#endif

	/* removing all */
	while(cmds > 0)
		del_cmd();

	/* assuming we're no more connected (new auth) */
	connected = 0;
	authed = 0;

	/* reconnect using default values */
	connect_server(_IRCPORT, _IRCSERV);
	auth(_IRCNICK, _IRCUSER, _IRCREAL, _IRCPASS);

	/* increment sleep time if connect failed */
	if(!connected)
		sleep_time = sleep_time * 2;
	else
		sleep_time = 1;

	reconnecting = 0;
}

