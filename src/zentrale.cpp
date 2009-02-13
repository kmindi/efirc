// zentrale.cpp
// Implementiert die Klasse Zentrale

#include <zentrale.h>

// main funkition erstellen, Instanz der Klasse Zentrale erstellen
// mit DECLARE_APP(Zentrale/) und wxGetApp() kann auf Funktionen dieser Klasse zugegrifffen werden
IMPLEMENT_APP(Zentrale) // erstellt main funktion
// {}

// Benutzerdefinierte Ereignisse deklarieren und definieren
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_NEUES_FENSTER, 7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_NEUES_FENSTER)

#define EVT_NEUES_FENSTER(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_NEUES_FENSTER, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),


BEGIN_EVENT_TABLE(Ereignisverwalter, wxEvtHandler)
    EVT_NEUES_FENSTER(wxID_ANY, Ereignisverwalter::BeiNeuesFenster)
END_EVENT_TABLE()

//{}
bool Zentrale::OnInit()
{
    Ereignisvw = new Ereignisverwalter; // einen Ereignisverwalter erzeugen
    
    
    
    // neuen Socket erzeugen (unter Windows liefert dies WSAData...?)
    new wxSocketClient();
    
    // dem Zeiger irc eine Instanz des IRCInterfaces zuweisen
    irc = new IRCInterface(_T("6667"),_T("irc.freenode.net"),_T("efirc_test"),_T("efirc_test"),_T("efirc_test"),_T("PASS"));
    //irc = new IRCInterface(_T("6667"),_T("localhost"),_T("efirc_test"),_T("efirc_test"),_T("efirc_test"),_T("PASS"));

    // FENSTER
    // dafuer sorgen, dass kein zeiger festgelegt ist
    for(int i=0;i<10;i++) { zgr_fenster[i]=NULL; }
    
    
    // erste Instanz der Fenster-klasse erzeugen.
    // VERBINDUNG ZUR KONFIGURATION
    raum = "#efirc";
    neuesFenster(raum);
    
    
    // Verlinkung der IRC-Funktionen starten und IRC Threads starten
    // Eine Instanz der Fensterklasse muss erzeugt sein
    connect_thread(); 
    
    
    
    // TEST FUNKTIONEN

    return TRUE;
} 


void Ereignisverwalter::BeiNeuesFenster(wxCommandEvent& event)
{
    wxGetApp().neuesFenster(event.GetString());
}

// Funktionen die auf die Fenster-KLasse zugreifen bzw. auf Instanzen eben dieser
void Zentrale::neuesFenster(wxString namedesfensters)
{
    bool fenstererzeugt = false;
    
    // nach freier Nummer suchen
    int i = 0;
    for(int j = 0; j<10 ; j++)
    {
        if(fenstername[j] == namedesfensters)
        fenstererzeugt = true;
    }
    while(fenstererzeugt == false && i <10)
    {
        if(zgr_fenster[i]==NULL)
        {
                // i ist auch die Fensternummer und die Window ID
                zgr_fenster[i] = new Fenster( namedesfensters, i, wxPoint(8, 8), wxSize(566, 341) ); // neue instanz erzeugen, i = id des fensters / window
                fenstername[i] = namedesfensters;
                zgr_fenster[i]->fensternummer = i;
                zgr_fenster[i]->Show(TRUE); // Fenster in den Vordergrund holen
                SetTopWindow(zgr_fenster[i]); // Fenster zum obersten Objekt machen
                
                // Anpassungsfunktionen:
                zgr_fenster[i]->TitelSetzen(namedesfensters); // Titel anpassen
                
                fenstererzeugt = true;
        }
        i++;
    }
    
    if(fenstererzeugt == false)
    {
        //oberstes fenster heraussuchen , dazu aktuellen Nickname als Parameter uebergeben
        fenstersuchen(irc->CurrentNick)->Fehler(1,namedesfensters);
    }
    
}

void Zentrale::fensterzerstoeren(int fensternummer)
{
    //irc->send_part((fenstername[fensternummer]).c_str());
    zgr_fenster[fensternummer]->Destroy();
    zgr_fenster[fensternummer] = NULL;
    fenstername[fensternummer] = _T("");
}

