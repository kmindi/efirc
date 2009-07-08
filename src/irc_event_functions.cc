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
void irc_allgemein(const irc_msg_data *msg_data, void *)
{
    if(wxGetApp().irc->connected || (wxGetApp().irc->_DBGRECON && !(wxGetApp().irc->connected)))
    {
        wxCommandEvent eventCustom(wxEVT_NEUER_IRC_BEFEHL); // Neues Ereignis erzeugen
        IRC_NACHRICHT *zgr_ircn = new IRC_NACHRICHT(msg_data); // Instanz der IRC_NACHRICHT-Klasse erstellen und merken
        eventCustom.SetClientData(zgr_ircn); // Dem Ereignis die Position der Daten mitteilen
        wxPostEvent(wxGetApp().Ereignisvw, eventCustom); // Ereignis ausloesen, bzw in die Abarbeitungswarteschlange einreihen
    }
}


void Ereignisverwalter::BeiNeueIRCNachricht(wxCommandEvent& event)
{
    // Daten aus dem Ereignis einem eigenen Zeiger zuweisen.
    const IRC_NACHRICHT *msg_data = (IRC_NACHRICHT *)event.GetClientData();

    // Befehl in Variable speichern
    wxString cmd = msg_data->cmd;
    long unsigned int cmd_int = 0;

    // Befehle Abfragen und entsprechende Funktionen aufrufen
    if(cmd.ToULong(&cmd_int, 10)) // Befehlstext zu einer Zahl konvertieren
    // Wenn der Befehl zu einer Zahl konvertiert werden konnte
    {
        switch(cmd_int)
        {
            // Willkommensnachricht 001
            case 1:
                wxGetApp().irc_welcome(msg_data); // Nickname setzen
                wxGetApp().irc_einfach(msg_data); // Nachricht aber auch noch ausgeben
                break;

            case 5:
                wxGetApp().irc_isupport(msg_data);
                break;

            // Thema wurde gesetzt wann.
            case 328:
                wxGetApp().irc_chanurl(msg_data);
                break;

            // Thema des Raums anzeigen
            case 332:
                wxGetApp().irc_topic(msg_data);
                break;

            // Thema wurde gesetzt wann.
            case 333:
                wxGetApp().irc_topicwhotime(msg_data);
                break;

            // Benutzerliste
            case 353:
                wxGetApp().irc_userlist(msg_data);
                break;

            // RPL_UNAWAY
            case 305:
                wxGetApp().irc_unaway(msg_data);
                break;

            // RPL_NOWAWAY
            case 306:
                wxGetApp().irc_nowaway(msg_data);
                break;

            // END OF MOTD und ersten Raum betreten
            case 376:
                wxGetApp().irc_endofmotd(msg_data);
                break;

            // WHOIS - USER
            case 311:
                wxGetApp().irc_whoisuser(msg_data);
                break;

            // WHOIS - ABWESEND
            case 301:
                wxGetApp().irc_whoisaway(msg_data);
                break;

            // WHOIS - RAEUME
            case 319:
                wxGetApp().irc_whoischan(msg_data);
                break;

            // WHOIS - UNTAETIG
            case 317:
                wxGetApp().irc_whoisidle(msg_data);
                break;

            // WHOIS - SERVERINFO
            case 312:
                wxGetApp().irc_whoisserver(msg_data);
                break;

            // WHOIS - Special
            case 320:
                wxGetApp().irc_whoisspecial(msg_data);
                break;

            // WHOIS - BENUTZT HOST
            case 338:
                wxGetApp().irc_whoisactually(msg_data);
                break;

            // Angeforderter Benutzername wird bereits benutzt
            case 433:
                wxGetApp().irc_nickinuse(msg_data);
                break;


            // Nachrichten die nicht angezeigt werden sollen
            case 318: // End of /WHOIS List
            case 366: // End of /NAMES List
                break;

            // Nachrichten die einfach angezeigt werden sollen
            case 2: // 002 RPL_YOURHOST
            case 3: // 003 RPL_CREATED
            case 4: // 004 RPL_MYINFO
            case 42: // 042 Unique ID
            case 250: // 250 RPL_STATSCONN
            case 251: // 251 RPL_LUSERCLIENT
            case 252: // 252 RPL_LUSEROP
            case 253: // 253 RPL_LUSERUNKOWN
            case 254: // 254 RPL_LUSERCHANNELS
            case 255: // 255 RPL_LUSERME
            case 265: // 265 RPL_LOCALUSERS
            case 266: // 266 RPL_GLOBALUSERS
            case 372: // 372 RPL_MOTD
            case 375: // 375 RPL_MOTDSTART
                wxGetApp().irc_einfach(msg_data);
                break;


            default:
                if(cmd_int >= 400 || cmd_int <=499)
                // Abfrage nach ERR_* Antworten
                {
                    wxGetApp().irc_fehler(msg_data);
                }
                else
                // wenn die Nummer nicht gefunden wurde
                {
                    wxGetApp().irc_unbekannt(msg_data);
                }

        }
    }
    else
    // wenn nicht
    {
        // PRIVMSG und NOTICE
        if(cmd == _T("PRIVMSG") || cmd == _T("NOTICE"))
        {
            wxGetApp().irc_pmsg(msg_data);
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

        else if(cmd == _T("INVITE"))
        {
            wxGetApp().irc_invite(msg_data);
        }

        else if(cmd == _T("TOPIC"))
        {
            wxGetApp().irc_requestedtopic(msg_data);
        }

        else if(cmd == _T("MODE"))
        {
            wxGetApp().irc_mode(msg_data);
        }

        // PING PONG
        else if(cmd == _T("PING"))
        {
            wxGetApp().irc_pong(msg_data);
        }
        
        else if(cmd == _T("KICK"))
        {
            wxGetApp().irc_kick(msg_data);
        }

        // wenn keine Uebereinstimmung gefunden wurde, Fehler anzeigen
        else
        {
            wxGetApp().irc_unbekannt(msg_data);
        }
    }

        // Angeforderten Speicher wieder freigeben
        delete msg_data;
}



// Empfangene normale Nachrichten werden ausgegeben
void Zentrale::irc_pmsg(const IRC_NACHRICHT *msg_data)
{
    wxChar leerzeichen = _T(' ');

    wxString text = msg_data->params_a[1];
    wxString empfaenger = msg_data->params_a[0];
    wxString user = msg_data->nick;

    // CTCP abfragen ("'\001'BEFEHL text'\001'")

    if (!(text.StartsWith(_T("\001")) && text.EndsWith(_T("\001"))))
    // Wenn nicht, dann ist es eine normale Nachricht
    {
            if(empfaenger.Upper() == irc->CurrentNick.Upper())
            // Wenn man selber der empfaenger ist ...
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,text);
            }
            else
            // ... oder wenn die Nachricht an den Raum geschickt wurde
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
        wxString ctcp(text.Mid(1,text.Len()-2)); // \001 vorne und hinten abschneiden
        wxString ctcp_befehl(ctcp.BeforeFirst(leerzeichen));
        wxString ctcp_text(ctcp.AfterFirst(leerzeichen));

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
                // Fenster auswählen
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString version = _T("efirc:") + efirc_version_string + _T(" (") + wxString(wxVERSION_STRING,wxConvUTF8) + _T("):") + wxGetOsDescription();
                irc->irc_send_ctcp_version(user.mb_str(), version.mb_str());
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("S_CTCP"), user, ctcp_befehl + _T(" ") + version);
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
                // Fenster auswählen
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString quelle = _T("http://efirc.sourceforge.net/");
                irc->irc_send_ctcp_source(user.mb_str(), quelle.mb_str());
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("S_CTCP"), user, ctcp_befehl + _T(" ") + quelle);
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
                // Fenster auswählen
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);
                wxString antwort = _T("VERSION FINGER SOURCE USERINFO CLIENTINFO PING TIME");
                irc->irc_send_ctcp_clientinfo(user.mb_str(), antwort.mb_str());
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("S_CTCP"), user, ctcp_befehl + _T(" ") + antwort);
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
                // Fenster auswählen
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("CTCP"),user,ctcp_befehl);

                char timestamp[30];
                time_t raw_time;
                tm *local_time;

                time(&raw_time);
                local_time = localtime(&raw_time);

                strftime(timestamp, 30, "%d.%m.%Y %H:%M:%S", local_time);

                wxString timewxString(timestamp, wxConvUTF8);
                wxString antwort = timewxString;

                irc->irc_send_ctcp_time(user.mb_str(), antwort.mb_str());
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("S_CTCP"), user, ctcp_befehl + _T(" ") + antwort);
            }
            else
            {
                fenstersuchen(empfaenger)->NachrichtAnhaengen(_T("P_PRIVMSG"),user,ctcp_befehl + _T(" ") + ctcp_text);
            }
        }
    }

}

