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
        ~IRC();

        void connect();
};
#endif
