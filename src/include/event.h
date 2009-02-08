// event.h
// definiert die Klasse fuer den eigenen Ereignisverwalter

#ifndef _EVENTS_
#define _EVENTS_

#include <wx/event.h>

class Ereignisverwalter : public wxEvtHandler 
{
    public:
    void BeiNeuesFenster(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()
};
#endif