// Willkommensnachrichten 001 , Nickname setzen
void Zentrale::irc_welcome(const IRC_NACHRICHT *msg_data)
{
    // bei 001 ist der aktuelle Nickname gleich dem Empfaenger der Nachricht (man selber)
    wxString alter_nick = irc->CurrentNick;
    wxString empfaenger = msg_data->params_a[0];
    if(irc->CurrentNick != empfaenger)
    // wenn der gesendete Nick nicht der gleiche wie der gespeicherte ist, dann überall anpassen, denn dann wurde er vom server geaendert
    {
        irc->CurrentNick = empfaenger; // neuen nickname setzen

        for(map< wxString, Fenster* >::iterator i = zgr_fenster.begin(); i != zgr_fenster.end(); i++)
        // In jedem Fenster neuen Namen setzen und Fenster anpassen
        {
            if(!(zgr_fenster[i->first]==NULL))
            // nicht in nicht vorhandenen Fenstern
            {
                if(i == zgr_fenster.find(alter_nick.Upper()))
                // falls gerade das Fenster geändert wird, das als Namen den eigenen Nickname hat
                // Nickname aenderungen anpassen so das Nachrichten wieder korrekt in das Fenster kommen
                {
                    zgr_fenster[i->first]->fenster_name = empfaenger;
                    zgr_fenster[i->first]->TitelSetzen(empfaenger);
                    // Map-Eintrag loeschen und neu erstellen, weil der Schlüssel nicht geaendert werden kann.
                    TitelSetzen(_T(""), empfaenger);
                    Fenster* zgr = zgr_fenster[i->first]; // Adresse speichern
                    zgr_fenster.erase(i); // Eintrag loeschen
                    zgr_fenster.insert(make_pair(empfaenger.Upper(), zgr)); // neuen Eintrag mit neuem Schluessel erstellen
                }
                else
                {
                    TitelSetzen(_T(""), empfaenger);
                }
            }
        }
    }

}

