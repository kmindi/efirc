// zentrale.cpp
// Implementiert die Klasse Zentrale

#include <zentrale.h>

// main funkition erstellen, Instanz der Klasse Zentrale erstellen
// mit DECLARE_APP(Zentrale/) und wxGetApp() kann auf Funktionen dieser Klasse zugegrifffen werden
IMPLEMENT_APP(Zentrale) // erstellt main funktion

bool Zentrale::OnInit()
{
    // neuen Socket erzeugen (unter Windows liefert dies WSAData...?)
    new wxSocketClient();
    
    // dem Zeiger irc eine Instanz des IRCInterfaces zuweisen
    irc = new IRCInterface(_T("6667"),_T("irc.freenode.net"),_T("efirc_test"),_T("efirc_test"),_T("efirc_test"),_T("PASS"));
    
    // Verlinkung der IRC-Funktionen starten und IRC Threads starten
    connect_thread(); 
    
    // FENSTER
    // dafuer sorgen, dass kein zeiger festgelegt ist
    for(int i=0;i<10;i++) { zgr_fenster[i]=NULL; }
    
    
    // erste Instanz der Fenster-klasse erzeugen.
    // VERBINDUNG ZUR KONFIGURATION
    raum = "#efirc";
    neuesFenster(raum);
    
    
    // TEST FUNKTIONEN
    neuesFenster("tescht");
    fenstersuchen("tescht")->Fehler(1); // fehleranzeigetest

    
    return TRUE;
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
    irc->send_part((fenstername[fensternummer]).c_str());
    zgr_fenster[fensternummer]->Destroy();
    zgr_fenster[fensternummer]=NULL;
}

void Zentrale::fensterzerstoeren(wxString namedesfensters)
{
    for(int i = 0;i<10;i++)
    {
        if(fenstername[i] == namedesfensters)
        {
            irc->send_part((fenstername[i]).c_str());
            zgr_fenster[i]->Destroy();
            zgr_fenster[i]=NULL;
        }
    }
}

// Gibt einen Zeiger auf ein Fenster zurueck
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
            if(fenstername[i]==name) // sobald der name uebereinstimmt wird der zeiger auf diesen frame zurueckgegeben
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

