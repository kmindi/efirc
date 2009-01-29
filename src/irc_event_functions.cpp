// irc_event_functions.cpp
// enthaelt die Funktionen die mit der IRC-Schnittstelle verbunden werden und von dort aufgerufen werden
// FUNKTIONEN SOLLEN MITGLIEDER VON ZENTRALE SEIN
#include <zentrale.h>

DECLARE_APP(Zentrale) //braucht man fuer wxGetApp um damit auf die funktionen der Zentrale zuzugreifen

// Empfangene normale Nachrichten werden ausgegeben
void irc_pmsg(const irc_msg_data *msg_data, void *cp)
{
    wxChar leerzeichen = ' ';
    
    wxString text = msg_data->params_a[1];
    wxString empfaenger = msg_data->params_a[0];
    wxString user = msg_data->nick;
    wxString prefix = _T("\001");
    
    // CTCP abfragen ("'\001'BEFEHL text'\001'")
    //Falls kein CTCP:
    
    if (!(text.StartsWith(prefix.c_str()) && text.EndsWith(prefix.c_str())))
    {
            if(empfaenger == wxGetApp().irc->CurrentNick) 
            // wenn man selber der empfaenger ist
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_PRIVMSG",user,text);
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("PRIVMSG",user,text);
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
        
        if(ctcp_befehl == "ACTION")
        {
            if(empfaenger == wxGetApp().irc->CurrentNick)
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_ACTION",user,ctcp_text);
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("ACTION",user,ctcp_text);
            }
        }
        
        if(ctcp_befehl == "VERSION")
        {
        
            if(ctcp_text == ctcp || ctcp_text == "")
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("CTCP",user,ctcp_befehl);
                wxString antwort = _T("\001VERSION efirc:0.3.testing:Windoofs\001");
                wxGetApp().irc->send_notice(user.c_str(), antwort.c_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_PRIVMSG",user,ctcp_befehl + " " + ctcp_text);
            }
        }
        
        if(ctcp_befehl == "FINGER")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("CTCP",user,ctcp_befehl);
                // FINGER ANTWORT?
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_PRIVMSG",user,ctcp_befehl + " " + ctcp_text);
            }
        }

        if(ctcp_befehl == "SOURCE")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("CTCP",user,ctcp_befehl);
                wxString antwort = _T("\001SOURCE http://efirc.sourceforge.net/\001");
                wxGetApp().irc->send_notice(user.c_str(), antwort.c_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_PRIVMSG",user,ctcp_befehl + " " + ctcp_text);
            }
        }

        if(ctcp_befehl == "USERINFO")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("CTCP",user,ctcp_befehl);
                // ANTWORT?
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_PRIVMSG",user,ctcp_befehl + " " + ctcp_text);
            }
        }

        if(ctcp_befehl == "CLIENTINFO")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("CTCP",user,ctcp_befehl);
                wxString antwort = _T("\001CLIENTINFO VERSION FINGER SOURCE USERINFO CLIENTINFO PING TIME\001");
                wxGetApp().irc->send_notice(user.c_str(), antwort.c_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_PRIVMSG",user,ctcp_befehl + " " + ctcp_text);
            }
        }

        // PING FORMAT IMMER MIT ZEITSTEMPEL
        // UNTERSCHIED IST NUR PRIVMSG ZUM ANFORDERN UND NOTICE ALS ANTWORT
        if(ctcp_befehl == "PING")
        {
            wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_PRIVMSG",user,ctcp_befehl + " " + ctcp_text);
        }


        if(ctcp_befehl == "TIME")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("CTCP",user,ctcp_befehl);
                
                char timestamp[30];
                time_t raw_time;
                tm *local_time;

                time(&raw_time);
                local_time = localtime(&raw_time);

                strftime(timestamp, 30, "%d.%m.%Y %H:%M:%S", local_time);

                wxString timewxString(timestamp);
                wxString antwort = "\001TIME "+ timewxString + "\001";
                
                wxGetApp().irc->send_notice(user.c_str(), antwort.c_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("P_PRIVMSG",user,ctcp_befehl + " " + ctcp_text);
            }
        }
    }

}

void irc_mode(const irc_msg_data *msg_data, void *cp)
{
    // NOCH HERAUSSUCHEN WENN +o und +v (=> @ und + vor nickname bzw chanop und voice)
    
    
    wxString Sender = msg_data->nick;
    wxString empfaenger = msg_data->params_a[0];
    wxString Parameter = "";
    wxString Raum = "";
    wxString Modus = "";

    if(Sender == "")
    {
        Sender = msg_data->sender;
    }

    for(int i = 0; i < msg_data->params_i; i++)
    {
        if(msg_data->params_a[i][0] == '#')
        {
            Raum = _T(" (");
            Raum += _T(msg_data->params_a[i]); 
            Raum += _T(")");
        }
        else if(msg_data->params_a[i][0] == '+' ||
                msg_data->params_a[i][0] == '-')
        {
            Modus = msg_data->params_a[i];
        }
        else
        {
            Parameter += " " + string(msg_data->params_a[i]);
        }
    }

    Modus += Parameter + Raum;

    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("MODE", Sender, Modus);
}