// RPL_ISUPPORT
void Zentrale::irc_isupport(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger = msg_data->params_a[0];
    wxString nachricht = _T("");
    for(int i = 1; i < msg_data->params_i; i++)
    {
        nachricht += _T(" ");
        nachricht += msg_data->params_a[i];
    }
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),_T(""), nachricht);
}

void Zentrale::irc_mode(const IRC_NACHRICHT *msg_data)
{
    // NOCH HERAUSSUCHEN WENN +o und +v (=> @ und + vor nickname bzw chanop und voice)

    
    // :keinname!keinname@192.168.0.100 MODE #efirc -k testpassword
    
    
    
    wxString Sender = msg_data->nick;
    wxString empfaenger = msg_data->params_a[0];
    wxString Parameter = _T("");
    wxString Modi = _T("");
    bool Raum = false;
    
    if(Sender == _T(""))
    {
        Sender = msg_data->sender;
    }
    
    if(empfaenger.StartsWith(_T("#")) || empfaenger.StartsWith(_T("&")))
    {
        Raum = true;
    }

    if(msg_data->params_i >= 2)
    {
        Modi = msg_data->params_a[1];

        for(int i = 2; i < msg_data->params_i; i++)
        {
            Parameter += _T(" ") + msg_data->params_a[i];
        }
        
        // Modi nur zerlegen wenn ein dritter Parameter uebergeben wurde
        if(msg_data->params_i >= 3)
        {
            bool positive_mode = false;
            wxString modi_setzen = _T("");
            wxString modi_entfernen = _T("");
            
            // -t+o 
            for(int i = 0; i < Modi.Len(); i++)
            // den ganzen modus text durchsuchen
            {
                if(Modi.GetChar(i) == _T('+'))
                    positive_mode = true;
                else if(Modi.GetChar(i) == _T('-'))
                    positive_mode = false;
                else if(positive_mode)
                    modi_setzen += Modi.GetChar(i);
                else
                    modi_entfernen += Modi.GetChar(i);
            }
            
            if(modi_setzen.Matches(_T("*o*")) )
                fenster(empfaenger)->BenutzerModiHinzufuegen(msg_data->params_a[2], _T("@"));
                
            else if(modi_entfernen.Matches(_T("*o*")))
                fenster(empfaenger)->BenutzerModiEntfernen(msg_data->params_a[2], _T("@"));
                
            if(modi_setzen.Matches(_T("*v*")) )
                fenster(empfaenger)->BenutzerModiHinzufuegen(msg_data->params_a[2], _T("+"));
                
            else if(modi_entfernen.Matches(_T("*v*")))
                fenster(empfaenger)->BenutzerModiEntfernen(msg_data->params_a[2], _T("+"));
        }
            
    }
    Modi += Parameter;

    fenster(empfaenger)->NachrichtAnhaengen(_T("MODE"), Sender, Modi);
}