void Zentrale::fensterzerstoeren(wxString namedesfensters)
{
    for(int i = 0;i<10;i++)
    {
        if(fenstername[i] == namedesfensters)
        {
            //irc->send_part((fenstername[i]).c_str());
            zgr_fenster[i]->Destroy();
            zgr_fenster[i] = NULL;
            fenstername[i] = _T("");
        }
    }
}

// Gibt IMMER einen Zeiger auf ein Fenster zurueck
Fenster* Zentrale::fenstersuchen(wxString name)
{
    Fenster *zgr = NULL; // Zeiger auf Fenster
    
    // Wenn der Name der aktuelle Nickname ist, dann wird die Nachricht im obersten Fenster angezeigt
    if(name == irc->CurrentNick)
    {
        // mit SetTopWindow zuletzt als oberstes Fenster festgelegtes Fenster suchen 
        // und Zeiger casten, damit er dem Rueckgabetyp "Fenster*" entspricht
        return dynamic_cast<Fenster*>(GetTopWindow()); 
    }
    else
    {
        int i = 0; // laufende Nummer auf 0 setzen
        while(i < 10 && zgr == NULL) // solange i kleiner 10 ist und der zeiger noch null ist wird der inhalt der schleife ausgefuehrt
        {
            if(fenstername[i] == name) // sobald der name uebereinstimmt wird der zeiger auf diesen frame zurueckgegeben
            {
                return zgr_fenster[i];
            }
            if(i==9)
            // falls keine uebereinstimmung gefunden wurde, Fehler ausgeben und oberstes Fenster als Ausgabe setzten
            {
                // mit SetTopWindow zuletzt als oberstes Fenster festgelegtes Fenster suchen 
                // und Zeiger casten, damit er dem Rueckgabetyp "Fenster*" entspricht
                zgr = dynamic_cast<Fenster*>(GetTopWindow());
                zgr->Fehler(3,_T(name));
                return zgr;
            }
            i++;
        }
    }
} 

// Gibt  IMMER einen Zeiger auf ein Fenster zurueck
// DARF nur mit VORHANDENEN Fenstern benutzt werden
Fenster* Zentrale::fenster(wxString name)
{
    while(1)
    {
        // fuer jeden Raum durchmachen da der name in jedem raum geaendert werden muss
        for(int i = 0; i<10; i++)
        {
            if(fenstername[i] == _T(name))
            {
                return zgr_fenster[i];
            }
        }
        // wenn kein Fenster gefunden wurde wird ein neues erstellt
        //neuesFenster(name);
    }
}


void Zentrale::BefehlVerarbeiten(int fensternummer, wxString befehl)
{
    // fenstername[fensternummer]
    wxChar leerzeichen = _T(' ');
    wxString befehl_name = befehl.BeforeFirst(leerzeichen);
    wxString befehl_parameter = befehl.AfterFirst(leerzeichen);
    
    if(befehl_name.Upper() == _T("QUIT"))
    {
        wxString quitmessage = _T("ef!rc");
        irc->disconnect_server(quitmessage.c_str());
        // ALLE FENSTER ZERSTOEREN
    }

    if(befehl_name.Upper() == _T("JOIN") && befehl_parameter != _T(""))
    {
        irc->send_join(befehl_parameter.c_str());
        // Auf eigenen JOIN Warten und dann neues Fenster aufmachen
    }
    
    if(befehl_name.Upper() == _T("PART") || befehl_name.Upper() == _T("LEAVE"))
    {
        if(befehl_parameter == _T(""))
        {
            //fensterzerstoeren(fensternummer);
            irc->send_part(fenstername[fensternummer].c_str());
        }
        else
        {
            //fensterzerstoeren(befehl_parameter);
            irc->send_part(befehl_parameter.c_str());
        }
    }
    
    if(befehl_name.Upper() == _T("NICK") && befehl_parameter != _T(""))
    {
            irc->send_nick(befehl_parameter.c_str()); // Nickname senden
            irc->WantedNick = befehl_parameter; 
            // gewollten Nickname speichern, damit die nickinuse-Funktion richtig reagieren kann.
    }
    
    if(befehl_name.Upper() == _T("ME") && befehl_parameter != _T(""))
    {
        // GEHT NOCH NICHT BEI NACHRICHTEN AN BENUTZER
        wxString me_text = _T("\001ACTION " + befehl_parameter + "\001");
        irc->send_privmsg(fenstername[fensternummer].c_str(), me_text.c_str());
        zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("ACTION"), irc->CurrentNick, befehl_parameter);
    }
    
    if(befehl_name.Upper() == _T("TOPIC"))
    {
        if(befehl_parameter == _T(""))
        {
            irc->send_topic(fenstername[fensternummer].c_str());
        }
        else
        {
            irc->send_topic(fenstername[fensternummer].c_str(),befehl_parameter.c_str());
        }
    }

    if((befehl_name.Upper() == _T("QUERY") || befehl_name.Upper() == _T("MSG")) && befehl_parameter != _T(""))
    {
        wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
        wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);
        
        irc->send_privmsg(empfaenger.c_str(),nachricht.c_str());
        zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("P_PRIVMSG"),irc->CurrentNick, empfaenger, nachricht);
    }
    
    if(befehl_name.Upper() == _T("AWAY"))
    {
        if(befehl_parameter == _T(""))
        {
            irc->send_away();
            zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("AWAY"));
        }
        else
        {
            irc->send_away(befehl_parameter.c_str());
            zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("AWAY"),befehl_parameter);
            
        }
    }
    
    
    if(befehl_name.Upper() == _T("CTCP") && befehl_parameter != _T(""))
    {
        wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
        wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);
        
        irc->send_privmsg(empfaenger.c_str(), (_T("\001" + nachricht + "\001")).c_str());
        
        zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("CTCP"),irc->CurrentNick, empfaenger, nachricht);
    }
    
    
    if(befehl_name.Upper() == _T("WHOIS") && befehl_parameter != _T(""))
    {
        irc->send_whois(befehl_parameter.c_str());
    }
    
    // sonstige Befehle
    if(befehl_name.Upper() == _T("CLEAR"))
    {
        zgr_fenster[fensternummer]->AusgabefeldLeeren();
    }
    
}

