/**
** efirc steht unter der MIT-Lizenz.
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the MIT License.
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// irc.cc
// implementiert die Klasse IRC

#include <irc.h>

using namespace std;

IRC::IRC(wxString port, wxString server, wxString nick, wxString user, wxString real, wxString pass)
    : IRCInterface(atoi(port.mb_str()), server.mb_str(), nick.mb_str(), user.mb_str(), real.mb_str(), pass.mb_str(), "efirc.log",2)
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
    irc_connect_server();
    irc_auth_client();
}