// Message of the day anzeigen
void Zentrale::irc_motd(const IRC_NACHRICHT *msg_data)
{
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("MOTD"), msg_data->params_a[1]);
}

// Am Ende der Nachricht des Tages automatisch einen Raum betreten
void Zentrale::irc_endofmotd(const IRC_NACHRICHT *msg_data)
{
    // den in der Konfigurationsdatei genannten Kanal betreten
    irc->irc_send_join(raeume.mb_str());
}

// Benutzerliste einlesen
void Zentrale::irc_userlist(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger = msg_data->params_a[2];
    wxString benutzerliste;
    benutzerliste = msg_data->params_a[3];
    fenster(empfaenger)->BenutzerHinzufuegen(benutzerliste);
}

// Benutzerliste aktualisieren / Benutzer hat den Raum betreten
void Zentrale::irc_join(const IRC_NACHRICHT *msg_data)
{

    wxString empfaenger = msg_data->params_a[0];
    wxString benutzer = msg_data->nick;

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
        fenster(empfaenger)->NachrichtAnhaengen(_T("BL_JOIN"),benutzer);
    }

}

// Benutzerliste aktualisieren / Benutzer hat den Raum verlassen
void Zentrale::irc_leave(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger = msg_data->params_a[0]; // = ein Raum
    wxString nachricht = _T("");
    if (msg_data->params_i > 1)
    {
        nachricht = msg_data->params_a[1];
    }

    wxString benutzer = msg_data->nick;

    if(benutzer.Upper() == irc->CurrentNick.Upper())
    // wenn man selber den Raum verlaesst
    {
        //fenster(empfaenger)->NachrichtAnhaengen(_T("BL_PART"), benutzer, nachricht);
        // immer anzeigen?
    }
    else
    {
        fenster(empfaenger)->BenutzerEntfernen(benutzer);
        fenster(empfaenger)->NachrichtAnhaengen(_T("BL_PART"), benutzer, nachricht);
    }

}

