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
    efirc_version_string = _T("0.3-"__SVNREVISION__);

    Ereignisvw = new Ereignisverwalter; // einen Ereignisverwalter erzeugen

    wxString standardkonfiguration_text = standardkonfiguration();
    config = new Konfiguration(_T("efirc.cfg"), standardkonfiguration_text);

    // Laufzeitkonfiguration anpassen
    Konfiguration_anpassen();

    // irc zeigt auf die IRC-Instanz
    //irc = new IRC(_T("6667"),_T("irc.freenode.net"),_T("efirc_test"),_T("efirc_test"),_T("efirc_test"),_T("PASS"));

    irc = new IRC(
    config->parsecfgvalue(_T("irc_port")),
    config->parsecfgvalue(_T("irc_server")),
    config->parsecfgvalue(_T("irc_nickname")),
    config->parsecfgvalue(_T("irc_username")),
    config->parsecfgvalue(_T("irc_realname")),
    zufallstext(8));

    // Reiteranzeige
        reiterframe = new wxFrame(NULL, wxID_ANY, _T("REITERFRAME"), wxDefaultPosition, wxSize(724, 416), wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxRESIZE_BORDER, _T("reiterframe"));
        wxGetApp().m_mgr.SetManagedWindow(reiterframe); // Das gerade erstellte Fenster dem AUI Verwalter zuweisen
        
        TitelSetzen(); // Titel anpassen
        
        reiterframe->SetIcon(wxIcon(icon)); // Icon setzten
        // Neues Reiteransichtobjekt erzeugen
        notebook = new wxAuiNotebook (reiterframe, wxID_ANY, wxPoint(0, 0), wxSize(720, 412), wxAUI_NB_TAB_MOVE | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_TOP);
        
        reiterframe->SetSizeHints(wxSize(760, 492)); // Breite + 20, Hoehe +60 ???
        // int breite, hoehe;
        // notebook->GetSize(&breite, &hoehe);
        // reiterframe->SetSizeHints(wxSize(breite, hoehe)); 
        reiterframe->Fit();
        reiterframe->Show(true);
        
    // erste Instanz der Fenster-klasse erzeugen.
    Fenster* fenster = neuesFenster(irc->CurrentHostname); // mit dem Namen des aktuellen Servers

    // IRC-Funktion mit IRCInterface verknuepfen und IRC Threads starten
    // Eine Instanz der Fensterklasse muss erzeugt sein
    irc->irc_set_default_link_function(&irc_allgemein);
    
    threads_starten();
        
    return TRUE;
}

