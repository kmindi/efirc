/**
** efirc steht unter der "Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland Lizenz".
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the "Creative Commons Attribution-Share Alike 3.0 Germany License".
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// zentrale.cc
// Implementiert die Klasse Zentrale

#include <zentrale.h>

// main funkition erstellen, Instanz der Klasse Zentrale erstellen
// mit DECLARE_APP(Zentrale/) und wxGetApp() kann auf Funktionen dieser Klasse zugegrifffen werden
IMPLEMENT_APP(Zentrale) // erstellt main funktion

bool Zentrale::OnInit()
{
    efirc_version_string = _T("0.3 testing");

    Ereignisvw = new Ereignisverwalter; // einen Ereignisverwalter erzeugen

    wxString standardkonfiguration_text = standardkonfiguration();
    config = new Konfiguration(_T("efirc.cfg"), standardkonfiguration_text);

    // Laufzeitkonfiguration anpassen
    Konfiguration_anpassen();


    // irc zeigt auf die IRC-Instanz
    //irc = new IRC(_T("6667"),_T("irc.freenode.net"),_T("efirc_test"),_T("efirc_test"),_T("efirc_test"),_T("PASS"));
    //irc = new IRC(_T("6667"),_T("localhost"),_T("efirc_test"),_T("efirc_test"),_T("efirc_test"),_T("PASS"));

    irc = new IRC(
    config->parsecfgvalue(_T("irc_port")),
    config->parsecfgvalue(_T("irc_server")),
    config->parsecfgvalue(_T("irc_nickname")),
    config->parsecfgvalue(_T("irc_username")),
    config->parsecfgvalue(_T("irc_realname")),
    _T("PASS"));


    // FENSTER
    // dafuer sorgen, dass kein zeiger festgelegt ist
    for(int i=0;i<max_fenster;i++) { zgr_fenster[i]=NULL; }


    // erste Instanz der Fenster-klasse erzeugen.
    // VERBINDUNG ZUR KONFIGURATION
    //raum = _T("#efirc");
    raum = config->parsecfgvalue(_T("irc_channel"));
    neuesFenster(raum);


    // Verlinkung der IRC-Funktionen starten und IRC Threads starten
    // Eine Instanz der Fensterklasse muss erzeugt sein
    connect_thread();



    // TEST FUNKTIONEN

    return TRUE;
}

//Konfigurationsfunktionen

//Standardkonfigurationsinhalt
wxString Zentrale::standardkonfiguration()
{
    wxString standardkonfiguration_text;

    // Verbindungsinformationen
    standardkonfiguration_text =  _T("irc_server = irc.freenode.net\n");
    standardkonfiguration_text += _T("irc_port = 6667\n");
    standardkonfiguration_text += _T("irc_channel = #efirc\n");
    standardkonfiguration_text += _T("irc_username = efirc\n");
    standardkonfiguration_text += _T("irc_realname = %real_name\n");
    standardkonfiguration_text += _T("irc_nickname = user_%random_string\n");

    // Farben
    standardkonfiguration_text += _T("colour_background = #510000\n");
    standardkonfiguration_text += _T("colour_topic_background = #EEEEEE\n");
    standardkonfiguration_text += _T("colour_topic_foreground = #000000\n");
    standardkonfiguration_text += _T("colour_output_messages_background = #EEEEEE\n");
    standardkonfiguration_text += _T("colour_output_messages_foreground = #000000\n");
    standardkonfiguration_text += _T("colour_input_messages_background = #EEEEEE\n");
    standardkonfiguration_text += _T("colour_input_messages_foreground = #000000\n");
    standardkonfiguration_text += _T("colour_channel_users_background = #EEEEEE\n");
    standardkonfiguration_text += _T("colour_channel_users_foreground = #000000\n");
    standardkonfiguration_text += _T("colour_button_background = #EEEEEE\n");
    standardkonfiguration_text += _T("colour_button_foreground = #640C0C\n");
    standardkonfiguration_text += _T("colour_error = #FF0000\n");

    // Texte
    standardkonfiguration_text += _T("text_title = %param4 [%param1@%param2:%param3] - \n");
    standardkonfiguration_text += _T("text_quit_message = tschuess\n");

    // Schriftarten
    standardkonfiguration_text += _T("font_topic = \n");
    standardkonfiguration_text += _T("font_output_messages = \n");
    standardkonfiguration_text += _T("font_input_messages = \n");
    standardkonfiguration_text += _T("font_channel_users = \n");
    standardkonfiguration_text += _T("font_button = \n");

    // Standardtexte
    // S =s elber, fuer Nachrichten die man selber geschrieben hat
    // P = privat, fuer Nachrichten die nicht an einen Raum, sondern an einen selber geschrieben wurden.
    standardkonfiguration_text += _T("local_P_PRIVMSG = [ <%param1> %param2 ]\n");
    standardkonfiguration_text += _T("local_S_P_PRIVMSG = [ <%param1->%param2> %param3 ]\n");
    standardkonfiguration_text += _T("local_PRIVMSG = <%param1> %param2\n");
    standardkonfiguration_text += _T("local_PRIVMSG_NOSENDER = %param2\n");
    standardkonfiguration_text += _T("local_ACTION = *%param1 [italic]%param2[/italic]\n");
    standardkonfiguration_text += _T("local_P_ACTION = [ *%param1 [italic]%param2[/italic] ]\n");
    standardkonfiguration_text += _T("local_TOPIC = The topic is: %param1\n");
    standardkonfiguration_text += _T("local_TOPIC_CHANGE = %param2 changed the topic to %param1\n");
    standardkonfiguration_text += _T("local_JOIN = %param1 joined the channel\n");
    standardkonfiguration_text += _T("local_PART = %param1 has left the channel (%param2)\n");
    standardkonfiguration_text += _T("local_QUIT = %param1 has left the network (%param2)\n");
    standardkonfiguration_text += _T("local_INVITE = %param1 invited you to %param2\n");
    standardkonfiguration_text += _T("local_NICK = %param1 changed his nickname to %param2\n");
    standardkonfiguration_text += _T("local_MOTD = %param1\n");
    standardkonfiguration_text += _T("local_CTCP = [ <%param1@CTCP> %param2 ]\n");
    standardkonfiguration_text += _T("local_S_CTCP = [ <%param1@CTCP->%param2> %param3 ]\n");
    standardkonfiguration_text += _T("local_MODE = %param1 set mode %param2\n");
    standardkonfiguration_text += _T("local_AWAY = You have been marked as being away (%param1)\n");
    standardkonfiguration_text += _T("local_RPL_UNAWAY = You are not longer marked as being away\n");
    standardkonfiguration_text += _T("local_RPL_NOWAWAY = You have been marked as being away\n");
    standardkonfiguration_text += _T("local_WHOIS_USER = [ WHOIS: %param1 (%param2@%param3 - %param4) ]\n");
    standardkonfiguration_text += _T("local_WHOIS_AWAY = [ WHOIS: %param1 is away %param2 ]\n");
    standardkonfiguration_text += _T("local_WHOIS_CHANNEL = [ WHOIS: %param1 is in %param2 ]\n");
    standardkonfiguration_text += _T("local_WHOIS_IDLE = [ WHOIS: %param1 is idle since %param2 seconds ]\n");
    standardkonfiguration_text += _T("local_WHOIS_SERVERMSG = [ WHOIS: %param1 %param2 %param3 ]\n");
    standardkonfiguration_text += _T("local_WHOIS_ACTUALLY = [ WHOIS: %param1 is actually using host %param2 ]\n");


    return standardkonfiguration_text;

}

wxString Zentrale::zufallstext(int anzahl_zeichen)
{
    wxString rndstring;
    // Zufallsgenerator initialisieren
    srand(time(NULL));

    for(int i = 1; i <= anzahl_zeichen; i++)
    {
        rndstring += wxChar(65 + rand()%26);
    }

    return rndstring;
}

// Konfiguration anpassen
void Zentrale::Konfiguration_anpassen()
{
    // Platzhalter ersetzen

    // [efirc_version]
    config->edit_cfg_replace(_T("%efirc_version"), efirc_version_string);
    // [efirc_random_string]
    config->edit_cfg_replace(_T("%random_string"), zufallstext(4));
    // [efirc_real_name]
    config->edit_cfg_replace(_T("%real_name"), wxGetUserId());
}

// Funktionen die auf die Fenster-Klasse zugreifen bzw. auf Instanzen eben dieser
void Zentrale::neuesFenster(wxString namedesfensters)
{
    bool fenstererzeugt = false;

    // nach freier Nummer suchen
    int i = 0;
    for(int j = 0; j < max_fenster ; j++)
    {
        if(fenstername[j].Upper() == namedesfensters.Upper())
        return;
    }
    while(fenstererzeugt == false && i < max_fenster)
    {
        if(zgr_fenster[i] == NULL)
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
    zgr_fenster[fensternummer]->Destroy();
    zgr_fenster[fensternummer] = NULL;
    fenstername[fensternummer] = _T("");
}

void Zentrale::fensterzerstoeren(wxString namedesfensters)
{
    for(int i = 0;i < max_fenster;i++)
    {
        if(fenstername[i].Upper() == namedesfensters.Upper())
        {
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
    if(name.Upper() == irc->CurrentNick.Upper())
    {
        // mit SetTopWindow zuletzt als oberstes Fenster festgelegtes Fenster suchen
        // und Zeiger casten, damit er dem Rueckgabetyp "Fenster*" entspricht
        return dynamic_cast<Fenster*>(GetTopWindow());
    }
    else
    {
        int i = 0; // laufende Nummer auf 0 setzen
        while(i < max_fenster && zgr == NULL) // solange i kleiner 10 ist und der zeiger noch null ist wird der inhalt der schleife ausgefuehrt
        {
            if(fenstername[i].Upper() == name.Upper()) // sobald der name uebereinstimmt wird der zeiger auf diesen frame zurueckgegeben
            {
                return zgr_fenster[i];
            }
            if(i == max_fenster - 1)
            // falls keine uebereinstimmung gefunden wurde, Fehler ausgeben und oberstes Fenster als Ausgabe setzten
            {
                // mit SetTopWindow zuletzt als oberstes Fenster festgelegtes Fenster suchen
                // und Zeiger casten, damit er dem Rueckgabetyp "Fenster*" entspricht
                zgr = dynamic_cast<Fenster*>(GetTopWindow());
                zgr->Fehler(3,name);
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
        for(int i = 0; i < max_fenster; i++)
        {
            if(fenstername[i].Upper() == name.Upper())
            {
                return zgr_fenster[i];
            }
        }
    }
}

void Zentrale::BefehlVerarbeiten(int fensternummer, wxString befehl)
{
    //fenstername[fensternummer]

    wxChar leerzeichen = _T(' ');
    wxString befehl_name = befehl.BeforeFirst(leerzeichen);
    wxString befehl_parameter = befehl.AfterFirst(leerzeichen);


    if(befehl_name.Upper() == _T("QUIT"))
    {
        wxString quitmessage = _T("");

        if(befehl_parameter == _T(""))
        {
            quitmessage = config->parsecfgvalue(_T("text_quit_message"));
        }
        else
        {
            quitmessage = befehl_parameter;
        }

        irc->disconnect_server(quitmessage.mb_str()); // Verbindung zum Server mit gegebener Nachricht trennen

        // kurz warten, damit die Verbindung ordnungsgemaess getrennt werden kann (Nachricht soll noch gesendet werden)
        #if defined linux || defined __unix__
        usleep(30000);
        #else
        Sleep(30);
        #endif

        // Alle Fenster zerstoeren
        for(int i = 0; i < max_fenster; i++)
        {
            if(!(wxGetApp().zgr_fenster[i]==NULL))
            // nicht in nicht vorhandenen Fenstern
            {
                zgr_fenster[i]->Destroy();
            }
        }
    }

    else if(befehl_name.Upper() == _T("JOIN") && befehl_parameter != _T(""))
    {
        irc->send_join(befehl_parameter.mb_str());
        // Auf eigenen JOIN Warten und dann neues Fenster aufmachen
    }

    else if(befehl_name.Upper() == _T("PART") || befehl_name.Upper() == _T("LEAVE"))
    {
        if(befehl_parameter == _T(""))
        {
            irc->send_part(fenstername[fensternummer].mb_str());
        }
        else
        {
            irc->send_part(befehl_parameter.mb_str());
        }
    }

    else if(befehl_name.Upper() == _T("NICK") && befehl_parameter != _T(""))
    {
            irc->send_nick(befehl_parameter.mb_str()); // Nickname senden
            irc->WantedNick = befehl_parameter;
            // gewollten Nickname speichern, damit die nickinuse-Funktion richtig reagieren kann.
    }

    else if(befehl_name.Upper() == _T("INVITE") && befehl_parameter != _T(""))
    {
        wxString nickname = befehl_parameter.BeforeFirst(leerzeichen);
        wxString raum = befehl_parameter.AfterFirst(leerzeichen);

        if(raum == _T(""))
        {
            raum = fenstername[fensternummer];
        }

        wxGetApp().irc->send_invite(nickname.mb_str(), raum.mb_str());
    }

    else if(befehl_name.Upper() == _T("ME") && befehl_parameter != _T(""))
    {
        // GEHT NOCH NICHT BEI NACHRICHTEN AN BENUTZER
        wxString me_text = _T("\001ACTION ") + befehl_parameter + _T("\001");
        irc->send_privmsg(fenstername[fensternummer].mb_str(), me_text.mb_str());
        zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("ACTION"), irc->CurrentNick, befehl_parameter);
    }

    else if(befehl_name.Upper() == _T("TOPIC"))
    {
        if(befehl_parameter == _T(""))
        {
            irc->send_topic(fenstername[fensternummer].mb_str());
        }
        else
        {
            irc->send_topic(fenstername[fensternummer].mb_str(),befehl_parameter.mb_str());
        }
    }

    else if((befehl_name.Upper() == _T("QUERY") || befehl_name.Upper() == _T("MSG")) && befehl_parameter != _T(""))
    {
        wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
        wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);

        irc->send_privmsg(empfaenger.mb_str(),nachricht.mb_str());
        zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("S_P_PRIVMSG"),irc->CurrentNick, empfaenger, nachricht);
    }

    else if(befehl_name.Upper() == _T("AWAY"))
    {
        if(befehl_parameter == _T(""))
        {
            irc->send_away();
        }
        else
        {
            irc->send_away(befehl_parameter.mb_str());
            zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("AWAY"),befehl_parameter);
        }
    }

    else if(befehl_name.Upper() == _T("CTCP") && befehl_parameter != _T(""))
    {
        wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
        wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);

        irc->send_privmsg(empfaenger.mb_str(), (_T("\001") + nachricht + _T("\001")).mb_str());

        zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("CTCP"),irc->CurrentNick, empfaenger, nachricht);
    }

    else if(befehl_name.Upper() == _T("WHOIS") && befehl_parameter != _T(""))
    {
        irc->send_whois(befehl_parameter.mb_str());
    }

    // sonstige Befehle
    else if(befehl_name.Upper() == _T("CLEAR"))
    {
        zgr_fenster[fensternummer]->AusgabefeldLeeren();
    }
    
    // Wenn der Befehl nicht gefunden wurde oder zu wenig parameter uebergeben wurden
    else
    {
        if(befehl_parameter == _T(""))
        {
            zgr_fenster[fensternummer]->Fehler(4,befehl_name);
        }
        else
        {
            zgr_fenster[fensternummer]->Fehler(4,befehl_name + _T(" ") + befehl_parameter);
        }
    }
    
        
}

void Zentrale::NachrichtSenden(int fensternummer, wxString nachricht)
{
    irc->send_privmsg(fenstername[fensternummer].mb_str(), nachricht.mb_str());
    // Nachricht im Textfenster anzeigen
    // UNICODE?
    //AKTUELLER NICKNAME?
    zgr_fenster[fensternummer]->NachrichtAnhaengen(_T("PRIVMSG"),irc->CurrentNick,nachricht);
}

void Zentrale::EingabeVerarbeiten(int fensternummer, wxString eingabe)
{
    //Ist es ein Befehl?
    wxString befehlsprefix = _T("/");
    if(eingabe.StartsWith(befehlsprefix, &eingabe))
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
void irc_allgemein(const irc_msg_data *msg_data, void *cp);

void Zentrale::connect_thread()
{
    
    irc->set_default_link_function(&irc_allgemein);
    
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
    wxString quitmessage = config->parsecfgvalue(_T("text_quit_message"));
    irc->disconnect_server(quitmessage.mb_str());
}