void Zentrale::irc_quit(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger(irc->CurrentNick);
    wxString benutzer = msg_data->nick;
    wxString nachricht = msg_data->params_a[0];
    bool benutzer_entfernt = false;

    for(map< wxString, Fenster* >::iterator i = zgr_fenster.begin(); i != zgr_fenster.end(); i++)
    {
        benutzer_entfernt = false; // In diesem Fenster wurde noch nicht versucht den Benutzer zu entfernen

        if(!(zgr_fenster[i->first]==NULL))
        // nicht in nicht vorhandenen Fenstern
        {
            benutzer_entfernt = zgr_fenster[i->first]->BenutzerEntfernen(benutzer);
            if(benutzer_entfernt == true)
            // Nachricht ausgeben falls der Benutzer in diesem Fenster entfernt werden konnte
            {
                zgr_fenster[i->first]->NachrichtAnhaengen(_T("BL_QUIT"), benutzer, nachricht);
            }
        }
    }
}

// Benutzerliste aktualisieren / Benutzer hat seinen Namen geaendert
void Zentrale::irc_nick(const IRC_NACHRICHT *msg_data)
{
    wxString alter_nick = irc->CurrentNick;
    wxString benutzer = msg_data->nick;
    wxString neuername = msg_data->params_a[0];

    for(map< wxString, Fenster* >::iterator i = zgr_fenster.begin(); i != zgr_fenster.end(); i++)
    {
        if(!(zgr_fenster[i->first]==NULL))
        // nicht in nicht vorhandenen Fenstern
        {
            if(benutzer.Upper() == alter_nick.Upper() || benutzer.Upper() == irc->WantedNick.Upper())
            // Wenn man selber der Benutzer ist, dann muss der eigene Nick geaendert werden
            {
                if(i == zgr_fenster.find(alter_nick.Upper()))
                // falls gerade das Fenster geändert wird, das als Namen den eigenen Nickname hat
                // Nickname aenderungen anpassen so das Nachrichten wieder korrekt in das Fenster kommen
                {
                    zgr_fenster[i->first]->fenster_name = neuername;
                    zgr_fenster[i->first]->TitelSetzen(neuername);
                    TitelSetzen(_T(""), neuername);
                    // Map-Eintrag loeschen und neu erstellen, weil der Schlüssel nicht geaendert werden kann.
                    Fenster* zgr = zgr_fenster[i->first]; // Adresse speichern
                    zgr_fenster.erase(i); // Eintrag loeschen
                    zgr_fenster.insert(make_pair(neuername.Upper(), zgr)); // neuen Eintrag mit neuem Schluessel erstellen
                }
                else
                {
                    zgr_fenster[i->first]->BenutzerAendern(benutzer,neuername);
                    TitelSetzen(_T(""), neuername);
                    zgr_fenster[i->first]->NachrichtAnhaengen(_T("BL_NICK"), benutzer, neuername);
                }

                irc->CurrentNick = neuername;
            }
            else
            // Andernfalls ist es logischerweise ein Benutzer der seinen Namen geaendert hat
            {
                // Namen aendern und bei Erfolg Nachricht anzeigen
                if(zgr_fenster[i->first]->BenutzerAendern(benutzer, neuername))
                zgr_fenster[i->first]->NachrichtAnhaengen(_T("BL_NICK"), benutzer, neuername);
            }
        }
    }
}

