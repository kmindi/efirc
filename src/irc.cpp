// irc.cpp
// implementiert die Klasse IRCInterface

#include <irc.h>

using namespace std;

IRCInterface::IRCInterface(wxString port, wxString server,
                           wxString nick, wxString user, wxString real,
                           wxString pass)
             : IRCSocket(atoi(port.mb_str()), server.mb_str(),
                         nick.mb_str(), user.mb_str(), real.mb_str(),
                         pass.mb_str(), "cppirc.log")
{
    CurrentNick = nick;
    WantedNick = nick;
    CurrentHostname = server;
    CurrentPort = port;
}

void
IRCInterface::connect()
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
