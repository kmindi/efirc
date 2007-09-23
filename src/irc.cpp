#include <irc.h>

using namespace std;

IRCInterface::IRCInterface(string port, string server,
                           string nick, string user, string real,
                           string pass)
             : IRCSocket(atoi(port.c_str()), server.c_str(),
                         nick.c_str(), user.c_str(), real.c_str(),
                         pass.c_str(), fopen("cppirc.log", "w"))
{
    // Verbindung zum Server aufbauen und authentifizieren
    connect_server(_IRCPORT, _IRCSERV);
    auth(_IRCNICK, _IRCUSER, _IRCREAL, _IRCPASS);
}

IRCInterface::~IRCInterface()
{
    // Logdatei schliessen
    fclose(_DBGSTR);
}
