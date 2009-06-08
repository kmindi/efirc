/**
** efirc steht unter der "Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland Lizenz".
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the "Creative Commons Attribution-Share Alike 3.0 Germany License".
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// zentrale.h
// Definiert die Klasse Zentrale

#ifndef _ZENTRALE_
#define _ZENTRALE_

#include <map>

// wxwidgets Quellen einbinden
#include <wx/app.h>
#include <wx/version.h> // fuer Versionsinformationen von wxWidgets
#include <wx/platinfo.h> // fuer Informationen der Laufzeitumgebung
#include <wx/utils.h> // fuer BS, Benutzer und Netzwerkfunktionen
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/textdlg.h> // fuer textdialog zur eingabe eines einzeiligen textes

class Zentrale; //Thread muss wissen, dass Zentrale eine Klasse ist

// eigene Quellen einbinden
#include <thread.h>
#include <config.h>
#include <ui.h>
#include <irc.h>
#include <event.h>
#include <version.h>

#include <icon.xpm>

class Zentrale : public wxApp
{
    public:
        Zentrale() {}
        ~Zentrale() {}

        virtual bool OnInit();

        wxString efirc_version_string;

        friend class Thread;
        friend class Ereignisverwalter;
        // muss ein Freund sein, damit die erstellten Threads
        // auf die privaten Thread Funktionen zugreifen koennen

        IRC *irc;
        Ereignisverwalter *Ereignisvw; // Zeiger auf einen eigenen Ereignisverwalter
        Konfiguration *config; // Zeiger zur Konfiguration

        void fensterzerstoeren(wxString);
        void EingabeVerarbeiten(wxString, wxString);

        Fenster* fenstersuchen(wxString);
        Fenster* fenster(wxString name);
        unsigned int anzahl_offene_fenster();

    private:
        map<wxString, Fenster*> zgr_fenster;
        Fenster* neuesFenster(wxString); // erstellt neue Instanz fuer uebergebenen Raum

        int OnExit();

        void zeige_ueber();

        //Konfiguration
        wxString standardkonfiguration();
        wxString zufallstext(int anzahl_zeichen = 4);
        void Konfiguration_anpassen();

        void BefehlVerarbeiten(wxString fenstername, wxString befehl);
        void NachrichtSenden(wxString fenstername, wxString nachricht);

        //Threads
        void threads_starten();
            void connect_thread();
            void recv_thread();
            void call_thread();

        Thread* thrd_connect;
        Thread* thrd_recv;
        Thread* thrd_call;

        // IRC FUNKTIONEN
        void irc_pmsg(const IRC_NACHRICHT *);
        void irc_welcome(const IRC_NACHRICHT *msg_data);
        void irc_isupport(const IRC_NACHRICHT *msg_data);
        void irc_mode(const IRC_NACHRICHT *msg_data);
        void irc_endofmotd(const IRC_NACHRICHT *msg_data);
        void irc_motd(const IRC_NACHRICHT *msg_data);
        void irc_chanurl(const IRC_NACHRICHT *msg_data);
        void irc_topic(const IRC_NACHRICHT *msg_data);
        void irc_topicwhotime(const IRC_NACHRICHT *msg_data);
        void irc_requestedtopic(const IRC_NACHRICHT *msg_data);
        void irc_pong(const IRC_NACHRICHT *msg_data);
        void irc_whoisuser(const IRC_NACHRICHT *msg_data);
        void irc_whoisaway(const IRC_NACHRICHT *msg_data);
        void irc_whoischan(const IRC_NACHRICHT *msg_data);
        void irc_whoisidle(const IRC_NACHRICHT *msg_data);
        void irc_whoisserver(const IRC_NACHRICHT *msg_data);
        void irc_whoisspecial(const IRC_NACHRICHT *msg_data);
        void irc_whoisactually(const IRC_NACHRICHT *msg_data);
        void irc_userlist(const IRC_NACHRICHT *msg_data);
        void irc_join(const IRC_NACHRICHT *msg_data);
        void irc_leave(const IRC_NACHRICHT *msg_data);
        void irc_quit(const IRC_NACHRICHT *msg_data);
        void irc_nick(const IRC_NACHRICHT *msg_data);
        void irc_nickinuse(const IRC_NACHRICHT *msg_data);
        void irc_unaway(const IRC_NACHRICHT *msg_data);
        void irc_nowaway(const IRC_NACHRICHT *msg_data);
        void irc_invite(const IRC_NACHRICHT *msg_data);
        void irc_kick(const IRC_NACHRICHT *msg_data);
        
        void irc_einfach(const IRC_NACHRICHT *msg_data);
        void irc_fehler(const IRC_NACHRICHT *msg_data);
        void irc_unbekannt(const IRC_NACHRICHT *msg_data);
};

#endif
