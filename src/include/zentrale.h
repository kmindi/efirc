// zentrale.h
// Definiert die Klasse Zentrale

#ifndef _ZENTRALE_
#define _ZENTRALE_

const int max_fenster = 10;

#include <wx/app.h>
#include <wx/version.h> // fuer Versionsinformationen von wxWidgets
#include <wx/platinfo.h> // fuer Informationen der Laufzeitumgebung
#include <wx/utils.h> // fuer BS, Benutzer und Netzwerkfunktionen
class Zentrale; //Thread muss wissen, dass Zentrale eine Klasse ist
#include <thread.h>
#include <config.h>
#include <ui.h>
#include <irc.h>
#include <event.h>

class Zentrale : public wxApp
{
    public:
        Zentrale() {}
        ~Zentrale() {}
        
        virtual bool OnInit();
        
        wxString efirc_version_string;
        
        friend class Thread; 
        // muss ein Freund sein, damit die erstellten Threads 
        // auf die privaten Thread funktionen zugreifen koennen
        
        IRCInterface *irc; //zeiger zum IRCInterface erzeugen
        Ereignisverwalter *Ereignisvw; // Zeiger auf einen eigenen Ereignisverwalter
        Konfiguration *config; // Zeiger zur Konfiguration
        
        void fensterzerstoeren(int);
        void fensterzerstoeren(wxString);
        void EingabeVerarbeiten(int,wxString);
        
        Fenster* fenstersuchen(wxString); 
        Fenster* fenster(wxString name);
        
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
        
        //Konfiguration
        wxString standardkonfiguration();
        wxString zufallstext(int anzahl_zeichen = 4);
        void Konfiguration_anpassen();
        
        
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
