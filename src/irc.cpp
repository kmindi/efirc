// irc.cpp
// implementiert die Klasse IRCInterface

#include <irc.h>

using namespace std;

IRCInterface::IRCInterface(wxString port, wxString server,
                           wxString nick, wxString user, wxString real,
                           wxString pass)
             : IRCSocket(atoi(port.c_str()), server.c_str(),
                         nick.c_str(), user.c_str(), real.c_str(),
                         pass.c_str(), fopen("cppirc.log", "w"))
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