void Zentrale::NachrichtSenden(int fensternummer, wxString nachricht)
{
    irc->send_privmsg(fenstername[fensternummer].c_str(), nachricht.c_str());
    // Nachricht im Textfenster anzeigen
    // UNICODE?
    //AKTUELLER NICKNAME?
    zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("PRIVMSG"),irc->CurrentNick,nachricht);
}

void Zentrale::EingabeVerarbeiten(int fensternummer, wxString eingabe)
{
    //Ist es ein Befehl?
    wxString befehlsprefix = _T("/");
    if(eingabe.StartsWith(befehlsprefix.c_str(), &eingabe))
    {
        BefehlVerarbeiten(fensternummer, eingabe.mb_str(wxConvUTF8));
    }
    // wenn nicht an raum/benutzer senden
    else
    {
        NachrichtSenden(fensternummer,eingabe.mb_str(wxConvUTF8));
    }
}



// THREADS
// THREADS
// THREADS
// Bevor die Funktionen die mit der IRC-Schnittstelle verbunden werden verwendet werden, muessen 
// sie bekannt gemacht sein
void irc_pmsg(const irc_msg_data *msg_data, void *cp);
void irc_mode(const irc_msg_data *msg_data, void *cp);
void irc_endofmotd(const irc_msg_data *msg_data, void *cp);
void irc_motd(const irc_msg_data *msg_data, void *cp);
void irc_topic(const irc_msg_data *msg_data, void *cp);
void irc_requestedtopic(const irc_msg_data *msg_data, void *cp);
void irc_error(const irc_msg_data *msg_data, void *cp);
void irc_pong(const irc_msg_data *msg_data, void *cp);
void irc_whoisuser(const irc_msg_data *msg_data, void *cp);
void irc_whoisaway(const irc_msg_data *msg_data, void *cp);
void irc_whoischan(const irc_msg_data *msg_data, void *cp);
void irc_whoisidle(const irc_msg_data *msg_data, void *cp);
void irc_whoisserver(const irc_msg_data *msg_data, void *cp);
void irc_userlist(const irc_msg_data *msg_data, void *cp);
void irc_join(const irc_msg_data *msg_data, void *cp);
void irc_leave(const irc_msg_data *msg_data, void *cp);
void irc_quit(const irc_msg_data *msg_data, void *cp);
void irc_nick(const irc_msg_data *msg_data, void *cp);
void irc_nickinuse(const irc_msg_data *msg_data, void *cp);
void irc_unaway(const irc_msg_data *msg_data, void *cp);
void irc_nowaway(const irc_msg_data *msg_data, void *cp);
// SOLLEN MITGLIEDER VON ZENTRALE SEIN; DANN DURCH zentrale.h SCHON BEKANNT

