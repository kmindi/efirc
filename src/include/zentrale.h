// zentrale.h
// Definiert die Klasse Zentrale

#ifndef _ZENTRALE_
#define _ZENTRALE_

#include <wx/app.h>
#include <wx/socket.h>
class Zentrale; //Thread muss wissen, dass Zentrale eine Klasse ist
#include <thread.h>
#include <ui.h>
#include <irc.h>
#include <event.h>

class Zentrale : public wxApp
{
    public:
        virtual bool OnInit();
        
        friend class Thread; 
        // muss ein Freund sein, damit die erstellten Threads 
        // auf die privaten Thread funktionen zugreifen koennen
        
        IRCInterface *irc; //zeiger zum IRCInterface erzeugen
        Ereignisverwalter *Ereignisvw; // Zeiger auf einen eigenen Ereignisverwalter
        
        void fensterzerstoeren(int);
        void fensterzerstoeren(wxString);
        void EingabeVerarbeiten(int,wxString);
        
        Fenster* fenstersuchen(wxString); 
        
        // --------------------------------------------------------------------------
        // NUR OEFFENTLICH WEIL DIE IRC FUNKTIONEN NOCH KEINE MITGLIEDER SIND
        // --------------------------------------------------------------------------
        
        // MAXIMALE ANZAHL DER RAEUME PER VARIABLE FESTSETZEN
        Fenster *zgr_fenster[10]; // feld fuer zeiger auf maximal 10 instanzen
        wxString fenstername[10]; 
        wxString raum; // fuer join nach motd, temporaer
        void neuesFenster(wxString); // erstellt neue Instanz fuer uebergebenen Raum
        
    private:
        
        int OnExit();
        
        
        void BefehlVerarbeiten(int fensternummer, wxString befehl);
        void NachrichtSenden(int fensternummer, wxString nachricht);
        
        //Threads
        void connect_thread();
        void recv_thread();
        void call_thread();
        
        
        //IRC FUNKTIONEN
        //void irc_pmsg(const irc_msg_data *msg_data, void *cp);
        
        
};

#endif