// irc.h
// definiert die Klasse IRCInterface

#ifndef _IRCInterface_
#define _IRCInterface_

#include <wx/string.h>
#include <cppirc/ircsocket.h>

using namespace std;

class IRCInterface : public IRCSocket
{
    public:
        wxString CurrentNick;
        wxString CurrentHostname;
        wxString CurrentPort;
        wxString WantedNick;
    
        IRCInterface(wxString, wxString, wxString, wxString, wxString, wxString);
        ~IRCInterface();

        void connect();
};
#endif
