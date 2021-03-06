/**
** efirc steht unter der MIT-Lizenz.
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the MIT License.
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// thread.h
// definiert die Klasse Thread

#ifndef _THREADS_
#define _THREADS_

#include <wx/thread.h>

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
