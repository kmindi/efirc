/**
** efirc steht unter der "Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland Lizenz".
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the "Creative Commons Attribution-Share Alike 3.0 Germany License".
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

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
