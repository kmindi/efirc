#include "ircinterface.h"

/****************************
 *
 * T C P / I P
 *
 ****************************/

/* connect server:port */
void
IRCInterface::irc_connect_server(const char *server_hostname, unsigned int server_port)
{
	/* dont connect if already connected */
	if(connected) {
		irc_write_message_f(3, "irc_connect_server", "Already connected.\n");
		return;
	} else if(connecting) {
		irc_write_message_f(3, "irc_connect_server", "Already connecting.\n");
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

	irc_write_message_f(0, "irc_connect_server", "Creating socket."
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
		irc_write_message_f(1, "irc_connect_server", "Created socket."
			" (%i)\n", sock);
	else {
		/*
		 * Now an error has occured. Error number is
		 * printed and the function exits with 1.
		 */
		#ifdef WIN32
		irc_write_message_f(3, "irc_connect_server", "Couldn't create"
			" socket. (%d)\n", WSAGetLastError());
		#else
		irc_write_message_f(3, "irc_connect_server", "Couldn't create"
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
	addr.sin_port = htons(server_port);

	/*
	 * server is our peer's hostname and gethostnyname() returns a
	 * pointer to an object describing the host by name or address.
	 */
	host = gethostbyname(server_hostname);

	/* check getaddr status */
	if(host != NULL)
		/* got response */
		irc_write_message_f(1, "irc_connect_server", "Got address.\n");
	else {
		/* an error ocurred */
		#ifdef WIN32
		irc_write_message_f(3, "irc_connect_server", "Failure while"
			" resolving host. (%d)\n", WSAGetLastError());
		#else
		irc_write_message_f(3, "irc_connect_server", "Failure while"
			" resolving host. (%s)\n", hstrerror(h_errno));
		#endif

		connecting = 0;

		return;
	}

	/* copies host address to peer address */
	memcpy(&addr.sin_addr.s_addr, host->h_addr_list[0],
	   host->h_length);

	irc_write_message_f(0, "irc_connect_server", "Connecting server."
		" [%s (%s):%i]\n", inet_ntoa(addr.sin_addr),
		host->h_name, server_port);

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
		irc_write_message_f(1, "irc_connect_server", "Connected to"
			" server.\n");
	else {
		/* the attempt hasn't been successfull */
		#ifdef WIN32
		irc_write_message_f(3, "irc_connect_server", "Couldn't connect to"
			" server. (%d)\n", WSAGetLastError());
		#else
		irc_write_message_f(3, "irc_connect_server", "Couldn't connect to"
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
IRCInterface::irc_connect_server(void)
{
	irc_connect_server(_IRCSERV, _IRCPORT);
}

/* recveive messages from IRC server */
void
IRCInterface::irc_receive_messages(void)
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
	o = strdup("");

	irc_write_message_f(0, "irc_receive_messages", "Receiving raw messages.\n");

	while(!disconnected) {
		/* initialise buffer with NULs */
		memset(buf, '\0', sizeof(buf));

		/* receive message from the server connected to */
		bl = irc_receive_message(buf, sizeof(buf));

		/* has server gone away? are we disconnected? */
		if(bl > 0) {
			/* received message, tell length and parse */
			irc_write_message_f(1, "irc_receive_messages", "Received message."
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
			c = irc_split_server_message(n);

			delete[] o;

			/* ignored message part */
			o = strdup(c);

			delete[] n;
		} else {
			if(!_DBGRECON) {
				irc_write_message_f(3, "irc_receive_messages", "Reconnecting"
					" disabled.\n");
				break;
			}

			irc_write_message_f(3, "irc_receive_messages", "Requesting reconnect.\n");

			/* disconnected...reconnect ;) */
			irc_reconnect_server();
		}
	}
}

/* send raw message */
void
IRCInterface::irc_send_message_f(const char *message_format, ...)
{
	char tmp[W_BUFSIZE];
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
	va_start(ap, message_format);

	vsnprintf(tmp, sizeof(tmp), message_format, ap);

	/* add to queue */
	irc_add_command_queue_entry(&IRCInterface::irc_send_message, tmp);

	/*
	 * normal return from the function
	 * whose variable argument list was
	 * initialized by va_start()
	 */
	va_end(ap);
}

int
IRCInterface::irc_send_message(const char *message)
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
	l = strlen(message) + 2;

	irc_write_message_f(2, "irc_send_message", "Sending message. (%s)\n", message);

	/* we have a size maximum in the IRC proto */
	if(l <= sizeof(tmp)) {
		irc_write_message_f(1, "irc_send_message", "Message size ok."
			" (%i/%i)\n", l, sizeof(tmp));

		/* actual message, cat \r\n */
		snprintf(tmp, sizeof(tmp), "%s\r\n", message);
	} else {
		irc_write_message_f(3, "irc_send_message", "Message too long."
			" (%i/%i)\n", l, sizeof(tmp));

		return -1;
	}

	/*
	 * send() is used to transmit a message
	 * to another socket. the socket must be
	 * connected.
	 *
	 * sock         : socket descriptor
	 * MSG_NOSIGNAL : do not generate SIGPIPE on EOF
	 */
	#if defined __FreeBSD__ || defined linux
	bl = send(sock, tmp, l, MSG_NOSIGNAL);
	#else
	bl = send(sock, tmp, l, 0);
	#endif

	/* check status and return send's return value */
	if(bl > -1)
		irc_write_message_f(1, "irc_send_message", "Sent raw data."
			" (%i)\n", bl);
	else
		#ifdef WIN32
		irc_write_message_f(3, "irc_send_message", "Couldn't send raw data."
			" (%d)\n", WSAGetLastError());
		#else
		irc_write_message_f(3, "irc_send_message", "Couldn't send raw data."
			" (%s)\n", strerror(errno));
		#endif

	return bl;
}

int
IRCInterface::irc_receive_message(char *buf, size_t len)
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
	bl = recv(sock, buf, len, 0);

	if(bl > -1)
		irc_write_message_f(1, "irc_receive_message", "Received raw data."
			" (%i)\n", bl);
	else
		#ifdef WIN32
		irc_write_message_f(3, "irc_receive_message", "Couldn't receive raw data."
			" (%d)\n", WSAGetLastError());
		#else
		irc_write_message_f(3, "irc_receive_message", "Couldn't receive raw data."
			" (%s)\n", strerror(errno));
		#endif

	return bl;
}

void
IRCInterface::irc_disconnect_server(const char *quit_message)
{
	/* do not reconnect () */
	_DBGRECON = 0;

	irc_write_message_f(1, "irc_disconnect_server", "Disconnecting.\n");

	/* tell IRC server we're disconnecting */
	irc_send_quit(quit_message);

	/* reset connection state */
	connected = 0;
	disconnected = 1;
}

void
IRCInterface::irc_disconnect_server(void)
{
	irc_disconnect_server("");
}

void
IRCInterface::irc_reconnect_server(void)
{
	/* when someone's connecting wait */
	if(connecting || reconnecting) {
		if(connecting)
			irc_write_message_f(3, "irc_reconnect_server", "Already connecting.\n");
		else if(reconnecting)
			irc_write_message_f(3, "irc_reconnect_server", "Already"
				" reconnecting.\n");

		while(connecting || reconnecting) { /* ... */ }

		return;
	}
	else if(disconnected)
	{
		irc_write_message_f(3, "irc_reconnect_server", "Already"
			" disconnected.\n");

		return;
	}

	irc_write_message_f(3, "irc_reconnect_server", "Trying reconnect. (%i)\n", sleep_time);

	reconnecting = 1;

	/* sleep given time before another attempt */
	#ifdef WIN32
	Sleep(sleep_time * 1000);
	#else
	sleep(sleep_time);
	#endif

	/* removing all */
	while(cmds > 0)
		irc_delete_command_queue_entry();

	/* assuming we're no more connected (new auth) */
	connected = 0;
	authed = 0;

	/* reconnect using default values */
	irc_connect_server();
	irc_auth_client();

	/* increment sleep time if connect failed */
	if(!connected)
		sleep_time = sleep_time * 2;
	else
		sleep_time = 1;

	reconnecting = 0;
}

