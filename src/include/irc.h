/**
** efirc steht unter der MIT-Lizenz.
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the MIT License.
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// irc.h
// definiert die Klasse IRC

#ifndef _IRC_
#define _IRC_

#include <wx/string.h>
#include <ircinterface.h>

using namespace std;

class IRC : public IRCInterface
{
    public:
        wxString CurrentNick;
        wxString CurrentHostname;
        wxString CurrentPort;
        wxString WantedNick;

        IRC(wxString, wxString, wxString, wxString, wxString, wxString);

        void connect();
};
#endif
