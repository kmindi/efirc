// event.h
// definiert die Klasse fuer den eigenen Ereignisverwalter

#ifndef _EVENTS_
#define _EVENTS_

#include <wx/event.h>

class Ereignisverwalter : public wxEvtHandler
{
    public:
    void BeiNeueIRCNachricht(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()
};

class IRC_NACHRICHT
{   
    public:
        IRC_NACHRICHT(const irc_msg_data *msg_data)
        {
            sender = wxString(msg_data->sender, wxConvUTF8);
            cmd = wxString(msg_data->cmd, wxConvUTF8);
            params = wxString(msg_data->params, wxConvUTF8);
            host = wxString(msg_data->host, wxConvUTF8);
            nick = wxString(msg_data->nick, wxConvUTF8);
            user = wxString(msg_data->user, wxConvUTF8);
            params_i = msg_data->params_i;
            
            params_a = new wxString[params_i];
            
            for(int i = 0; i < params_i; i++)
            {
                params_a[i] = msg_data->params_a[i];
            }
        }
        
        ~IRC_NACHRICHT() {}
        
        wxString sender, cmd, params, host, nick, user;
        wxString *params_a;
        int params_i;
};

#endif
