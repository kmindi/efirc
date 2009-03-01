// irc_event_functions.cpp
// enthaelt die Funktionen die mit der IRC-Schnittstelle verbunden werden und von dort aufgerufen werden
// FUNKTIONEN SOLLEN MITGLIEDER VON ZENTRALE SEIN
#include <zentrale.h>

DECLARE_APP(Zentrale) //braucht man fuer wxGetApp um damit auf die funktionen der Zentrale zuzugreifen


// Benutzerdefiniertes Ereignis muss hier auch bekannt sein, definiert wird es in zentrale.cpp
DECLARE_EVENT_TYPE(wxEVT_NEUES_FENSTER, 7777)


// Empfangene normale Nachrichten werden ausgegeben
void irc_pmsg(const irc_msg_data *msg_data, void *cp)
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
            if(empfaenger.Upper() == wxGetApp().irc->CurrentNick.Upper())
            // wenn man selber der empfaenger ist
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,text);
            }
            else
            {
                if(user == _T(""))
                {
                    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),user,text);
                }
                else
                {
                    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG"),user,text);
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
            if(empfaenger.Upper() == wxGetApp().irc->CurrentNick.Upper())
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

            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString version = _T("efirc:") + wxGetApp().efirc_version_string + _T("(") + wxString(wxVERSION_STRING,wxConvUTF8) + _T("):Windoofs");
                wxGetApp().irc->send_ctcp_version(user.mb_str(), version.mb_str());
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
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }

        if(ctcp_befehl == _T("SOURCE"))
        {
            if(ctcp_text == ctcp || ctcp_text == _T(""))
            {
                // Fenster auswählen
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString quelle = _T("http://efirc.sourceforge.net/");
                wxGetApp().irc->send_ctcp_source(user.mb_str(), quelle.mb_str());
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
                wxString antwort = _T("VERSION FINGER SOURCE USERINFO CLIENTINFO PING TIME");
                wxGetApp().irc->send_ctcp_clientinfo(user.mb_str(), antwort.mb_str());
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
            wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
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

                wxString timewxString(timestamp, wxConvUTF8);
                wxString antwort = timewxString;

                wxGetApp().irc->send_ctcp_time(user.mb_str(), antwort.mb_str());
            }
            else
            {
                wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }
    }

}


// Willkommensnachrichten 001 002 003 und 004
void irc_welcome(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nachricht = _T("");
    for(int i = 1; i < msg_data->params_i; i++)
    {
        nachricht += _T(" ");
        nachricht += wxString(msg_data->params_a[i], wxConvUTF8);
    }
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),_T(""), nachricht);

    // bei 001 ist der aktuelle Nickname gleich dem Empfaenger der Nachricht (man selber)
    if(wxString(msg_data->cmd, wxConvUTF8) == _T("001"))
    {
        // fuer jeden Raum durchmachen da der name in jedem raum geaendert werden muss
        for(int i = 0; i < max_fenster; i++)
        {
            if(!(wxGetApp().zgr_fenster[i]==NULL))
            // nicht in nicht vorhandenen Fenstern
            {
                    wxGetApp().irc->CurrentNick = empfaenger;
                    wxGetApp().zgr_fenster[i]->TitelSetzen(wxGetApp().fenstername[i], empfaenger);
            }
        }
    }
}

// RPL_ISUPPORT
void irc_isupport(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nachricht = _T("");
    for(int i = 1; i < msg_data->params_i; i++)
    {
        nachricht += _T(" ");
        nachricht += wxString(msg_data->params_a[i], wxConvUTF8);
    }
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),_T(""), nachricht);
}

void irc_mode(const irc_msg_data *msg_data, void *cp)
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

    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("MODE"), Sender, Modus);
}

// Message of the day anzeigen
void irc_motd(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("MOTD"),wxString(msg_data->params_a[1], wxConvUTF8));
}

// Am Ende der Nachricht des Tages automatisch einen Raum betreten
void irc_endofmotd(const irc_msg_data *msg_data, void *cp)
{
    // den in der Konfigurationsdatei genannten Kanal betreten
    wxGetApp().irc->send_join(wxGetApp().config->parsecfgvalue(_T("irc_channel")).mb_str());
}

// Benutzerliste einlesen
void irc_userlist(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[2], wxConvUTF8);
    wxString benutzerliste;
    benutzerliste = wxString(msg_data->params_a[3], wxConvUTF8);
    wxGetApp().fenster(empfaenger)->BenutzerHinzufuegen(benutzerliste);
}