// Message of the day anzeigen
void irc_motd(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("MOTD",wxT(msg_data->params_a[1]));
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
    wxString empfaenger = msg_data->params_a[2];
    wxString benutzerliste;
    benutzerliste = msg_data->params_a[3];
    wxGetApp().fenstersuchen(empfaenger)->BenutzerHinzufuegen(benutzerliste);
}

// Benutzerliste aktualisieren / Benutzer hat den Raum betreten
void irc_join(const irc_msg_data *msg_data, void *cp)
{
    
    wxString empfaenger = msg_data->params_a[0];
    wxString benutzer = msg_data->nick;

    if(benutzer == wxGetApp().irc->CurrentNick)
    // Wenn man selber der Benutzer ist, dann hat man den Raum betreten
    // Deshalb wird hier das Fenster erst erstellt und nicht schon bei der Befehlsabfrage
    {
        wxGetApp().neuesFenster(_T(msg_data->params_a[0]));
    }
    else
    // Andernfalls ist es logischerweise ein neuer Benutzer der den Raum betreten hat
    {
        wxGetApp().fenstersuchen(empfaenger)->BenutzerHinzufuegen(benutzer);
        wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("JOIN",benutzer);
    }
    
}

// Benutzerliste aktualisieren / Benutzer hat den Raum verlassen
void irc_leave(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
    wxString nachricht = msg_data->params_a[1];
    wxString benutzer = msg_data->nick;
    wxGetApp().fenstersuchen(empfaenger)->BenutzerEntfernen(benutzer);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("PART",benutzer,nachricht);
}

// Benutzerliste aktualisieren / Benutzer hat seinen Namen geaendert
void irc_nick(const irc_msg_data *msg_data, void *cp)
{
    //wxString empfaenger = msg_data->params_a[1];
    wxString benutzer = msg_data->nick;
    wxString neuername = msg_data->params_a[0];

    // fuer jeden Raum durchmachen da der name in jedem raum geaendert werden muss
    for(int i = 0; i<10; i++)
    {
        if(!(wxGetApp().zgr_fenster[i]==NULL))
        // nicht in nicht vorhandenen Fenstern
        {
            if(benutzer == wxGetApp().irc->CurrentNick)
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


// Thema des Raums anzeigen
void irc_topic(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[1];
    wxGetApp().fenstersuchen(empfaenger)->ThemaAendern(_T(msg_data->params_a[2]));
}

// Thema des Raums anzeigen (angefordert)
void irc_requestedtopic(const irc_msg_data *msg_data, void *cp)
{
   wxString empfaenger = msg_data->params_a[0];
   wxGetApp().fenstersuchen(empfaenger)->ThemaAendern(_T(msg_data->params_a[1]));
}

// Verschiedene Fehlermeldungen anzeigen
void irc_error(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
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
    wxString empfaenger = msg_data->params_a[0];
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("RPL_UNAWAY");
}

// RPL_NOWAWAY / 306
void irc_nowaway(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("RPL_NOWAWAY");
}

//whois Antworten anzeigen

void irc_whoisuser(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
    wxString nick = _T(msg_data->params_a[1]);
    wxString user = _T(msg_data->params_a[2]);
    wxString host = _T(msg_data->params_a[3]);
    wxString name = _T(msg_data->params_a[5]);
    
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("WHOIS_BENUTZER",nick,user,host,name);
}

void irc_whoisaway(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
    wxString nick = _T(msg_data->params_a[1]);
    wxString text = _T(msg_data->params_a[2]);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("WHOIS_ABWESEND",nick,text);
}

void irc_whoischan(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
    wxString nick = _T(msg_data->params_a[1]);
    wxString chans = _T(msg_data->params_a[2]);

    // Rechte noch beachten ([@|+]#channel)???
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("WHOIS_RAEUME",nick,chans);
}

void irc_whoisidle(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
    wxString nick = _T(msg_data->params_a[1]);
    wxString sekunden = _T(msg_data->params_a[2]);

    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("WHOIS_UNTAETIG",nick,sekunden);
}

void irc_whoisserver(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger = msg_data->params_a[0];
    wxString nick = _T(msg_data->params_a[1]);
    wxString server = _T(msg_data->params_a[2]);
    wxString servernachricht = _T(msg_data->params_a[3]);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen("WHOIS_SERVERNACHRICHT",nick,server,servernachricht);
}