// Titel des Hauptfensters setzen
void Zentrale::TitelSetzen(wxString titel, wxString nick)
{
    if(nick == _T(""))
        nick = irc->CurrentNick;
    
    // Titel anpassen
    wxString titel_text = wxGetApp().config->parsecfgvalue(_T("text_title"));
    
    
    
    titel_text.Replace(_T("%param1"), nick);
    titel_text.Replace(_T("%param2"), irc->CurrentHostname);
    titel_text.Replace(_T("%param3"), irc->CurrentPort);
    titel_text.Replace(_T("%param4"), titel);
    
    reiterframe->SetTitle(titel_text + _T("efirc ") + wxGetApp().efirc_version_string);
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

    standardkonfiguration_text += _T("max_DONT_SHOW_USERLIST_CHANGES = 25\n");
    standardkonfiguration_text += _T("bool_automatic_nickchange_if_in_use = 1\n");

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

    // Schriftarten
    standardkonfiguration_text += _T("font_topic = \n");
    standardkonfiguration_text += _T("font_output_messages = \n");
    standardkonfiguration_text += _T("font_input_messages = \n");
    standardkonfiguration_text += _T("font_channel_users = \n");
    standardkonfiguration_text += _T("font_button = \n");

    // Standardtexte
    // S =s elber, fuer Nachrichten die man selber geschrieben hat
    // P = privat, fuer Nachrichten die nicht an einen Raum, sondern an einen selber geschrieben wurden.
    // BL = Benutzerliste
    standardkonfiguration_text += _T("local_P_PRIVMSG = -%param1- %param2\n");
    standardkonfiguration_text += _T("local_S_P_PRIVMSG = >%param1< %param2\n");
    standardkonfiguration_text += _T("local_PRIVMSG = <%param1> %param2\n");
    standardkonfiguration_text += _T("local_PRIVMSG_NOSENDER = %param2\n");
    standardkonfiguration_text += _T("local_ACTION = *%param1 [italic]%param2[/italic]\n");
    standardkonfiguration_text += _T("local_P_ACTION = *%param1 [italic]%param2[/italic]\n");
    standardkonfiguration_text += _T("local_CHANNEL_URL = The channel url is: %param1\n");
    standardkonfiguration_text += _T("local_TOPIC = The topic is: %param1\n");
    standardkonfiguration_text += _T("local_TOPICWHOTIME = Topic was set by %param1 at %param2\n");
    standardkonfiguration_text += _T("local_TOPIC_CHANGE = %param2 changed the topic to %param1\n");
    standardkonfiguration_text += _T("local_BL_JOIN = %param1 joined the channel\n");
    standardkonfiguration_text += _T("local_BL_PART = %param1 has left the channel (%param2)\n");
    standardkonfiguration_text += _T("local_BL_QUIT = %param1 has left the network (%param2)\n");
    standardkonfiguration_text += _T("local_INVITE = %param1 invited you to %param2\n");
    standardkonfiguration_text += _T("local_BL_NICK = %param1 changed his nickname to %param2\n");
    standardkonfiguration_text += _T("local_CTCP = -%param1@CTCP- %param2\n");
    standardkonfiguration_text += _T("local_S_CTCP = >%param1@CTCP< %param2\n");
    standardkonfiguration_text += _T("local_MODE = %param1 set mode %param2\n");
    standardkonfiguration_text += _T("local_AWAY = You have been marked as being away (%param1)\n");
    standardkonfiguration_text += _T("local_RPL_UNAWAY = You are not longer marked as being away\n");
    standardkonfiguration_text += _T("local_RPL_NOWAWAY = You have been marked as being away\n");
    standardkonfiguration_text += _T("local_WHOIS_USER = WHOIS: %param1 (%param2@%param3 - %param4)\n");
    standardkonfiguration_text += _T("local_WHOIS_AWAY = WHOIS: %param1 is away %param2\n");
    standardkonfiguration_text += _T("local_WHOIS_CHANNEL = WHOIS: %param1 is in %param2\n");
    standardkonfiguration_text += _T("local_WHOIS_IDLE = WHOIS: %param1 is idle since %param2 seconds\n");
    standardkonfiguration_text += _T("local_WHOIS_SERVERMSG = WHOIS: %param1 %param2 %param3\n");
    standardkonfiguration_text += _T("local_WHOIS_SPECIAL = WHOIS: %param1 %param2\n");
    standardkonfiguration_text += _T("local_WHOIS_ACTUALLY = WHOIS: %param1 is actually using host %param2\n");
    standardkonfiguration_text += _T("local_KICK = %param1 has kicked %param2 (%param3)\n");
    standardkonfiguration_text += _T("local_KICK_SELF = You were kicked by %param1 (%param2)\n");
    standardkonfiguration_text += _T("local_quit_message = efirc sagt tschuess\n");
    
    standardkonfiguration_text += _T("local_label_button = Send\n");
    standardkonfiguration_text += _T("local_label_input = Put your message here and press Enter\n");

    // Fehler (werden in konfigurierter Farbe dargestellt)
    standardkonfiguration_text += _T("local_ERR_IRC = (!) %param1\n"); // case 2
    standardkonfiguration_text += _T("local_ERR_COMMAND_UNKNOWN = (!) Unknown command (%param1)\n"); // case 4
    standardkonfiguration_text += _T("local_ERR_IRC_COMMAND_UNKNOWN = (!) Unknown IRC command (%param1)\n"); // case 5
    standardkonfiguration_text += _T("local_ERR_COMMAND_MISSING_PARAMETER = (!) Missing parameter for \"%param1\"\n"); // case 5

    // Anzeige bestimmter Dialog oder anderer nicht irc spezifischer Nachrichten
    standardkonfiguration_text += _T("local_DLG_NEWNICK_NICKINUSE_TEXT = The Nickname you try to use is already in use. You have to choose a new one:\n"); // case 5
    standardkonfiguration_text += _T("local_DLG_NEWNICK_CAPTION = Change your Nickname\n"); // case 5

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
        config->edit_cfg_replace(_T("%efirc_version"), efirc_version_string);
        config->edit_cfg_replace(_T("%random_string"), zufallstext(4));
        config->edit_cfg_replace(_T("%real_name"), wxGetUserId());
}

