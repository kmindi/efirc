// irc.cc
// implementiert die Klasse IRC

#include <irc.h>

using namespace std;

IRC::IRC(wxString port, wxString server, wxString nick, wxString user, wxString real, wxString pass)
    : IRCInterface(atoi(port.mb_str()), server.mb_str(), nick.mb_str(), user.mb_str(), real.mb_str(), pass.mb_str(), "cppirc.log",2)
{
    CurrentNick = nick;
    WantedNick = nick;
    CurrentHostname = server;
    CurrentPort = port;
}

void
IRC::connect()
{
    // Verbindung zum Server aufbauen und authentifizieren
    connect_server();
    auth();
}

IRC::~IRC()
{
    // Logdatei schliessen
    fclose(_DBGSTR);
}
