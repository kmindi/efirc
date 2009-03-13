/**
** efirc steht unter der "Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland Lizenz".
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the "Creative Commons Attribution-Share Alike 3.0 Germany License".
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// irc_event_functions.cc
// enthaelt die Funktionen die mit der IRC-Schnittstelle verbunden werden und von dort aufgerufen werden
// FUNKTIONEN SOLLEN MITGLIEDER VON ZENTRALE SEIN
#include <zentrale.h>

DECLARE_APP(Zentrale) //braucht man fuer wxGetApp um damit auf die funktionen der Zentrale zuzugreifen


// Benutzerdefinierte Ereignisse deklarieren und definieren
DECLARE_EVENT_TYPE(wxEVT_NEUER_IRC_BEFEHL, -1)
DEFINE_EVENT_TYPE(wxEVT_NEUER_IRC_BEFEHL)

#define EVT_NEUER_IRC_BEFEHL(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_NEUER_IRC_BEFEHL, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

    
BEGIN_EVENT_TABLE(Ereignisverwalter, wxEvtHandler)
    EVT_NEUER_IRC_BEFEHL(wxID_ANY, Ereignisverwalter::BeiNeueIRCNachricht)
END_EVENT_TABLE()    



// allgemeine Funktion fuer alle ankommenden Befehle.
void irc_allgemein(const irc_msg_data *msg_data, void *cp)
{
    wxCommandEvent eventCustom(wxEVT_NEUER_IRC_BEFEHL); // Neues Ereignis erzeugen
    IRC_NACHRICHT *zgr_ircn = new IRC_NACHRICHT(msg_data); // Instanz der IRC_NACHRICHT-Klasse erstellen und merken
    eventCustom.SetClientData(zgr_ircn); // Dem Ereignis die Position der Daten mitteilen
    wxPostEvent(wxGetApp().Ereignisvw, eventCustom); // Ereignis ausloesen, bzw in die Abarbeitungswarteschlange einreihen
}


void Ereignisverwalter::BeiNeueIRCNachricht(wxCommandEvent& event)
{
    // Daten aus dem Ereignis einem eigenen Zeiger zuweisen.
    
    const IRC_NACHRICHT *msg_data = (IRC_NACHRICHT *)event.GetClientData();
    
    // Feld fuer Befehle, damit man mit switch case default durchgehen kann
    // Fuer hoehere Geschwindigkeit und einfachere Anpassbarkeit
    
    wxString cmd(msg_data->cmd, wxConvUTF8); // Befehl in Variable speichern
    
    // Befehle Abfragen und entsprechende Funktionen aufrufen
    
        // PRIVMSG und NOTICE
        if(cmd == _T("PRIVMSG") || cmd == _T("NOTICE"))
        {
            wxGetApp().irc_pmsg(msg_data);
        }
        
        // Willkommensnachrichten 001 002 003 und 004
        else if(cmd == _T("001") || cmd == _T("002") || cmd == _T("003") || cmd == _T("004"))
        {
            wxGetApp().irc_welcome(msg_data);
        }
        
        else if(cmd == _T("005"))
        {
            wxGetApp().irc_isupport(msg_data);
        }
        
        else if(cmd == _T("JOIN"))
        {
            wxGetApp().irc_join(msg_data);
        }
        
        else if(cmd == _T("PART"))
        {
            wxGetApp().irc_leave(msg_data);
        }
        
        else if(cmd == _T("QUIT"))
        {
            wxGetApp().irc_quit(msg_data);
        }
        
        else if(cmd == _T("NICK"))
        {
            wxGetApp().irc_nick(msg_data);
        }
        
        // Angeforderter Benutzername wird bereits benutzt
        else if(cmd == _T("433"))
        {
            wxGetApp().irc_nickinuse(msg_data);
        }
        
        else if(cmd == _T("INVITE"))
        {
            wxGetApp().irc_invite(msg_data);
        }
        
        // Thema des Raums anzeigen
        else if(cmd == _T("332"))
        {
            wxGetApp().irc_topic(msg_data);
        }
        
        else if(cmd == _T("TOPIC"))
        {
            wxGetApp().irc_requestedtopic(msg_data);
        }
        
        // Benutzerliste
        else if(cmd == _T("353"))
        {
            wxGetApp().irc_userlist(msg_data);
        }
        
        else if(cmd == _T("MODE"))
        {
            wxGetApp().irc_mode(msg_data);
        }
        
        // RPL_UNAWAY
        else if(cmd == _T("305"))
        {
            wxGetApp().irc_unaway(msg_data);
        }
        
        // RPL_NOWAWAY
        else if(cmd == _T("306"))
        {
            wxGetApp().irc_nowaway(msg_data);
        }
        
        // PING PONG
        else if(cmd == _T("PING"))
        {
            wxGetApp().irc_pong(msg_data);
        }
        
        // Message of the day anzeigen
        else if(cmd == _T("372") || cmd == _T("375"))
        {
            wxGetApp().irc_motd(msg_data);
        }
        
        // END OF MOTD und ersten Raum betreten
        else if(cmd == _T("376")) 
        {
            wxGetApp().irc_endofmotd(msg_data);
        }

        // WHOIS - USER
        else if(cmd == _T("311"))
        {
            wxGetApp().irc_whoisuser(msg_data);
        }
        
        // WHOIS - ABWESEND
        else if(cmd == _T("301"))
        {
            wxGetApp().irc_whoisaway(msg_data);
        }
        
        // WHOIS - RAEUME
        else if(cmd == _T("319"))
        {
            wxGetApp().irc_whoischan(msg_data);
        }
        
        // WHOIS - UNTAETIG
        else if(cmd == _T("317"))
        {
            wxGetApp().irc_whoisidle(msg_data);
        }
        
        // WHOIS - SERVERINFO
        else if(cmd == _T("312"))
        {
            wxGetApp().irc_whoisserver(msg_data);
        }
        
        // WHOIS - BENUTZT HOST
        else if(cmd == _T("338"))
        {
            wxGetApp().irc_whoisactually(msg_data);
        }
        
        
        // Nachrichten die nicht angezeigt werden sollen
        else if(cmd == _T("318") || cmd == _T("366"))
        {
            // 318 End of /WHOIS List
            // 366 End of /NAMES List
        }
        
        // Abfrage nach Nachrichten die einfach nur ausgegeben werden sollen.
        else if(cmd == _T("042") || cmd == _T("250") || cmd == _T("251") || cmd == _T("252") || cmd == _T("253") || cmd == _T("254") | cmd == _T("255"))
        {
            // 042 Unique ID
            // 250 RPL_STATSCONN
            // 251 RPL_LUSERCLIENT
            // 252 RPL_LUSEROP
            // 253 RPL_LUSERUNKOWN
            // 254 RPL_LUSERCHANNELS
            // 255 RPL_LUSERME
            
            wxGetApp().irc_einfach(msg_data);
        }
        
        // Abfrage nach ERR_* Antworten
        else if(cmd == _T("300"))
        {
            wxGetApp().irc_fehler(msg_data);
        }
        
        // wenn keine Uebereinstimmung gefunden wurde, Fehler anzeigen
        else
        {
            wxGetApp().irc_unbekannt(msg_data);
        }     
        
        // Angeforderten Speicher wieder freigeben
        delete msg_data;
}



// Empfangene normale Nachrichten werden ausgegeben
void Zentrale::irc_pmsg(const IRC_NACHRICHT *msg_data)
{
    wxChar leerzeichen = _T(' ');

    wxString text(msg_data->params_a[1], wxConvUTF8);
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString user(msg_data->nick, wxConvUTF8);
    wxString prefix = _T("\001");

    // CTCP abfragen ("'\001'BEFEHL text'\001'")
    //Falls kein CTCP:

    if (!(text.StartsWith(prefix) && text.EndsWith(prefix)))
    {
            if(empfaenger.Upper() == irc->CurrentNick.Upper())
            // wenn man selber der empfaenger ist
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,text);
            }
            else
            {
                if(user == _T(""))
                {
                    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),user,text);
                }
                else
                {
                    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG"),user,text);
                }
            }
    }

    //CTCP behandeln
    else
    {
        // Text aufteilen
        wxString ctcp(text.Mid(1,text.Len()-2), wxConvUTF8); // \001 vorne und hinten abschneiden
        wxString ctcp_befehl(ctcp.BeforeFirst(leerzeichen), wxConvUTF8);
        wxString ctcp_text(ctcp.AfterFirst(leerzeichen), wxConvUTF8);

        //Befehle abfragen
        // MIT SWITCH / CASE -> schneller

        if(ctcp_befehl == _T("ACTION"))
        {
            if(empfaenger.Upper() == irc->CurrentNick.Upper())
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_ACTION"),user,ctcp_text);
            }
            else
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("ACTION"),user,ctcp_text);
            }
        }

        if(ctcp_befehl == _T("VERSION"))
        {

            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster ausw�hlen
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString version = _T("efirc:") + efirc_version_string + _T(" (") + wxString(wxVERSION_STRING,wxConvUTF8) + _T("):") + wxGetOsDescription();
                irc->send_ctcp_version(user.mb_str(), version.mb_str());
            }
            else
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        if(ctcp_befehl == _T("FINGER"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                // FINGER ANTWORT?
            }
            else
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        if(ctcp_befehl == _T("SOURCE"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster ausw�hlen
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString quelle = _T("http://efirc.sourceforge.net/");
                irc->send_ctcp_source(user.mb_str(), quelle.mb_str());
            }
            else
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        if(ctcp_befehl == _T("USERINFO"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                // ANTWORT?
            }
            else
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        if(ctcp_befehl == _T("CLIENTINFO"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster ausw�hlen
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString antwort = _T("VERSION FINGER SOURCE USERINFO CLIENTINFO PING TIME");
                irc->send_ctcp_clientinfo(user.mb_str(), antwort.mb_str());
            }
            else
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        // PING FORMAT IMMER MIT ZEITSTEMPEL
        // UNTERSCHIED IST NUR PRIVMSG ZUM ANFORDERN UND NOTICE ALS ANTWORT
        if(ctcp_befehl == _T("PING"))
        {
            fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
            //ANTWORT?
        }


        if(ctcp_befehl == _T("TIME"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster ausw�hlen
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);

                char timestamp[30];
                time_t raw_time;
                tm *local_time;

                time(&raw_time);
                local_time = localtime(&raw_time);

                strftime(timestamp, 30, "%d.%m.%Y %H:%M:%S", local_time);

                wxString timewxString(timestamp, wxConvUTF8);
                wxString antwort = timewxString;

                irc->send_ctcp_time(user.mb_str(), antwort.mb_str());
            }
            else
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }
    }

}

// Willkommensnachrichten 001 002 003 und 004
void Zentrale::irc_welcome(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nachricht = _T("");
    for(int i = 1; i < msg_data->params_i; i++)
    {
        nachricht += _T(" ");
        nachricht += wxString(msg_data->params_a[i], wxConvUTF8);
    }
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),_T(""), nachricht);

    // bei 001 ist der aktuelle Nickname gleich dem Empfaenger der Nachricht (man selber)
    if(wxString(msg_data->cmd, wxConvUTF8) == _T("001"))
    {
        // fuer jeden Raum durchmachen da der name in jedem raum geaendert werden muss
        for(int i = 0; i < max_fenster; i++)
        {
            if(!(zgr_fenster[i]==NULL))
            // nicht in nicht vorhandenen Fenstern
            {
                    irc->CurrentNick = empfaenger;
                    zgr_fenster[i]->TitelSetzen(fenstername[i], empfaenger);
            }
        }
    }
}

// RPL_ISUPPORT
void Zentrale::irc_isupport(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nachricht = _T("");
    for(int i = 1; i < msg_data->params_i; i++)
    {
        nachricht += _T(" ");
        nachricht += wxString(msg_data->params_a[i], wxConvUTF8);
    }
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),_T(""), nachricht);
}

void Zentrale::irc_mode(const IRC_NACHRICHT *msg_data)
{
    // NOCH HERAUSSUCHEN WENN +o und +v (=> @ und + vor nickname bzw chanop und voice)


    wxString Sender(msg_data->nick, wxConvUTF8);
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString Parameter = _T("");
    wxString Raum = _T("");
    wxString Modus = _T("");

    if(Sender == _T(""))
    {
        Sender = wxString(msg_data->sender, wxConvUTF8);
    }

    for(int i = 0; i < msg_data->params_i; i++)
    {
        if(msg_data->params_a[i][0] == _T('#'))
        {
            Raum = _T(" (");
            Raum += wxString(msg_data->params_a[i], wxConvUTF8);
            Raum += _T(")");
        }
        else if(msg_data->params_a[i][0] == _T('+') ||
                msg_data->params_a[i][0] == _T('-'))
        {
            Modus = wxString(msg_data->params_a[i], wxConvUTF8);
        }
        else
        {
            Parameter += _T(" ") + wxString(msg_data->params_a[i], wxConvUTF8);
        }
    }

    Modus += Parameter + Raum;

    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("MODE"), Sender, Modus);
}

// Message of the day anzeigen
void Zentrale::irc_motd(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("MOTD"),wxString(msg_data->params_a[1], wxConvUTF8));
}

// Am Ende der Nachricht des Tages automatisch einen Raum betreten
void Zentrale::irc_endofmotd(const IRC_NACHRICHT *msg_data)
{
    // den in der Konfigurationsdatei genannten Kanal betreten
    irc->send_join(config->parsecfgvalue(_T("irc_channel")).mb_str());
}

// Benutzerliste einlesen
void Zentrale::irc_userlist(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[2], wxConvUTF8);
    wxString benutzerliste;
    benutzerliste = wxString(msg_data->params_a[3], wxConvUTF8);
    fenster(empfaenger)->BenutzerHinzufuegen(benutzerliste);
}

// Benutzerliste aktualisieren / Benutzer hat den Raum betreten
void Zentrale::irc_join(const IRC_NACHRICHT *msg_data)
{

    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString benutzer(msg_data->nick, wxConvUTF8);

    if(benutzer.Upper() == irc->CurrentNick.Upper())
    // Wenn man selber der Benutzer ist, dann hat man den Raum betreten
    // Deshalb wird hier das Fenster erst erstellt und nicht schon bei der Befehlsabfrage
    // es kann ja auch sein, dass man im Raum gebannt ist oder aehnliches
    {
        neuesFenster(empfaenger);
    }
    else
    // Andernfalls ist es logischerweise ein neuer Benutzer der den Raum betreten hat
    {
        fenster(empfaenger)->BenutzerHinzufuegen(benutzer);
        fenster(empfaenger)->NachrichtAnhaengen(_T("JOIN"),benutzer);
    }

}

// Benutzerliste aktualisieren / Benutzer hat den Raum verlassen
void Zentrale::irc_leave(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8); // = ein Raum
    wxString nachricht(msg_data->params_a[1], wxConvUTF8);
    wxString benutzer(msg_data->nick, wxConvUTF8);

    if(benutzer.Upper() == irc->CurrentNick.Upper())
    // wenn man selber den Raum verlaesst
    {
        fensterzerstoeren(empfaenger);
    }
    else
    {
        fenster(empfaenger)->BenutzerEntfernen(benutzer);
        fenster(empfaenger)->NachrichtAnhaengen(_T("PART"), benutzer, nachricht);
    }

}

void Zentrale::irc_quit(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(irc->CurrentNick, wxConvUTF8);
    wxString benutzer(msg_data->nick, wxConvUTF8);
    wxString nachricht(msg_data->params_a[0], wxConvUTF8);
    bool benutzer_entfernt = false;

    // IN JEDEM FENSTER ENTFERNEN
    for(int i = 0; i < max_fenster; i++)
    {
        benutzer_entfernt = false; // In diesem Fenster wurde noch nicht versucht den Benutzer zu entfernen

        if(!(zgr_fenster[i]==NULL))
        // nicht in nicht vorhandenen Fenstern
        {
            benutzer_entfernt = zgr_fenster[i]->BenutzerEntfernen(benutzer);
            if(benutzer_entfernt = true)
            // Nachricht ausgeben falls der Benutzer in diesem Fenster entfernt werden konnte
            {
                zgr_fenster[i]->NachrichtAnhaengen(_T("QUIT"), benutzer, nachricht);
            }
        }
    }
}

// Benutzerliste aktualisieren / Benutzer hat seinen Namen geaendert
void Zentrale::irc_nick(const IRC_NACHRICHT *msg_data)
{
    //wxString empfaenger(msg_data->params_a[1], wxConvUTF8);
    wxString benutzer(msg_data->nick, wxConvUTF8);
    wxString neuername(msg_data->params_a[0], wxConvUTF8);

    // fuer jeden Raum durchmachen da der name in jedem raum geaendert werden muss
    for(int i = 0; i < max_fenster; i++)
    {
        if(!(zgr_fenster[i]==NULL))
        // nicht in nicht vorhandenen Fenstern
        {
            if(benutzer.Upper() == irc->CurrentNick.Upper() || benutzer.Upper() == irc->WantedNick.Upper())
            // Wenn man selber der Benutzer ist, dann muss der eigene Nick geaendert werden
            {
                zgr_fenster[i]->BenutzerAendern(benutzer,neuername);
                irc->CurrentNick = neuername;
                zgr_fenster[i]->TitelSetzen(fenstername[i],neuername);
                zgr_fenster[i]->NachrichtAnhaengen(_T("NICK"), benutzer, neuername);
            }
            else
            // Andernfalls ist es logischerweise ein neuer Benutzer der den Raum betreten hat
            {
                zgr_fenster[i]->BenutzerAendern(benutzer, neuername);
                // nachricht anzeigen
                zgr_fenster[i]->NachrichtAnhaengen(_T("NICK"), benutzer, neuername);
            }
        }
    }
}

// Nickname wird bereits genutzt
void Zentrale::irc_nickinuse(const IRC_NACHRICHT *msg_data)
{
    // VERBINDUNG ZUR KONFIGURATION
    // Fehlermeldung anzeigen (alternativ irc_error aufrufen hier.

    //statt diesem irc_error aufrufen
        wxString fehler(msg_data->cmd, wxConvUTF8);
        for(int i = 0; i < msg_data->params_i; i++)
        {
            fehler += _T(" ");
            fehler += wxString(msg_data->params_a[i], wxConvUTF8);
        }
        fenstersuchen(irc->WantedNick)->Fehler(2,fehler);
     //statt diesem irc_error aufrufen

    irc->WantedNick += _T("_"); // _ an den namen anhaengen
    irc->send_nick(irc->WantedNick.mb_str());
}

// Einladung in einen Raum
void Zentrale::irc_invite(const IRC_NACHRICHT *msg_data)
{
    wxString benutzer(msg_data->nick, wxConvUTF8);
    wxString raum(msg_data->params_a[1], wxConvUTF8);

    fenstersuchen(irc->CurrentNick)->NachrichtAnhaengen(_T("INVITE"),benutzer,raum);
}

// Thema des Raums anzeigen
void Zentrale::irc_topic(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[1], wxConvUTF8);
    fenster(empfaenger)->ThemaAendern(wxString(msg_data->params_a[2], wxConvUTF8));
}

// Thema des Raums anzeigens
void Zentrale::irc_requestedtopic(const IRC_NACHRICHT *msg_data)
{
   wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
   wxString benutzer(msg_data->nick, wxConvUTF8);
   fenster(empfaenger)->ThemaAendern(wxString(msg_data->params_a[1], wxConvUTF8), benutzer);
}

// Auf Ping mit Pong antworten
void Zentrale::irc_pong(const IRC_NACHRICHT *msg_data)
{
    irc->send_pong(msg_data->params_a[0].mb_str());
}

// RPL_UNAWAY / 305
void Zentrale::irc_unaway(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("RPL_UNAWAY"));
}

// RPL_NOWAWAY / 306
void Zentrale::irc_nowaway(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("RPL_NOWAWAY"));
}

//whois Antworten anzeigen

void Zentrale::irc_whoisuser(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString user(msg_data->params_a[2], wxConvUTF8);
    wxString host(msg_data->params_a[3], wxConvUTF8);
    wxString name(msg_data->params_a[5], wxConvUTF8);

    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_USER"),nick,user,host,name);
}

void Zentrale::irc_whoisaway(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString text(msg_data->params_a[2], wxConvUTF8);
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_AWAY"),nick,text);
}

void Zentrale::irc_whoischan(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString chans(msg_data->params_a[2], wxConvUTF8);

    // Rechte noch beachten ([@|+]#channel)???
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_CHANNEL"),nick,chans);
}

void Zentrale::irc_whoisidle(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString sekunden(msg_data->params_a[2], wxConvUTF8);

    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_IDLE"),nick,sekunden);
}

void Zentrale::irc_whoisserver(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString server(msg_data->params_a[2], wxConvUTF8);
    wxString servernachricht(msg_data->params_a[3], wxConvUTF8);
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_SERVERMSG"),nick,server,servernachricht);
}

void Zentrale::irc_whoisactually(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString server(msg_data->params_a[2], wxConvUTF8);
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_ACTUALLY"),nick,server);
}

// ERR_*-Nachrichten
// Verschiedene Fehlermeldungen anzeigen 
void Zentrale::irc_fehler(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString fehler(msg_data->cmd, wxConvUTF8);
    for(int i = 0; i < msg_data->params_i; i++)
    {
        fehler += _T(" ");
        fehler += wxString(msg_data->params_a[i], wxConvUTF8);
    }
    fenstersuchen(empfaenger)->Fehler(2,fehler);
}


// Einfache Anzeige von Nachrichten
void Zentrale::irc_einfach(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nachricht = _T("");
    for(int i = 1; i < msg_data->params_i; i++)
    // beginnend bei eins, damit der eigene Nickname nicht angezeigt wird.
    {
        nachricht += _T(" ");
        nachricht += wxString(msg_data->params_a[i], wxConvUTF8);
    }
    fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),_T(""), nachricht);
}

// Fehler anzeigen. Nachricht wurde nicht behandelt bzw, Befehl nicht abgefragt
void Zentrale::irc_unbekannt(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString fehler(msg_data->cmd, wxConvUTF8);
    for(int i = 0; i < msg_data->params_i; i++)
    {
        fehler += _T(" ");
        fehler += wxString(msg_data->params_a[i], wxConvUTF8);
    }
    fenstersuchen(empfaenger)->Fehler(5,fehler);
}