// Benutzerliste aktualisieren / Benutzer hat den Raum betreten
void irc_join(const irc_msg_data *msg_data, void *cp)
{

    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString benutzer(msg_data->nick, wxConvUTF8);

    if(benutzer.Upper() == wxGetApp().irc->CurrentNick.Upper())
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
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8); // = ein Raum
    wxString nachricht(msg_data->params_a[1], wxConvUTF8);
    wxString benutzer(msg_data->nick, wxConvUTF8);

    if(benutzer.Upper() == wxGetApp().irc->CurrentNick.Upper())
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
    wxString empfaenger(wxGetApp().irc->CurrentNick, wxConvUTF8);
    wxString benutzer(msg_data->nick, wxConvUTF8);
    wxString nachricht(msg_data->params_a[0], wxConvUTF8);
    bool benutzer_entfernt = false;

    // IN JEDEM FENSTER ENTFERNEN
    for(int i = 0; i < max_fenster; i++)
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
    //wxString empfaenger(msg_data->params_a[1], wxConvUTF8);
    wxString benutzer(msg_data->nick, wxConvUTF8);
    wxString neuername(msg_data->params_a[0], wxConvUTF8);

    // fuer jeden Raum durchmachen da der name in jedem raum geaendert werden muss
    for(int i = 0; i < max_fenster; i++)
    {
        if(!(wxGetApp().zgr_fenster[i]==NULL))
        // nicht in nicht vorhandenen Fenstern
        {
            if(benutzer.Upper() == wxGetApp().irc->CurrentNick.Upper() || benutzer.Upper() == wxGetApp().irc->WantedNick.Upper())
            // Wenn man selber der Benutzer ist, dann muss der eigene Nick geaendert werden
            {
                wxGetApp().zgr_fenster[i]->BenutzerAendern(benutzer,neuername);
                wxGetApp().irc->CurrentNick = neuername;
                wxGetApp().zgr_fenster[i]->TitelSetzen(wxGetApp().fenstername[i],neuername);
                wxGetApp().zgr_fenster[i]->NachrichtAnhaengen(_T("NICK"), benutzer, neuername);
            }
            else
            // Andernfalls ist es logischerweise ein neuer Benutzer der den Raum betreten hat
            {
                wxGetApp().zgr_fenster[i]->BenutzerAendern(benutzer, neuername);
                // nachricht anzeigen
                wxGetApp().zgr_fenster[i]->NachrichtAnhaengen(_T("NICK"), benutzer, neuername);
            }
        }
    }
}

// Nickname wird bereits genutzt
void irc_nickinuse(const irc_msg_data *msg_data, void *cp)
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
        wxGetApp().fenstersuchen(wxGetApp().irc->WantedNick)->Fehler(2,fehler);
     //statt diesem irc_error aufrufen

    wxGetApp().irc->WantedNick += _T("_"); // _ an den namen anhaengen
    wxGetApp().irc->send_nick(wxGetApp().irc->WantedNick.mb_str());
}

// Einladung in einen Raum
void irc_invite(const irc_msg_data *msg_data, void *cp)
{
    wxString benutzer(msg_data->nick, wxConvUTF8);
    wxString raum(msg_data->params_a[1], wxConvUTF8);

    wxGetApp().fenstersuchen(wxGetApp().irc->CurrentNick)->NachrichtAnhaengen(_T("INVITE"),benutzer,raum);
}

// Thema des Raums anzeigen
void irc_topic(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[1], wxConvUTF8);
    wxGetApp().fenster(empfaenger)->ThemaAendern(wxString(msg_data->params_a[2], wxConvUTF8));
}

// Thema des Raums anzeigens
void irc_requestedtopic(const irc_msg_data *msg_data, void *cp)
{
   wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
   wxString benutzer(msg_data->nick, wxConvUTF8);
   wxGetApp().fenster(empfaenger)->ThemaAendern(wxString(msg_data->params_a[1], wxConvUTF8), benutzer);
}

// Verschiedene Fehlermeldungen anzeigen
void irc_error(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString fehler(msg_data->cmd, wxConvUTF8);
    for(int i = 0; i < msg_data->params_i; i++)
    {
        fehler += _T(" ");
        fehler += wxString(msg_data->params_a[i], wxConvUTF8);
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
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("RPL_UNAWAY"));
}

// RPL_NOWAWAY / 306
void irc_nowaway(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("RPL_NOWAWAY"));
}

//whois Antworten anzeigen

void irc_whoisuser(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString user(msg_data->params_a[2], wxConvUTF8);
    wxString host(msg_data->params_a[3], wxConvUTF8);
    wxString name(msg_data->params_a[5], wxConvUTF8);

    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_USER"),nick,user,host,name);
}

void irc_whoisaway(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString text(msg_data->params_a[2], wxConvUTF8);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_AWAY"),nick,text);
}

void irc_whoischan(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString chans(msg_data->params_a[2], wxConvUTF8);

    // Rechte noch beachten ([@|+]#channel)???
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_CHANNEL"),nick,chans);
}

void irc_whoisidle(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString sekunden(msg_data->params_a[2], wxConvUTF8);

    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_IDLE"),nick,sekunden);
}

void irc_whoisserver(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString nick(msg_data->params_a[1], wxConvUTF8);
    wxString server(msg_data->params_a[2], wxConvUTF8);
    wxString servernachricht(msg_data->params_a[3], wxConvUTF8);
    wxGetApp().fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("WHOIS_SERVERMSG"),nick,server,servernachricht);
}

// allgemeine Funktion fuer Befehle die noch nicht eingebaut wurden, aber trotzdem angezeigt werden koennen
void irc_notimplemented(const irc_msg_data *msg_data, void *cp)
{
    wxString empfaenger(msg_data->params_a[0], wxConvUTF8);
    wxString fehler(msg_data->cmd, wxConvUTF8);
    for(int i = 0; i < msg_data->params_i; i++)
    {
        fehler += _T(" ");
        fehler += wxString(msg_data->params_a[i], wxConvUTF8);
    }
    wxGetApp().fenstersuchen(empfaenger)->Fehler(4,fehler);
}