// Nickname wird bereits genutzt
void Zentrale::irc_nickinuse(const IRC_NACHRICHT *msg_data)
{
    irc_fehler(msg_data); // Fehlermeldung anzeigen

    if(config->parsecfgvalue(_T("bool_automatic_nickchange_if_in_use")) == _T("1"))
    // Abfragen ob der Nickname automatisch geaendert weren darf wenn er schon vorhanden ist
    {
        irc->WantedNick += _T("_"); // _ an den namen anhaengen
        irc->irc_send_nick(irc->WantedNick.mb_str());
    }
    else
    {
        if(nickdialog != NULL) nickdialog->Destroy(); // Falls der Dialog schon angezeigt wird diesen zerstoeren
        nickdialog = new wxTextEntryDialog(fenster(irc->CurrentNick), config->parsecfgvalue(_T("local_DLG_NEWNICK_NICKINUSE_TEXT")), config->parsecfgvalue(_T("local_DLG_NEWNICK_CAPTION")), wxEmptyString, wxOK);
        if(nickdialog->ShowModal() == wxID_OK && nickdialog->GetValue() != _T(""))
        {
            irc->WantedNick = nickdialog->GetValue();
            irc->irc_send_nick(irc->WantedNick.mb_str());
        }
        else
        // wenn nichts eingegeben wurde oder der Dialog beendet wurde trotzdem einen Nick senden
        {
            irc->irc_send_nick(irc->WantedNick.mb_str());
        }
    }
}

// Einladung in einen Raum
void Zentrale::irc_invite(const IRC_NACHRICHT *msg_data)
{
    wxString benutzer = msg_data->nick;
    wxString raum = msg_data->params_a[1];

    fenstersuchen(irc->CurrentNick)->NachrichtAnhaengen(_T("INVITE"),benutzer,raum);
}

// URL des Raumes anzeigen
void Zentrale::irc_chanurl(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger = msg_data->params_a[1];
    fenster(empfaenger)->NachrichtAnhaengen(_T("CHANNEL_URL"), msg_data->params_a[2]);
}

// Thema des Raums anzeigen
void Zentrale::irc_topic(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger = msg_data->params_a[1];
    fenster(empfaenger)->ThemaAendern(msg_data->params_a[2]);
}

// Thema wurde gesetzt wann.
void Zentrale::irc_topicwhotime(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger = msg_data->params_a[1];

    // Zeitstempel erzeugen
    long unsigned int raw_time_long;
    msg_data->params_a[3].ToULong(&raw_time_long,10);

    char timestamp[50];
    time_t raw_time;
    raw_time = raw_time_long;

    tm *local_time;
    local_time = localtime(&raw_time);
    strftime(timestamp, 50, "%d.%m.%Y %X", local_time);
    wxString zeit(timestamp, wxConvUTF8);

    fenster(empfaenger)->NachrichtAnhaengen(_T("TOPICWHOTIME"), msg_data->params_a[2], zeit);
}

// Thema des Raums anzeigens
void Zentrale::irc_requestedtopic(const IRC_NACHRICHT *msg_data)
{
   wxString empfaenger = msg_data->params_a[0];
   wxString benutzer = msg_data->nick;
   fenster(empfaenger)->ThemaAendern(msg_data->params_a[1], benutzer);
}

// Auf Ping mit Pong antworten
void Zentrale::irc_pong(const IRC_NACHRICHT *msg_data)
{
    irc->irc_send_pong(msg_data->params_a[0].mb_str());
}

// RPL_UNAWAY / 305
void Zentrale::irc_unaway(const IRC_NACHRICHT *msg_data)
{
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("RPL_UNAWAY"));
}

// RPL_NOWAWAY / 306
void Zentrale::irc_nowaway(const IRC_NACHRICHT *msg_data)
{
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("RPL_NOWAWAY"));
}

//whois Antworten anzeigen

void Zentrale::irc_whoisuser(const IRC_NACHRICHT *msg_data)
{
    wxString nick = msg_data->params_a[1];
    wxString user = msg_data->params_a[2];
    wxString host = msg_data->params_a[3];
    wxString name = msg_data->params_a[5];

    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("WHOIS_USER"), nick, user, host, name);
}

void Zentrale::irc_whoisaway(const IRC_NACHRICHT *msg_data)
{
    wxString nick = msg_data->params_a[1];
    wxString text = msg_data->params_a[2];
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("WHOIS_AWAY"), nick, text);
}