void Zentrale::connect_thread()
{
    // Ereignisverknüpfung
    // TODO wirklich Ereignisse implementieren

    irc->add_link("PRIVMSG", &irc_pmsg);
    irc->add_link("NOTICE", &irc_pmsg);
    
    irc->add_link("MODE", &irc_mode);
    
    irc->add_link("301", &irc_whoisaway);
    irc->add_link("305", &irc_unaway);
    irc->add_link("306", &irc_nowaway);
    irc->add_link("311", &irc_whoisuser);
    irc->add_link("312", &irc_whoisserver);
    irc->add_link("317", &irc_whoisidle);
    irc->add_link("319", &irc_whoischan);
    
    irc->add_link("372", &irc_motd);
    irc->add_link("376", &irc_endofmotd);
    irc->add_link("332", &irc_topic);
    irc->add_link("TOPIC", &irc_requestedtopic);
    irc->add_link("353", &irc_userlist);
    irc->add_link("JOIN", &irc_join);
    irc->add_link("PART", &irc_leave);
    irc->add_link("QUIT", &irc_quit);
    irc->add_link("NICK", &irc_nick);
    irc->add_link("PING", &irc_pong);
    //irc->add_link("KICK", &irc_kick);

    // FEHLER
    // Fehler Antworten
    irc->add_link("401", &irc_error);
    irc->add_link("402", &irc_error);
    irc->add_link("403", &irc_error);
    irc->add_link("404", &irc_error);
    irc->add_link("405", &irc_error);
    irc->add_link("406", &irc_error);
    irc->add_link("407", &irc_error);
    irc->add_link("409", &irc_error);
    irc->add_link("411", &irc_error);
    irc->add_link("412", &irc_error);
    irc->add_link("413", &irc_error);
    irc->add_link("414", &irc_error);
    irc->add_link("421", &irc_error);
    irc->add_link("422", &irc_error);
    irc->add_link("423", &irc_error);
    irc->add_link("424", &irc_error);
    irc->add_link("431", &irc_error);
    irc->add_link("432", &irc_error);
    irc->add_link("433", &irc_nickinuse);
    irc->add_link("436", &irc_error);
    irc->add_link("441", &irc_error);
    irc->add_link("442", &irc_error);
    irc->add_link("443", &irc_error);
    irc->add_link("444", &irc_error);
    irc->add_link("445", &irc_error);
    irc->add_link("446", &irc_error);
    irc->add_link("451", &irc_error);
    irc->add_link("461", &irc_error);
    irc->add_link("462", &irc_error);
    irc->add_link("463", &irc_error);
    irc->add_link("464", &irc_error);
    irc->add_link("465", &irc_error);
    irc->add_link("467", &irc_error);
    irc->add_link("471", &irc_error);
    irc->add_link("472", &irc_error);
    irc->add_link("473", &irc_error);
    irc->add_link("474", &irc_error);
    irc->add_link("475", &irc_error);
    irc->add_link("481", &irc_error);
    irc->add_link("482", &irc_error);
    irc->add_link("483", &irc_error);
    irc->add_link("491", &irc_error);
    irc->add_link("501", &irc_error);
    irc->add_link("502", &irc_error);
    
    
    irc->connect();
    
    Thread *thread = new Thread(&Zentrale::recv_thread); // Thread für recv_thread starten
    Thread *thread2 = new Thread(&Zentrale::call_thread); // Thread fuer call_thread starten
    if (thread->Create() == wxTHREAD_NO_ERROR) { thread->Run(); }
    if (thread2->Create() == wxTHREAD_NO_ERROR) { thread2->Run(); }
    
}

// Thread-Funktion fuer recv_raw-Schleife
void Zentrale::recv_thread()
{
    irc->recv_raw();
}

// Thread-Funkton fuer Abarbeitung der Befehlsschlange
void Zentrale::call_thread()
{
    irc->call_cmd();
}

int Zentrale::OnExit()
{
    wxString quitmessage = _T("ef!rc");
    irc->disconnect_server(quitmessage.c_str());
}