// Funktionen die auf die Fenster-Klasse zugreifen bzw. auf Instanzen eben dieser

// Gibt die Anzahl der offenen Fenster zurueck
unsigned int Zentrale::anzahl_offene_fenster()
{
    return zgr_fenster.size();
}

// Neues Fenster erzeugen und Zeiger auf dieses zurueckgeben
Fenster* Zentrale::neuesFenster(wxString namedesfensters)
{
    map<wxString, Fenster*>::iterator iter = zgr_fenster.find(namedesfensters.Upper());

    // Neues Fenster nur erstellen wenn noch keines erstellt wurde
    if(iter == zgr_fenster.end())
    {
        // Neues Fenster erstellen und Adresse des Speicherbereichs in zgr merken
        Fenster *zgr = new Fenster(namedesfensters, wxPoint(0, 0), wxSize(720, 412));

        
        
        // Neuen Eintrag in der Map erstellen
        zgr_fenster.insert(make_pair(namedesfensters.Upper(), zgr));

        zgr_fenster[namedesfensters.Upper()]->fenster_name = namedesfensters;
        zgr_fenster[namedesfensters.Upper()]->TitelSetzen(namedesfensters); // Titel anpassen
        zgr_fenster[namedesfensters.Upper()]->Show(TRUE); // Fenster in den Vordergrund holen

        notebook->AddPage((wxWindow *)zgr, namedesfensters);
        
        return zgr;
    }

    else
    {
        return zgr_fenster[namedesfensters.Upper()];
    }
}

// Fenster zerstoeren
void Zentrale::fensterzerstoeren(wxString namedesfensters)
{
    if(zgr_fenster.end() != zgr_fenster.find(namedesfensters.Upper()))
    // Nur versuchen zu zerstoeren wenn das Fenster auch existiert
    {
        notebook->DeletePage(notebook->GetPageIndex((wxWindow*) zgr_fenster[namedesfensters.Upper()]));
        zgr_fenster.erase(namedesfensters.Upper());
    }
}

// Gibt IMMER einen Zeiger auf ein Fenster zurueck
Fenster* Zentrale::fenstersuchen(wxString name)
{
    map<wxString, Fenster*>::iterator iter = zgr_fenster.find(name.Upper());

    if(iter != zgr_fenster.end())
    // Wenn das Fenster gefunden wurde
    {
        return zgr_fenster[name.Upper()];
    }
    else
    {
        if(name.Upper() == irc->CurrentNick.Upper())
        // Wenn nicht, ist es entweder eine Nachricht an einen selber...
        {
            return fenster(name);
        }
        else
        // ...oder eine andere Nachricht die im Serverfenster angezeigt werden soll
        {
            return fenster(irc->CurrentHostname);
        }
    }
}

// Gibt  IMMER einen Zeiger auf ein Fenster zurueck
Fenster* Zentrale::fenster(wxString name)
{
    map<wxString,Fenster*>::iterator iter = zgr_fenster.find(name.Upper());

    if(iter != zgr_fenster.end())
    // Wenn das Fenster gefunden wurde
    {
        return zgr_fenster[name.Upper()];
    }
    else
    // Wenn nicht ein neues erstellen
    {
        return neuesFenster(name);
    }
}