void Zentrale::irc_whoischan(const IRC_NACHRICHT *msg_data)
{
    wxString nick = msg_data->params_a[1];
    wxString chans = msg_data->params_a[2];

    // Rechte noch beachten ([@|+]#channel)???
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("WHOIS_CHANNEL"), nick, chans);
}

void Zentrale::irc_whoisidle(const IRC_NACHRICHT *msg_data)
{
    wxString nick = msg_data->params_a[1];
    wxString sekunden = msg_data->params_a[2];
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("WHOIS_IDLE"), nick, sekunden);
}

void Zentrale::irc_whoisserver(const IRC_NACHRICHT *msg_data)
{
    wxString nick = msg_data->params_a[1];
    wxString server = msg_data->params_a[2];
    wxString servernachricht = msg_data->params_a[3];
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("WHOIS_SERVERMSG"), nick, server, servernachricht);
}

void Zentrale::irc_whoisspecial(const IRC_NACHRICHT *msg_data)
{
    wxString nick = msg_data->params_a[1];
    wxString special = msg_data->params_a[2];
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("WHOIS_SPECIAL"), nick, special);
}

void Zentrale::irc_whoisactually(const IRC_NACHRICHT *msg_data)
{
    wxString nick = msg_data->params_a[1];
    wxString server = msg_data->params_a[2];
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("WHOIS_ACTUALLY"), nick, server);
}

void Zentrale::irc_kick(const IRC_NACHRICHT *msg_data)
{
    wxString empfaenger = msg_data->params_a[0];
    wxString benutzer = msg_data->params_a[1];
    wxString kommentar = msg_data->params_a[2];

    fenster(empfaenger)->BenutzerEntfernen(benutzer);
    
    if(benutzer.Upper() == irc->CurrentNick.Upper())
    // Entweder wurde man bedauerlicherweise selber aus dem Raum geschmissen ...
    {
        if(kommentar == msg_data->nick)
            fenster(empfaenger)->NachrichtAnhaengen(_T("KICK_SELF"), msg_data->nick);
        else
            fenster(empfaenger)->NachrichtAnhaengen(_T("KICK_SELF"), msg_data->nick, kommentar);
    }
    else
    // ... oder gluecklicherweise ein anderer Benutzer
    {
        if(kommentar == msg_data->nick)
            fenster(empfaenger)->NachrichtAnhaengen(_T("KICK"), msg_data->nick, benutzer);
        else
            fenster(empfaenger)->NachrichtAnhaengen(_T("KICK"), msg_data->nick, benutzer, kommentar);
    }
    

}

// ERR_*-Nachrichten
// Verschiedene Fehlermeldungen anzeigen
void Zentrale::irc_fehler(const IRC_NACHRICHT *msg_data)
{
    wxString fehler = msg_data->cmd;
    for(int i = 0; i < msg_data->params_i; i++)
    {
        fehler += _T(" ");
        fehler += msg_data->params_a[i];
    }
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("ERR_IRC"), fehler);
}


// Einfache Anzeige von Nachrichten
void Zentrale::irc_einfach(const IRC_NACHRICHT *msg_data)
{
    wxString nachricht = _T("");
    for(int i = 1; i < msg_data->params_i; i++)
    // beginnend bei eins, damit der eigene Nickname nicht angezeigt wird.
    {
        nachricht += msg_data->params_a[i];
        nachricht += _T(" ");
    }
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("PRIVMSG_NOSENDER"),_T(""), nachricht);
}

// Fehler anzeigen. Nachricht wurde nicht behandelt bzw, Befehl nicht abgefragt
void Zentrale::irc_unbekannt(const IRC_NACHRICHT *msg_data)
{
    wxString fehler = msg_data->cmd;
    for(int i = 0; i < msg_data->params_i; i++)
    {
        fehler += _T(" ");
        fehler += msg_data->params_a[i];
    }
    fenster(irc->CurrentHostname)->NachrichtAnhaengen(_T("ERR_IRC_COMMAND_UNKNOWN"), fehler);
}
