// irc_event_functions.cpp
// enthaelt die Funktionen die mit der IRC-Schnittstelle verbunden werden und von dort aufgerufen werden
// FUNKTIONEN SOLLEN MITGLIEDER VON ZENTRALE SEIN
#include <zentrale.h>

DECLARE_APP(Zentrale) //braucht man fuer wxGetApp um damit auf die funktionen der Zentrale zuzugreifen


// Benutzerdefiniertes Ereignis muss hier auch bekannt sein, definiert wird es in zentrale.cpp
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_NEUES_FENSTER, 7777)
END_DECLARE_EVENT_TYPES()


// Empfangene normale Nachrichten werden ausgegeben
void irc_pmsg(const irc_msg_data *msg_data, void *cp)
{
    wxChar leerzeichen = _T(' ');
    
    wxString text = _T(msg_data->params_a[1]);
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString user = _T(msg_data->nick);
    wxString prefix = _T("\001");
    
    // CTCP abfragen ("'\001'BEFEHL text'\001'")
    //Falls kein CTCP:
    
    if (!(text.StartsWith(prefix.c_str()) && text.EndsWith(prefix.c_str())))
    {
            if(empfaenger == wxGetApp().irc->CurrentNick) 
            // wenn man selber der empfaenger ist
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,text);
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG"),user,text);
            }
    }
    
    //CTCP behandeln
    else
    {
        // Text aufteilen
        wxString ctcp = text.Mid(1,text.Len()-2); // \001 vorne und hinten abschneiden
        wxString ctcp_befehl = ctcp.BeforeFirst(leerzeichen);
        wxString ctcp_text = ctcp.AfterFirst(leerzeichen);

        //Befehle abfragen
        // MIT SWITCH / CASE -> schneller
        
        if(ctcp_befehl == _T("ACTION"))
        {
            if(empfaenger == wxGetApp().irc->CurrentNick)
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_ACTION"),user,ctcp_text);
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("ACTION"),user,ctcp_text);
            }
        }
        
        if(ctcp_befehl == _T("VERSION"))
        {
        
            if(ctcp_text == ctcp || ctcp_text == "")
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString antwort = _T("\001VERSION efirc:0.3.testing:Windoofs\001");
                wxGetApp().irc->send_notice(user.c_str(), antwort.c_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }
        
        if(ctcp_befehl == _T("FINGER"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                // FINGER ANTWORT?
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + " " + ctcp_text);
            }
        }

        if(ctcp_befehl == _T("SOURCE"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString antwort = _T("\001SOURCE http://efirc.sourceforge.net/\001");
                wxGetApp().irc->send_notice(user.c_str(), antwort.c_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        if(ctcp_befehl == _T("USERINFO"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                // ANTWORT?
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        if(ctcp_befehl == _T("CLIENTINFO"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString antwort = _T("\001CLIENTINFO VERSION FINGER SOURCE USERINFO CLIENTINFO PING TIME\001");
                wxGetApp().irc->send_notice(user.c_str(), antwort.c_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        // PING FORMAT IMMER MIT ZEITSTEMPEL
        // UNTERSCHIED IST NUR PRIVMSG ZUM ANFORDERN UND NOTICE ALS ANTWORT
        if(ctcp_befehl == _T("PING"))
        {
            wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            //ANTWORT?
        }


        if(ctcp_befehl == _T("TIME"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                
                char timestamp[30];
                time_t raw_time;
                tm *local_time;

                time(&raw_time);
                local_time = localtime(&raw_time);

                strftime(timestamp, 30, "%d.%m.%Y %H:%M:%S", local_time);

                wxString timewxString(timestamp);
                wxString antwort = _T("\001TIME " + timewxString + "\001");
                
                wxGetApp().irc->send_notice(user.c_str(), antwort.c_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }
    }

}

void irc_mode(const irc_msg_data *msg_data, void *cp)
{
    // NOCH HERAUSSUCHEN WENN +o und +v (=> @ und + vor nickname bzw chanop und voice)
    
    
    wxString Sender = _T(msg_data->nick);
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString Parameter = _T("");
    wxString Raum = _T("");
    wxString Modus = _T("");

    if(Sender == _T(""))
    {
        Sender = _T(msg_data->sender);
    }

    for(int i = 0; i < msg_data->params_i; i++)
    {
        if(msg_data->params_a[i][0] == _T('#'))
        {
            Raum = _T(" (");
            Raum += _T(msg_data->params_a[i]); 
            Raum += _T(")");
        }
        else if(msg_data->params_a[i][0] == _T('+') ||
                msg_data->params_a[i][0] == _T('-'))
        {
            Modus = msg_data->params_a[i];
        }
        else
        {
            Parameter += _T(" ") + string(msg_data->params_a[i]);
        }
    }

    Modus += Parameter + Raum;

    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("MODE"), Sender, Modus);
}

// Message of the day anzeigen
void irc_motd(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("MOTD"),_T(msg_data->params_a[1]));
}

// Am Ende der Nachricht des Tages automatisch einen Raum betreten
void irc_endofmotd(const irc_msg_data *msg_data, void *cp)
{
    // den in der Konfigurationsdatei genannten Kanal betreten
    wxGetApp().irc->send_join(wxGetApp().raum.c_str());
}

// Benutzerliste einlesen
void irc_userlist(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[2]);
    wxString benutzerliste;
    benutzerliste = _T(msg_data->params_a[3]);
    wxGetApp().fenster(empfaenger)->BenutzerHinzufuegen(benutzerliste);
}

// Benutzerliste aktualisieren / Benutzer hat den Raum betreten
void irc_join(const irc_msg_data *msg_data, void *cp)
{
    
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString benutzer = _T(msg_data->nick);

    if(benutzer == wxGetApp().irc->CurrentNick)
    // Wenn man selber der Benutzer ist, dann hat man den Raum betreten
    // Deshalb wird hier das Fenster erst erstellt und nicht schon bei der Befehlsabfrage
    // es kann ja auch sein, dass man im Raum gebannt ist oder aehnliches 
    {
        wxCommandEvent eventCustom(wxEVT_NEUES_FENSTER);
        eventCustom.SetString(empfaenger);
        wxPostEvent(wxGetApp().Ereignisvw, eventCustom);
    }
    else
    // Andernfalls ist es logischerweise ein neuer Benutzer der den Raum betreten hat
    {
        wxGetApp().fenster(empfaenger)->BenutzerHinzufuegen(benutzer);
        wxGetApp().fenster(empfaenger)->NachrichtAnhaengen(_T("JOIN"),benutzer);
    }
    
}

// Benutzerliste aktualisieren / Benutzer hat den Raum verlassen
void irc_leave(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]); // = ein Raum
    wxString nachricht = _T(msg_data->params_a[1]);
    wxString benutzer = _T(msg_data->nick);
    
    if(benutzer == wxGetApp().irc->CurrentNick)
    // wenn man selber den Raum verlaesst
    {
        wxGetApp().fensterzerstoeren(empfaenger);
    }
    else
    {
        wxGetApp().fenster(empfaenger)->BenutzerEntfernen(benutzer);
        wxGetApp().fenster(empfaenger)->NachrichtAnhaengen(_T("PART"), benutzer, nachricht);
    }
    
}

void irc_quit(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(wxGetApp().irc->CurrentNick);
    wxString benutzer = _T(msg_data->nick);
    wxString nachricht = _T(msg_data->params_a[0]);
    bool benutzer_entfernt = false;

    // IN JEDEM FENSTER ENTFERNEN
    for(int i = 0; i<10; i++)
    {
        benutzer_entfernt = false; // In diesem Fenster wurde noch nicht versucht den Benutzer zu entfernen
        
        if(!(wxGetApp().zgr_fenster[i]==NULL))
        // nicht in nicht vorhandenen Fenstern
        {
            benutzer_entfernt = wxGetApp().zgr_fenster[i]->BenutzerEntfernen(benutzer);
            if(benutzer_entfernt = true)
            // Nachricht ausgeben falls der Benutzer in diesem Fenster entfernt werden konnte
            {
                wxGetApp().zgr_fenster[i]->NachrichtAnhaengen(_T("QUIT"), benutzer, nachricht);
            }
        }
    }
}

// Benutzerliste aktualisieren / Benutzer hat seinen Namen geaendert
void irc_nick(const irc_msg_data *msg_data, void *cp)
{
    //wxString empfaenger = _T(msg_data->params_a[1]);
    wxString benutzer = _T(msg_data->nick);
    wxString neuername = _T(msg_data->params_a[0]);

    // fuer jeden Raum durchmachen da der name in jedem raum geaendert werden muss
    for(int i = 0; i<10; i++)
    {
        if(!(wxGetApp().zgr_fenster[i]==NULL))
        // nicht in nicht vorhandenen Fenstern
        {
            if(benutzer == wxGetApp().irc->CurrentNick || benutzer == wxGetApp().irc->WantedNick)
            // Wenn man selber der Benutzer ist, dann muss der eigene Nick geaendert werden
            {
                wxGetApp().zgr_fenster[i]->BenutzerAendern(benutzer,neuername);
                wxGetApp().irc->CurrentNick = neuername;
                wxGetApp().zgr_fenster[i]->TitelSetzen(wxGetApp().fenstername[i],neuername);
                // nachricht anzeigen
            }
            else
            // Andernfalls ist es logischerweise ein neuer Benutzer der den Raum betreten hat
            {
                wxGetApp().zgr_fenster[i]->BenutzerAendern(benutzer,neuername);
                // nachricht anzeigen
                //wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("NICK",benutzer);
            }
        }
    }
}

// Nickname wird bereits genutzt
void irc_nickinuse(const irc_msg_data *msg_data, void *cp)
{
    // WENN DER FEHLER SCHON BEIM VERBINDEN AUFTRITT 
    // WIRD DER NEUE NAME NICHT VOM SERVER NOCHMAL GESENDET
    // MAN KANN ALSO NICHT MIT irc_nick DADRAUF REAGIEREN
    
    
    // VERBINDUNG ZUR KONFIGURATION
    // Fehlermeldung anzeigen (alternativ irc_error aufrufen hier.
    
    
    //statt diesem irc_error aufrufen
        wxString fehler = _T(msg_data->cmd);
        for(int i = 0; i < msg_data->params_i; i++)
        {
            fehler += _T(" ");
            fehler += _T(msg_data->params_a[i]);
        }
        wxGetApp().fenstersuchen(wxGetApp().irc->WantedNick)->Fehler(2,fehler);
     //statt diesem irc_error aufrufen
     
     
    wxGetApp().irc->WantedNick += _T("_"); // _ an den namen anhaengen
    wxGetApp().irc->send_nick(wxGetApp().irc->WantedNick);
}


// Thema des Raums anzeigen
void irc_topic(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[1]);
    wxGetApp().fenster(empfaenger)->ThemaAendern(_T(msg_data->params_a[2]));
}

// Thema des Raums anzeigens
void irc_requestedtopic(const irc_msg_data *msg_data, void *cp)
{
   wxString empfaenger = _T(msg_data->params_a[0]);
   wxGetApp().fenster(empfaenger)->ThemaAendern(_T(msg_data->params_a[1]));
}

// Verschiedene Fehlermeldungen anzeigen
void irc_error(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString fehler = _T(msg_data->cmd);
    for(int i = 0; i < msg_data->params_i; i++)
    {
        fehler += _T(" ");
        fehler += _T(msg_data->params_a[i]);
    }
    wxGetApp().fenstersuchen(empfaenger)->Fehler(2,fehler);
}

// Auf Ping mit Pong antworten
void irc_pong(const irc_msg_data *msg_data, void *cp)
{
    wxGetApp().irc->send_pong(msg_data->params_a[0]);
}

// RPL_UNAWAY / 305
void irc_unaway(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("RPL_UNAWAY"));
}

// RPL_NOWAWAY / 306
void irc_nowaway(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("RPL_NOWAWAY"));
}

//whois Antworten anzeigen

void irc_whoisuser(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString nick = _T(msg_data->params_a[1]);
    wxString user = _T(msg_data->params_a[2]);
    wxString host = _T(msg_data->params_a[3]);
    wxString name = _T(msg_data->params_a[5]);
    
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_BENUTZER"),nick,user,host,name);
}

void irc_whoisaway(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString nick = _T(msg_data->params_a[1]);
    wxString text = _T(msg_data->params_a[2]);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_ABWESEND"),nick,text);
}

void irc_whoischan(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString nick = _T(msg_data->params_a[1]);
    wxString chans = _T(msg_data->params_a[2]);

    // Rechte noch beachten ([@|+]#channel)???
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_RAEUME"),nick,chans);
}

void irc_whoisidle(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString nick = _T(msg_data->params_a[1]);
    wxString sekunden = _T(msg_data->params_a[2]);

    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_UNTAETIG"),nick,sekunden);
}

void irc_whoisserver(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = _T(msg_data->params_a[0]);
    wxString nick = _T(msg_data->params_a[1]);
    wxString server = _T(msg_data->params_a[2]);
    wxString servernachricht = _T(msg_data->params_a[3]);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_SERVERNACHRICHT"),nick,server,servernachricht);
}