void Zentrale::BefehlVerarbeiten(wxString fenstername, wxString befehl)
{
    //fenstername[fensternummer]

    wxChar leerzeichen = _T(' ');
    wxString befehl_name = befehl.BeforeFirst(leerzeichen);
    wxString befehl_parameter = befehl.AfterFirst(leerzeichen);

    befehl_name.MakeUpper();

    bool parameter_erwartet = false;
    bool parameter_vorhanden = false;

    if(befehl_parameter != _T("")) parameter_vorhanden = true;

    wxLocale *local = new wxLocale(wxLANGUAGE_GERMAN); // wird fuer ueber-Befehl gebraucht

    // Befehle die nicht unbedingt einen Parameter erwarten
    if(befehl_name == _T("QUIT") || befehl_name == _T("EXIT") || befehl_name == _T("BEENDEN"))
    {
        wxString quitmessage = _T("");

        if(!parameter_vorhanden)
        {
            quitmessage = config->parsecfgvalue(_T("local_quit_message"));
        }
        else
        {
            quitmessage = befehl_parameter;
        }

        irc->irc_disconnect_server(quitmessage.mb_str()); // Verbindung zum Server mit gegebener Nachricht trennen

        // Alle Fenster zerstoeren
        for(map< wxString, Fenster* >::iterator i = zgr_fenster.begin(); i != zgr_fenster.end(); i++)
        {
            if(!(wxGetApp().zgr_fenster[i->first]==NULL))
            // nicht in nicht vorhandenen Fenstern
            {
                fensterzerstoeren(i->first);
            }
        }
        
        reiterframe->Destroy();
        
    }

    else if(befehl_name == _T("PART") || befehl_name == _T("LEAVE"))
    {
        if(!parameter_vorhanden)
        {
            irc->irc_send_part(fenstername.mb_str());
        }
        else
        {
            irc->irc_send_part(befehl_parameter.mb_str());
        }
    }

    else if(befehl_name == _T("TOPIC"))
    {
        if(!parameter_vorhanden)
        {
            irc->irc_send_topic(fenstername.mb_str());
        }
        else
        {
            irc->irc_send_topic(fenstername.mb_str(), befehl_parameter.mb_str());
        }
    }

    else if(befehl_name == _T("CLEAR"))
    {
        zgr_fenster[fenstername.Upper()]->AusgabefeldLeeren();
    }

    else if(befehl_name == _T("ABOUT") || befehl_name == _T("\u00dcBER"))
    // \u00DC = grosses U_UMLAUT
    {
        zeige_ueber();
    }

    else if(befehl_name == _T("AWAY"))
    {
        if(!parameter_vorhanden)
        {
            irc->irc_send_away();
        }
        else
        {
            irc->irc_send_away(befehl_parameter.mb_str());
            zgr_fenster[fenstername.Upper()]->NachrichtAnhaengen(_T("AWAY"), befehl_parameter);
        }
    }

    else if(befehl_name == _T("JOIN"))
    {
        if(parameter_vorhanden)
        irc->irc_send_join(befehl_parameter.mb_str());

        else parameter_erwartet = true;
        // Auf eigenen JOIN Warten und dann neues Fenster aufmachen
    }

    else if(befehl_name == _T("NICK"))
    {
        if(parameter_vorhanden)
        {
            irc->irc_send_nick(befehl_parameter.mb_str()); // Nickname senden
            irc->WantedNick = befehl_parameter;
            // gewollten Nickname speichern, damit die nickinuse-Funktion richtig reagieren kann.
        }
        else parameter_erwartet = true;
    }

    else if(befehl_name == _T("INVITE"))
    {
        if(parameter_vorhanden)
        {
            wxString nickname = befehl_parameter.BeforeFirst(leerzeichen);
            wxString raum = befehl_parameter.AfterFirst(leerzeichen);

            if(raum == _T(""))
            {
                raum = fenstername;
            }

            wxGetApp().irc->irc_send_invite(nickname.mb_str(), raum.mb_str());
        }
        else parameter_erwartet = true;
    }

    else if(befehl_name == _T("ME"))
    {
        if(parameter_vorhanden)
        {
            // GEHT NOCH NICHT BEI NACHRICHTEN AN BENUTZER
            wxString me_text = _T("\001ACTION ") + befehl_parameter + _T("\001");
            irc->irc_send_privmsg(fenstername.mb_str(), me_text.mb_str());
            zgr_fenster[fenstername.Upper()]->NachrichtAnhaengen(_T("ACTION"), irc->CurrentNick, befehl_parameter);
        }
        else parameter_erwartet = true;
    }

    else if((befehl_name == _T("QUERY") || befehl_name == _T("MSG")))
    {
        if(parameter_vorhanden)
        {
            wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
            wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);

            irc->irc_send_privmsg(empfaenger.mb_str(),nachricht.mb_str());
            zgr_fenster[fenstername.Upper()]->NachrichtAnhaengen(_T("S_P_PRIVMSG"), empfaenger, nachricht);
        }
        else parameter_erwartet = true;
    }

    else if(befehl_name == _T("CTCP"))
    {
        if(parameter_vorhanden)
        {
            wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
            wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);

            irc->irc_send_privmsg(empfaenger.mb_str(), (_T("\001") + nachricht + _T("\001")).mb_str());

            zgr_fenster[fenstername.Upper()]->NachrichtAnhaengen(_T("S_CTCP"), empfaenger, nachricht);
        }
        else parameter_erwartet = true;
    }

    else if(befehl_name == _T("WHOIS"))
    {
        if(parameter_vorhanden)
            irc->irc_send_whois(befehl_parameter.mb_str());
        else
            parameter_erwartet = true;
    }
    
    else if((befehl_name == _T("KICK")))
    {
        if(parameter_vorhanden)
        {
            wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
            wxString nachricht = befehl_parameter.AfterFirst(leerzeichen);
            
            if(nachricht == _T(""))
                irc->irc_send_kick(fenstername.mb_str(), empfaenger.mb_str());
            else
                irc->irc_send_kick(fenstername.mb_str(), empfaenger.mb_str(), nachricht.mb_str());
        }
        
        else parameter_erwartet = true;
    }
    
    else if((befehl_name == _T("MODE")))
    {
        if(parameter_vorhanden)
        {
            wxString empfaenger = befehl_parameter.BeforeFirst(leerzeichen);
            wxString modi = befehl_parameter.AfterFirst(leerzeichen);
            
            if(modi == _T(""))
            // Wenn keine Modi angegeben werden, sollen die Modi abgefragt werden
                irc->irc_send_mode(empfaenger.mb_str());
            else
            // Angegebene Modi setzen
                irc->irc_send_mode(empfaenger.mb_str(), modi.mb_str());
        }
        
        else parameter_erwartet = true;
    }

    // Falls der Befehl nicht gefunden wurde
    else
    {
        if(parameter_vorhanden)
        zgr_fenster[fenstername.Upper()]->NachrichtAnhaengen(_T("ERR_COMMAND_UNKNOWN"), befehl_name + _T(" ") + befehl_parameter);
        else
        zgr_fenster[fenstername.Upper()]->NachrichtAnhaengen(_T("ERR_COMMAND_UNKNOWN"), befehl_name);
    }

    // Falls ein Parameter erwartet wurde (Befehl wurde dann auf jeden Fall gefunden
    if(parameter_erwartet)
    {
        zgr_fenster[fenstername.Upper()]->NachrichtAnhaengen(_T("ERR_COMMAND_MISSING_PARAMETER"), befehl_name);
    }

    delete local; // locale wird nur fuer ueber-Befehl gesetzt.
}

