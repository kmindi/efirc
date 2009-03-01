#include "ircinterface.h"

/****************************
 *
 * T C P / I P
 *
 ****************************/

/* connect server:port */
void
IRCInterface::connect_server(unsigned int port, const char *server)
{
	/* dont connect if already connected */
	if(connected) {
		debug(3, "connect_server", "Already connected.\n");
		return;
	} else if(connecting) {
		debug(3, "connect_server", "Already connecting.\n");
		return;
	}

	/* connect() return code */
	int s;

	/* peer address */
	sockaddr_in addr;
	/* host reference */
	hostent *host;

	/* now connecting */
	connecting = 1;
	/* probably we aren't connected yet */
	connected = 0;

	/* close socket so that it isn't just overwritten */
	#ifdef WIN32
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
	else {
		/*
		 * Now an error has occured. Error number is
		 * printed and the function exits with 1.
		 */
		#ifdef WIN32
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
	else {
		/* an error ocurred */
		#ifdef WIN32
		debug(3, "connect_server", "Failure while"
			" resolving host. (%d)\n", WSAGetLastError());
		#else
		debug(3, "connect_server", "Failure while"
			" resolving host. (%s)\n", hstrerror(h_errno));
		#endif

		connecting = 0;

		return;
	}

	/* copies host address to peer address */
	memcpy(&addr.sin_addr.s_addr, host->h_addr_list[0],
	   host->h_length);

	debug(0, "connect_server", "Connecting server."
		" [%s (%s):%i]\n", inet_ntoa(addr.sin_addr),
		host->h_name, port);

	/*
	 * connect() initiates a connection on a socket. It attempts to
	 * make a connection to another socket. The function is called
	 * with following arguments.
	 *
	 * sock : our socket descriptor
	 * addr : specifies other socket
	 */
	s = connect(sock, (sockaddr *)&addr, sizeof(addr));

	/* same as above but only 0 on success */
	if(s == 0)
		/* connection attempt has been successfull */
		debug(1, "connect_server", "Connected to"
			" server.\n");
	else {
		/* the attempt hasn't been successfull */
		#ifdef WIN32
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

void
IRCInterface::connect_server(void)
{
	connect_server(_IRCPORT, _IRCSERV);
}

/* recveive messages from IRC server */
void
IRCInterface::recv_raw(void)
{
	size_t ol, l;

	/*
	 * read(2) return values
	 *
	 * >0: the number of bytes actually read
	 *  0: EOF
	 * -1: error (errno is set)
	 */
	ssize_t bl;

	/* buffer to put raw message in */
	char buf[R_BUFSIZE];
	char *c, *o, *n;

	/* does need to have a length of 0 */
	o = new char[1];
	*o = '\0';

	debug(0, "recv_raw", "Receiving raw messages.\n");

	/* recv, recv, recv,... */
	while(1) {
		/*
		 * read() attempts to read bytes of data from
		 * an object into a buffer.
		 *
		 * socket    : references object
		 * buf       : buffer read into
		 * BUFSIZE   : bytes to read
		 */
		#ifdef WIN32
		bl = recv(sock, buf, sizeof(buf), 0);
		#else
		bl = read(sock, buf, sizeof(buf));
		#endif

		/* has server gone away? are we disconnected? */
		if(bl > 0) {
			/* received message, tell length and parse */
			debug(1, "recv_raw", "Received message."
				" [%s (%i)]\n", "...", bl);

			ol = strlen(o);
			l = ol + bl + 1;

			/*
			 * will be our finally parsed message
			 * and may have to hold a previous ignored
			 * message part
			 */
			n = new char[l];

			/* there has been something ignored - add */
			if(ol > 0)
				snprintf(n, l, "%s%s", o, buf);
			else
				strlcpy(n, buf, l);

			/* (cut and) parse message(s) */
			c = parse(n);

			delete o;

			/* ignored message part */
			l = strlen(c) + 1;
			o = new char[l];
			strlcpy(o, c, l);

			delete n;
		} else {
			#ifdef WIN32
			debug(3, "recv_raw", "Couldn't receive"
				" message. (%d)\n", WSAGetLastError());
			#else
			debug(3, "recv_raw", "Couldn't receive"
				" message. (%s)\n", strerror(errno));
			#endif

			if(!_DBGRECON) {
				debug(3, "recv_raw", "Reconnecting"
					" disabled.\n");
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
IRCInterface::send_raw(const char *fmt, ...)
{
	char tmp[W_BUFSIZE];
// TODO len check
	va_list ap;

	/*
	 * the argument list
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

	vsnprintf(tmp, sizeof(tmp), fmt, ap);

	/* add to queue */
	add_cmd(tmp, &IRCInterface::sock_send);

	/*
	 * normal return from the function
	 * whose variable argument list was
	 * initialized by va_start()
	 */
	va_end(ap);
}

int
IRCInterface::sock_send(const char *buf)
{
	size_t l;

	/*
	 * send(2) return values
	 *
	 * >=0: the number of characters sent
	 *  -1: error
	 */
	ssize_t bl;

	char tmp[W_BUFSIZE];

	/* length of string to send */
	l = strlen(buf) + 2;

	debug(2, "sock_send", "Sending message. (%s)\n", buf);

	/* we have a size maximum in the IRC proto */
	if(l <= sizeof(tmp)) {
		debug(1, "sock_send", "Message size ok."
			" (%i/%i)\n", l, sizeof(tmp));

		/* actual message, cat \r\n */
		snprintf(tmp, sizeof(tmp), "%s\r\n", buf);
	} else {
		debug(3, "sock_send", "Message too long."
			" (%i/%i)\n", l, sizeof(tmp));
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
	bl = send(sock, tmp, l, 0);

	/* check status and return send's return value */
	if(bl > -1)
		debug(1, "sock_send", "Sent raw data."
			" (%i)\n", bl);
	else
		#ifdef WIN32
		debug(3, "sock_send", "Couldn't send raw data."
			" (%d)\n", WSAGetLastError());
		#else
		debug(3, "sock_send", "Couldn't send raw data."
			" (%s)\n", strerror(errno));
		#endif

	return bl;
}

int
IRCInterface::sock_recv(char *buf)
{
	/*
	 * recv(2) return values
	 *
	 * >=0: the number of bytes received
	 *  -1: error
	 */
	ssize_t bl;

	/*
	 * recv() is used to receive a message
	 * from a socket. the socket must be
	 * connected.
	 *
	 * sock : socket descriptor
	 * 0    : default flags
	 */
	bl = recv(sock, buf, R_BUFSIZE, 0);

	if(bl > -1)
		debug(1, "sock_recv", "Received raw data."
			" (%i)\n", bl);
	else
		#ifdef WIN32
		debug(3, "sock_send", "Couldn't receive raw data."
			" (%d)\n", WSAGetLastError());
		#else
		debug(3, "sock_recv", "Couldn't receive raw data."
			" (%s)\n", strerror(errno));
		#endif

	return bl;
}

void
IRCInterface::disconnect_server(const char *quit_msg)
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
IRCInterface::reconnect(void)
{
	/* when someone's connecting wait */
	if(connecting || reconnecting) {
		if(connecting)
			debug(3, "reconnect", "Already connecting.\n");
		else if(reconnecting)
			debug(3, "reconnect", "Already"
				" reconnecting.\n");

		while(connecting || reconnecting) { /* ... */ }

		return;
	}

	debug(3, "reconnect", "Trying reconnect. (%i)\n", sleep_time);

	reconnecting = 1;

	/* sleep given time before another attempt */
	#ifdef WIN32
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
	connect_server();
	auth();

	/* increment sleep time if connect failed */
	if(!connected)
		sleep_time = sleep_time * 2;
	else
		sleep_time = 1;

	reconnecting = 0;
}

