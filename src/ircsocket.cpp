/*
 * our header file is ircsocket.h
 *
 * class definition
 * function dummies
 * structures
 *
 * see it for more information ;)
 */
#include "ircsocket.h"

// TODO errorhandler for ^C
//      malloc()-use
//      strtok()
//      strncpy
//      split
//      start function

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

      /* no sleep_time needed */
      sleep_time = 0;

      /* set defaults */
      _IRCPORT = port;
      strcpy(_IRCSERV, server);
      strcpy(_IRCNICK, nick);
      strcpy(_IRCUSER, user);
      strcpy(_IRCREAL, real);
      strcpy(_IRCPASS, pass);
      _DBGSTR = log;
      _DBGSPACE = 15;
      _DBGLEVEL = 1;
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
      /* pointing to new cmd in queue */
      irc_queue_cmd *ntp;

      /* struct with command and arguments */
      ntp = new irc_queue_cmd;

      /* new command on top  */
      ntp->function = function;
      ntp->buf = new char[strlen(buf) + 1];
      strcpy(ntp->buf, buf);
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
      /* new link on top */
      irc_act_link *ntp;

      ntp = new struct irc_act_link;

      ntp->function = function;
      ntp->cmd = new char[strlen(cmd) + 1];
      strcpy(ntp->cmd, cmd);
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
      int i, params, length, text;
      /* se substring */
      char *sstr;
      char *wstr;

      /* number of parameters (excluding `\s:.*$') */
      params = 0;
      /* length of resulting parameters join */
      length = 0;
      /* specifies wheter a string is found in parameters-list */
      text = 0;

      /* struct containing sender, command and parameters */
      irc_msg_data *msg_data = new irc_msg_data;

      debug(0, "parse_msg", "Parsing message. (%s)\n",
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

                    msg_data->sender = new char[strlen(raw_msg) + 1];
                    strcpy(msg_data->sender, raw_msg);

                    debug(1, "parse_msg", "Found sender."
                       " (%s)\n", msg_data->sender);

                    raw_msg = sstr + 1;
             }
             else
             {
                    msg_data->sender = new char[strlen("No sender") + 1];
                    strcpy(msg_data->sender, "No sender");

                    debug(1, "parse_msg", "No sender.\n");
             }

             /* command */
             sstr = strstr(raw_msg, " ");
             if(sstr != NULL)
             {
                    *sstr = '\0';

                    msg_data->cmd = new char[strlen(raw_msg) + 1];
                    strcpy(msg_data->cmd, raw_msg);

                    debug(1, "parse_msg", "Found command."
                       " (%s)\n", msg_data->cmd);

                    raw_msg = sstr + 1;
             }
             else
             {
                    msg_data->cmd = new char[strlen("No command") + 1];
                    strcpy(msg_data->cmd, "No command");

                    debug(1, "parse_msg", "No command.\n");
             }

             /* parameters */
             msg_data->params = new char[strlen(raw_msg) + 1];
             strcpy(msg_data->params, raw_msg);

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

                    msg_data->nick = new char[strlen(wstr) + 1];
                    strcpy(msg_data->nick, wstr);
                    wstr = sstr + 1;

                    debug(1, "parse_msg", "Found nickname."
                       " (%s)\n", msg_data->nick);

                    sstr += 1;
             }
             else
             {
                    msg_data->nick = new char[strlen("No nick") + 1];
                    strcpy(msg_data->nick, "No nick");

                    debug(1, "parse_msg", "No nick.\n");
             }

             sstr = strstr(wstr, "@");
             if(sstr != NULL)
             {
                    *sstr = '\0';

                    msg_data->user = new char[strlen(wstr) + 1];
                    strcpy(msg_data->user, wstr);
                    wstr = sstr + 1;

                    debug(1, "parse_msg", "Found username."
                       " (%s)\n", msg_data->user);

                    sstr += 1;
             }
             else
             {
                    msg_data->user = new char[strlen("No user") + 1];
                    strcpy(msg_data->user, "No user");

                    debug(1, "parse_msg", "No user.\n");
             }

             msg_data->host = new char[strlen(wstr) + 1];
             strcpy(msg_data->host, wstr);

             debug(1, "parse_msg", "Found host."
                " (%s)\n", msg_data->host);

             /* not usable anymore - set to "USER" */
             delete msg_data->sender;
             msg_data->sender = new char[strlen(msg_data->nick) +
                strlen(msg_data->user) + strlen(msg_data->host) + 1];
             sprintf(msg_data->sender, "%s%s%s", msg_data->nick,
                msg_data->user, msg_data->host);
      }
      /* we have a server message */
      else
      {
             debug(1, "parse_msg", "Is server message.\n");

// TODO server we're really connected to
             msg_data->sender = new char[strlen("SERVER") + 1];
             strcpy(msg_data->sender, "SERVER");

             /* command */
             sstr = strstr(raw_msg, " ");
             if(sstr != NULL)
             {
                    *sstr = '\0';

                    msg_data->cmd = new char[strlen(raw_msg) + 1];
                    strcpy(msg_data->cmd, raw_msg);

                    debug(1, "parse_msg", "Found command."
                       " (%s)\n", msg_data->cmd);

                    raw_msg = sstr + 1;
             }
             else
             {
                    msg_data->cmd = new char[strlen("No command") + 1];
                    strcpy(msg_data->cmd, "No command");

                    debug(1, "parse_msg", "No command.\n");
             }

             /* parameters */
             msg_data->params = new char[strlen(raw_msg) + 1];
             strcpy(msg_data->params, raw_msg);

             debug(1, "parse_msg", "Found parameters."
                " (%s)\n", msg_data->params);

             /* safely :/ */
             msg_data->host = new char[strlen("No host.") + 1];
             strcpy(msg_data->host, "No host");
             msg_data->nick = new char[strlen("No nick.") + 1];
             strcpy(msg_data->nick, "No nick");
             msg_data->user = new char[strlen("No user.") + 1];
             strcpy(msg_data->user, "No user");
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
                   *sstr = '\0';
            }
            /* string - skip ':' */
            else
                   wstr++;

            msg_data->params_a[i] = new char[strlen(wstr) + 1];
            strcpy(msg_data->params_a[i], wstr);

            wstr = sstr + 1;
      }

      /* parameters are rebuilt */
      delete msg_data->params;

      /* get full length including separators */
      for(i = 0; i < params; i++)
            length += strlen(msg_data->params_a[i]) + 1;

      /* allocate and clear (?) */
      msg_data->params = new char[length + 1];
      memset(msg_data->params, '\0', length + 1);

      /* join parameters */
      for(i = 0; i < params; i++)
      {
            /*
             * if last parameter is a string
             * prefix it with a ':'
             */
            if(text && i == params - 1)
                   strcat(msg_data->params, ":");

            strcat(msg_data->params, msg_data->params_a[i]);

            /*
             * if not last parameter cat space
             * as seperator
             */
            if(i != params - 1)
                   strcat(msg_data->params, " ");
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

/****************************
 *
 * I O
 *
 ****************************/

int
IRCSocket::debug(unsigned int level, const char *sender,
   const char *fmt, ...)
{
      /* return if level is below wanted level */
      if(level < _DBGLEVEL)
             return 1;

      int i, status, spaces;
      char level_ind;
      char timestamp[9];
      char *space, *nfmt;
      time_t raw_time;
      /* struct containing sec, min, hr,... */
      tm *local_time;
      /* for more on va_* see below */
      va_list ap;
      va_start(ap, fmt);

      /* get local time */
      time(&raw_time);
      local_time = localtime(&raw_time);

      /* set up level-indicator */
      if(level == 0)
             level_ind = '.';
      else if(level == 1)
             level_ind = 'i';
      else if(level == 2)
             level_ind = 'i';
      else if(level == 3)
             level_ind = '!';
      else
             level_ind = '?';

      /* set up space */
      spaces = _DBGSPACE - strlen(sender);

      if(spaces < 0)
                spaces = 0;

      space = new char[spaces + 1];
      memset(space, '\0', spaces + 1);

      /* fill space with spaces :) */
      for(i = 0; i < spaces; i++)
             strcat(space, " ");

      /* set up timestamp (%H:%M:%S) */
      strftime(timestamp, 9, "%H:%M:%S", local_time);

      /* set up logline */
      nfmt = new char[strlen(space) + strlen(sender) + strlen(fmt) +
         strlen(timestamp) + 4];
      sprintf(nfmt, "%s|%c|%s%s%s", timestamp, level_ind, sender, space,
         fmt);

      status = vfprintf(_DBGSTR, nfmt, ap);

      va_end(ap);

      /* flush stream */
      fflush(_DBGSTR);

      delete nfmt;
      delete space;

      return status;
}

void
IRCSocket::reconnect(void)
{
      if(reconnecting)
      {
            debug(3, "reconnect", "Already reconnecting.\n");
            return;
      }

      reconnecting = 1;

      /* when someone's connecting wait */
      if(connecting)
      {
            debug(3, "reconnect", "Already connecting.\n");

            while(connecting) { /* ... */ }

            /* check wheter connect has been successfull */
            if(connected)
            {
                  sleep_time = 0;
                  reconnecting = 0;
                  return;
            }
      }

      /* sleep given time before another attempt */
      #ifdef WINDOWS
      Sleep(sleep_time * 1000);
      #else
      sleep(sleep_time);
      #endif

      debug(3, "reconnect", "Trying reconnect.\n");

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
            sleep_time++;
      else
            sleep_time = 0;

      reconnecting = 0;
}

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
      int length;
      /* buffer to put raw message in */
      char buf[R_BUFSIZE + 1];
      char *cbuf;
      char *obuf = new char[0];
      char *nbuf;

      /* does need to have a length of 0 */
      *obuf = '\0';

      debug(0, "recv_raw", "Receiving raw messages.\n");

      /* recv, recv, recv,... */
      while(1)
      {
            /* clear buffer */
            memset(buf, '\0', R_BUFSIZE + 1);

            /*
             * read() attempts to read bytes of data from
             * an object into a buffer.
             *
             * socket    : references object
             * buf       : buffer read into
             * BUFSIZE   : bytes to read
             */
            #ifdef WINDOWS
            length = recv(sock, buf, R_BUFSIZE, 0);
            #else
            length = read(sock, buf, R_BUFSIZE);
            #endif

            /* has server gone away? are we disconnected? */
            if(length > 0)
            {
                    /* received message, tell length and parse */
                    debug(1, "recv_raw", "Received message."
                       " [%s (%i)]\n", "...", length);

                    /*
                     * will be our finally parsed message
                     * and may have to hold a previous ignored
                     * message part
                     */
                    nbuf = new char[strlen(obuf) + length + 1];

                    /* there has been something ignored - add */
                    if(strlen(obuf) > 0)
                    {
                          sprintf(nbuf, "%s%s", obuf, buf);
                    }
                    else
                    {
                          strcpy(nbuf, buf);
                    }

                    /* (cut and) parse message(s) */
                    cbuf = parse(nbuf);

                    delete obuf;
                    delete nbuf;

                    /* ignored message part */
                    obuf = new char[strlen(cbuf) + 1];
                    strcpy(obuf, cbuf);
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
      /* tell IRC server we're disconnecting */
      send_quit(quit_msg);
}

/****************************
 *
 * I R C   C O M M A N D S
 *
 ****************************/

/* auth new IRC user */
void
IRCSocket::auth(const char *nick, const char *user, const char *real,
   const char *pass)
{
// TODO you aren't allowed to call when already done?
      if(authed)
      {
            debug(3, "auth", "Already authed.\n");
            return;
      }

      authed = 1;

      /*
       * User authentifaction is needed on every connect.
       * First we need a session password, then a specific
       * nick and the user- and realname. These data can
       * be viewed by calling WHOIS.
       */
      send_pass(pass);
      send_nick(nick);
      send_user(user, real);
}

/* send final QUIT message to server */
void
IRCSocket::send_quit(const char *quitmsg)
{
      send_raw("QUIT :%s", quitmsg);
}

/* send PASS when authing user */
void
IRCSocket::send_pass(const char *pass)
{
      send_raw("PASS %s", pass);
}

/* send NICK to change user's nickname */
void
IRCSocket::send_nick(const char *nick)
{
      send_raw("NICK %s", nick);

// TODO sometime we DO NOT have this nick
      /* new nick requested? */
      if(strcmp(_IRCNICK, nick))
            strcpy(_IRCNICK, nick);
}

/* send USER command to authenticate user */
void
IRCSocket::send_user(const char *user, const char *real)
{
      send_raw("USER %s host serv :%s", user, real);
}

/* send JOIN command and join channel ;) */
void
IRCSocket::send_join(const char *chan)
{
      send_raw("JOIN %s", chan);
}

/* send PONG back */
void
IRCSocket::send_pong(const char *key)
{
      send_raw("PONG %s", key);
}

/* CTCP - our client version */
void
IRCSocket::send_version(const char *recipient)
{
      char tmp[W_BUFSIZE];

      snprintf(tmp, W_BUFSIZE, "\001VERSION\001 %s", VERSION);
      send_notice(recipient, tmp);
}

/* the NOTICE command */
void
IRCSocket::send_notice(const char *recipient, const char *text)
{
      send_raw("NOTICE %s :%s", recipient, text);
}

/* the PRIVMSG command */
void
IRCSocket::send_privmsg(const char *recipient, const char *text)
{
      send_raw("PRIVMSG %s :%s", recipient, text);
}

/* TOPIC */
void
IRCSocket::send_topic(const char *chan, const char *text)
{
      send_raw("TOPIC %s :%s", chan, text);
}

/* PART */
void
IRCSocket::send_part(const char *chan)
{
      send_raw("PART %s", chan);
}