void Zentrale::NachrichtSenden(wxString fenstername, wxString nachricht)
{
    irc->irc_send_privmsg(fenstername.mb_str(), nachricht.mb_str());
    // Nachricht im Textfenster anzeigen
    // UNICODE?
    zgr_fenster[fenstername.Upper()]->NachrichtAnhaengen(_T("PRIVMSG"),irc->CurrentNick,nachricht);
}

void Zentrale::EingabeVerarbeiten(wxString fenstername, wxString eingabe)
{
    //Ist es ein Befehl?...
    wxString befehlsprefix = _T("/");
    if(eingabe.StartsWith(befehlsprefix, &eingabe))
    {
        BefehlVerarbeiten(fenstername, eingabe);
    }
    // oder eine normale Nachricht, dann an Raum/Benutzer senden
    else
    {
        NachrichtSenden(fenstername,eingabe);
    }
}

void Zentrale::zeige_ueber()
{
    wxFrame* dlg_ueber;

    // Neues Fenster erzeugen
        dlg_ueber = new wxFrame(NULL, wxID_ANY, _T("info@efirc"), wxDefaultPosition, wxSize(600,400), wxCAPTION | wxCLOSE_BOX | wxSTAY_ON_TOP | wxSYSTEM_MENU /*| wxRESIZE_BORDER*/, _T("AboutDialog"));

    // Fenster anpassen
        dlg_ueber->Center(); // In der Mitte anzeigen
        dlg_ueber->SetIcon(wxIcon(icon)); // Icon setzen

    // Informationen in einem nicht veraenderbaren Textfeld anzeigen
        wxTextCtrl *st_infotext = new wxTextCtrl(dlg_ueber, -1, _T(""), wxPoint(5,5), dlg_ueber->GetSize(), wxBORDER_NONE | wxTE_MULTILINE | wxTE_READONLY , wxDefaultValidator, _T("st_infotext"));

    // Textfeld anpassen
        // Schriftdarstellung aendern
        st_infotext->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxNORMAL, wxNORMAL, FALSE));
        // Textfarbe aendern
        st_infotext->SetBackgroundColour( _T("#510000") );
        st_infotext->SetForegroundColour( _T("#000000") );

    // Fenster anzeigen
    dlg_ueber->Show();

    wxString info = _T("");

    info.Append(_T("         _____.__                 \n"));
    info.Append(_T("   _____/ ____\\__|______   ____   \n"));
    info.Append(_T(" _/ __ \\   __\\|  \\_  __ \\_/ ___\\  \n"));
    info.Append(_T(" \\  ___/|  |  |  ||  | \\/\\  \\___  \n"));
    info.Append(_T("  \\___  >__|  |__||__|    \\___  > \n"));
    info.Append(_T("      \\/                      \\/  \n"));
    info.Append(_T("\n"));
    info.Append(_T("efirc - easy and fast internet relay chat client\n"));
    info.Append(efirc_version_string + _T("\n"));
    info.Append(_T("GNU GCC "__COMPILERVERSION__", "wxVERSION_STRING
    #if wxUSE_UNICODE == 1
    ", UNICODE"
    #endif
    ", "__COMPILEDATE__" "__COMPILETIME__"\n"));
    info.Append(_T("\n"));
    info.Append(_T("Deutsch:\n"));
    info.Append(_T("efirc steht unter der \"Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland\" Lizenz.\n"));
    info.Append(_T("http://creativecommons.org/licenses/by-sa/3.0/de/deed\n"));
    info.Append(_T("efirc wird von Kai Mindermann und Fabian Ruch entwickelt.\n"));
    info.Append(_T("\n"));
    info.Append(_T("English:\n"));
    info.Append(_T("efirc is licensed under the \"Creative Commons Attribution-Share Alike 3.0 Germany\" License.\n"));
    info.Append(_T("http://creativecommons.org/licenses/by-sa/3.0/de/deed.en\n"));
    info.Append(_T("efirc is developed by Kai Mindermann and Fabian Ruch.\n"));
    info.Append(_T("\n"));
    info.Append(_T("http://efirc.sf.net/"));

    wxString tmp_text = _T("");
    wxString prefix = _T("\n<efirc> ");

    // maximale Zeilenlaenge ist gleich Breite des Textfelds durch groesse eines Zeichens minus groesse eines Zeichens
    unsigned int zeilenlaenge = (st_infotext->GetSize().x / st_infotext->GetCharWidth()) ;

    while(info != _T(""))
    {
        tmp_text = info.BeforeFirst(_T('\n')); // Alles vor dem ersten Zeilenumbruch steht jetzt in tmp_text

        // solange tmp_text und prefix zusammen laenger als die zeilenlaenge sind
        while(tmp_text.Len() + prefix.Len() > zeilenlaenge)
        {
            st_infotext->AppendText(prefix + tmp_text.Left(zeilenlaenge - prefix.Len())); // prefix + den linken Teil von tmp_text bis zur Position (maximale Zeilenlaenge - prefix Laenge)
            tmp_text = tmp_text.Mid(zeilenlaenge - prefix.Len()); // In tmp_text steht jetzt nicht mehr der Text der schon angezeigt wurde
        }
        st_infotext->AppendText(prefix + tmp_text);

        info = info.AfterFirst(_T('\n')); // Alles nach dem ersten Zeilenumbruch steht jetzt in tmp_text
    }
}

void Zentrale::threads_starten()
{
    thrd_connect = new Thread(&Zentrale::connect_thread);
    thrd_recv = new Thread(&Zentrale::recv_thread);
    thrd_call = new Thread(&Zentrale::call_thread);
    
    if (thrd_connect->Create() == wxTHREAD_NO_ERROR) { thrd_connect->Run(); }
    if (thrd_recv->Create() == wxTHREAD_NO_ERROR) { thrd_recv->Run(); }
    if (thrd_call->Create() == wxTHREAD_NO_ERROR) { thrd_call->Run(); }
}

// Thread-Funktion fuer Verbindungsaufbau
void Zentrale::connect_thread()
{
    irc->connect();
}

// Thread-Funktion fuer recv_raw-Schleife
void Zentrale::recv_thread()
{
    irc->irc_receive_messages();
}

// Thread-Funkton fuer Abarbeitung der Befehlsschlange
void Zentrale::call_thread()
{
    irc->irc_call_command_queue_entries();
}

// OnExit wird ausgefuehrt wenn das ganze Programm beendet wird
int Zentrale::OnExit()
{
    BefehlVerarbeiten(_T(""), _T("/QUIT"));
    return 0;
}
