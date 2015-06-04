/**
** efirc steht unter der MIT-Lizenz.
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the MIT License.
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

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
            // wenn etwas nicht umgewandelt werden kann, wird eine leere Zeichenkette zurueckgegeben
            // um trotzdem den Inhalt der Variablen umzuwandeln wird eine Alternative Methode verwendet
            // ("wxConvCurrent, points to the conversion object that the user interface is supposed to use")
            
            sender = wxString(msg_data->sender, wxConvUTF8); if(sender == _T("")) sender = wxString(msg_data->sender, *wxConvCurrent);
            cmd = wxString(msg_data->cmd, wxConvUTF8); if(cmd == _T("")) cmd = wxString(msg_data->cmd, *wxConvCurrent);
            params = wxString(msg_data->params, wxConvUTF8); if(params == _T("")) params = wxString(msg_data->params, *wxConvCurrent);
            host = wxString(msg_data->host, wxConvUTF8); if(host == _T("")) host = wxString(msg_data->host, *wxConvCurrent);
            nick = wxString(msg_data->nick, wxConvUTF8); if(nick == _T("")) nick = wxString(msg_data->nick, *wxConvCurrent);
            user = wxString(msg_data->user, wxConvUTF8); if(user == _T("")) user = wxString(msg_data->user, *wxConvCurrent);
            params_i = msg_data->params_i;

            params_a = new wxString[params_i];

            for(int i = 0; i < params_i; i++)
            {
                params_a[i] = wxString(msg_data->params_a[i], wxConvUTF8);
                if(params_a[i] == _T("")) params_a[i] = wxString(msg_data->params_a[i], *wxConvCurrent);
            }
        }

        ~IRC_NACHRICHT()
        {
            delete[] params_a;
        }

        wxString sender, cmd, params, host, nick, user;
        wxString *params_a;
        int params_i;
};

#endif