void Zentrale::BefehlVerarbeiten(int fensternummer, wxString befehl)
{
    // fenstername[fensternummer]
    wxChar leerzeichen = _T(' ');
    wxString befehl_name = befehl.BeforeFirst(leerzeichen);
    wxString befehl_parameter = befehl.AfterFirst(leerzeichen);
    
    if(befehl_name.Upper() == "QUIT")
    {
        // alle Fenster zerstoeren        
    }

    if(befehl_name.Upper() == "JOIN" && befehl_parameter != "")
    {
        irc->send_join(befehl_parameter.c_str());
        // Auf eigenen JOIN Warten und dann neues Fenster aufmachen
    }
    
    if(befehl_name.Upper() == "PART" || befehl_name.Upper() == "LEAVE")
    {
        if(befehl_parameter == "")
        {
            fensterzerstoeren(fensternummer);
        }
        else
        {
            fensterzerstoeren(befehl_parameter);
        }
    }
    
    if(befehl_name.Upper() == "NICK" && befehl_parameter != "")
    {
            irc->send_nick(befehl_parameter.c_str());
            // Aktuellen Nick manuell veraendern
            // damit die nickinuse-Funktion den
            // gewollten nick als aktuellen nickname
            // erhaelt
            // irc->WantedNick = param;
            
            // wenn kein nickname gesetzt wurde (also der nick user_QIFS oder so ist)
            // dann weiterversuchen
            // andernfalls nachricht anzeigen und an den nick _ anhaehgen nach zwei versuchen 
            // aufforderung zum selber eingeben anzeigen, auf NICK neuernick warten
    }
    
    if(befehl_name.Upper() == "ME" && befehl_parameter != "")
    {
        // GEHT NOCH NICHT BEI NACHRICHTEN AN BENUTZER
        wxString me_text = _T("\001ACTION " + befehl_parameter + "\001");
        irc->send_privmsg(fenstername[fensternummer].c_str(), me_text.c_str());
        zgr_fenster[fensternummer]->NachrichtAnhaengen("ACTION", irc->CurrentNick, befehl_parameter);
    }
    
    if(befehl_name.Upper() == "TOPIC")
    {
        if(befehl_parameter == "")
        {
            irc->send_topic(fenstername[fensternummer].c_str());
        }
        else
        {
            irc->send_topic(fenstername[fensternummer].c_str(),befehl_parameter.c_str());
        }
    }

    if((befehl_name.Upper() == "QUERY" || befehl_name.Upper() == "MSG") && befehl_parameter != "")
    {
        wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
        wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);
        
        irc->send_privmsg(empfaenger.c_str(),nachricht.c_str());
        zgr_fenster[fensternummer]->NachrichtAnhaengen("P_PRIVMSG",irc->CurrentNick, empfaenger, nachricht);
    }
    
    if(befehl_name.Upper() == "AWAY")
    {
        if(befehl_parameter == "")
        {
            irc->send_away();
            zgr_fenster[fensternummer]->NachrichtAnhaengen("AWAY");
        }
        else
        {
            irc->send_away(befehl_parameter.c_str());
            zgr_fenster[fensternummer]->NachrichtAnhaengen("AWAY",befehl_parameter);
            
        }
    }
    
    
    if(befehl_name.Upper() == "CTCP" && befehl_parameter != "")
    {
        wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
        wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);
        
        irc->send_privmsg(empfaenger.c_str(), (_T("\001" + nachricht + "\001")).c_str());
        
        zgr_fenster[fensternummer]->NachrichtAnhaengen("CTCP",irc->CurrentNick, empfaenger, nachricht);
    }
    
    
    if(befehl_name.Upper() == "WHOIS" && befehl_parameter != "")
    {
        irc->send_whois(befehl_parameter.c_str());
    }
    
    // sonstige Befehle
    if(befehl_name.Upper() == "CLEAR")
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
    zgr_fenster[fensternummer]->NachrichtAnhaengen("PRIVMSG",irc->CurrentNick,nachricht);
}

void Zentrale::EingabeVerarbeiten(int fensternummer, wxString eingabe)
{
    //Ist es ein Befehl?
    wxString befehlsprefix = "/";
    if(eingabe.StartsWith(befehlsprefix.c_str(), &eingabe))
    {
        BefehlVerarbeiten(fensternummer, eingabe);
    }
    // wenn nicht an raum/benutzer senden
    else
    {
        NachrichtSenden(fensternummer,eingabe);
    }
}



// THREADS
// THREADS
// THREADS
// Bevor die Funktionen die mit der IRC-Schnittstelle verbunden werden verwendet werden, muessen 
// sie bekannt gemacht sein
void irc_pmsg(const irc_msg_data *msg_data, void *cp);
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
void irc_nick(const irc_msg_data *msg_data, void *cp);
void irc_unaway(const irc_msg_data *msg_data, void *cp);
void irc_nowaway(const irc_msg_data *msg_data, void *cp);
// SOLLEN MITGLIEDER VON ZENTRALE SEIN; DANN DURCH zentrale.h SCHON BEKANNT

void Zentrale::connect_thread()
{
    irc->connect();

    // Ereignisverknüpfung
    // TODO wirklich Ereignisse implementieren

    irc->add_link("PRIVMSG", &irc_pmsg);
    irc->add_link("NOTICE", &irc_pmsg);
    
    //irc->add_link("MODE", &irc_mode);
    
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
    //irc->add_link("PART", &irc_leave);
    //irc->add_link("QUIT", &irc_quit);
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
    irc->add_link("433", &irc_error); //nick in use
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
    wxString quitmessage = wxT("ef!rc");
    irc->disconnect_server(quitmessage.c_str());
}
