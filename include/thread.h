// thread.h
// definiert die Klasse Thread

#ifndef _THREADS_
#define _THREADS_

#include <wx/thread.h>

// NUR NOCH EINE KLASSE
// MIT KONSTRUKTORPARAMETER, ZEIGER AUF FUNKTION IN DER ZENTRALE
// DIESE FUNKTION WIRD IN ENTRY AUFGERUFEN

class Thread : public wxThread 
{
    private:
        void (Zentrale::*adresse)();
    
    public:
        Thread(void (Zentrale::*fkt_zum_aufrufen)());
        ~Thread();
        virtual void* Entry();
};
#endif
