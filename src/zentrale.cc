/**
** efirc steht unter der MIT-Lizenz.
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the MIT License.
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// zentrale.cc
// Implementiert die Klasse Zentrale

#include <zentrale.h>

BEGIN_EVENT_TABLE(Hauptfenster, wxFrame)
    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, Hauptfenster::BeiFensterSchliessen)
END_EVENT_TABLE()

// main funkition erstellen, Instanz der Klasse Zentrale erstellen
// mit DECLARE_APP(Zentrale/) und wxGetApp() kann auf Funktionen dieser Klasse zugegrifffen werden
IMPLEMENT_APP(Zentrale) // erstellt main funktion

bool Zentrale::OnInit()
{
    efirc_version_string = _T("0.3-"__SVNREVISION__);

    srand(time(NULL));
    
    Ereignisvw = new Ereignisverwalter; // einen Ereignisverwalter erzeugen


    // KommandozeilenParser initialisieren
    parser = new wxCmdLineParser();
    parser->SetSwitchChars(_T("-"));
    parser->SetDesc(befehlszeilenparameterliste);
    parser->SetCmdLine(argc, argv);
    parser->Parse();
    
    if(parser->Found(_T("h")))
    return false;
    
    // Ueberpruefen ob auf der Kommandozeile er Pfad der Konfigurationsdatei angegeben wurde
    if(!parser->Found(_T("c")))
        config = new Konfiguration(_T("efirc.cfg"), standardkonfiguration_text);
    else
    {
        wxString pfad = _T("");
        parser->Found(_T("c"), &pfad);
        config = new Konfiguration(pfad, standardkonfiguration_text);
    }
        

    // Laufzeitkonfiguration anpassen
    Konfiguration_anpassen();
    
    // Befehlszeilenoptionen fuer IRC Verbindungsdaten beachten:
    wxString tmp_port,tmp_server,tmp_nick;
    if(!parser->Found(_T("p"), &tmp_port)) tmp_port = config->parsecfgvalue(_T("irc_port"));
    if(!parser->Found(_T("s"), &tmp_server)) tmp_server = config->parsecfgvalue(_T("irc_server"));
    if(!parser->Found(_T("n"), &tmp_nick)) tmp_nick = config->parsecfgvalue(_T("irc_nickname"));

    irc = new IRC(tmp_port, tmp_server, tmp_nick,
    config->parsecfgvalue(_T("irc_username")),
    config->parsecfgvalue(_T("irc_realname")),
    zufallstext(8));
    
    // Befehlszeilenoptionen fuer Raeume beachten
    if(!parser->Found(_T("ch"), &raeume)) raeume = config->parsecfgvalue(_T("irc_channel"));


    // Reiteranzeige
        reiterframe = new Hauptfenster(NULL, wxID_ANY, _T("REITERFRAME"), wxDefaultPosition, wxSize(724, 416), wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxRESIZE_BORDER, _T("reiterframe"));
        
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


// Wenn der Schliessen-Knopf eines Reiters gedrueckt wurde oder dieses Fenster geschlossen werden soll
void Hauptfenster::BeiFensterSchliessen(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    
    // Name des Fensters herausfinden (nicht fenster_name, weil es auch andere Fenster als Fensterklassenobjekte sein koennen)
    wxString namedesfensters = (ctrl->GetPageText(evt.GetSelection()));
	
    if(wxGetApp().zgr_fenster.find(namedesfensters.Upper()) != wxGetApp().zgr_fenster.end())
    {
        if(wxGetApp().anzahl_offene_fenster() == 1)
        {
            wxGetApp().EingabeVerarbeiten(namedesfensters, _T("/quit"));
        }
        else
        {
            if(namedesfensters.Upper() != wxGetApp().irc->CurrentHostname.Upper() && namedesfensters.Upper() != wxGetApp().irc->CurrentNick.Upper())
            {
                wxGetApp().EingabeVerarbeiten(namedesfensters, _T("/part"));
            }
        }
        // Zeiger entfernen
        wxGetApp().zgr_fenster.erase(namedesfensters.Upper());
    }
    
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
wxString Zentrale::zufallstext(int anzahl_zeichen)
{
    wxString rndstring;
    // Zufallsgenerator initialisieren

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
        config->edit_cfg_replace(_T("%italic"), _T("\x0009"));
        config->edit_cfg_replace(_T("%bold"), _T("\x0002"));
        config->edit_cfg_replace(_T("%underlined"), _T("\x001F"));
        config->edit_cfg_replace(_T("%colour"), _T("\x0003"));
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

        notebook->AddPage((wxWindow *)zgr, namedesfensters, true);
        
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
    
    // Wenn das Fenster gefunden wurde
    if(iter != zgr_fenster.end()) return zgr_fenster[name.Upper()];
    
    // Wenn nicht, ist es entweder eine Nachricht an einen selber...
    if(name.Upper() == irc->CurrentNick.Upper()) return fenster(name);
    
    // ...oder eine andere Nachricht die im Serverfenster angezeigt werden soll
    return fenster(irc->CurrentHostname);
}

// Gibt  IMMER einen Zeiger auf ein Fenster zurueck
Fenster* Zentrale::fenster(wxString name)
{
    map<wxString,Fenster*>::iterator iter = zgr_fenster.find(name.Upper());
    
    // Wenn das Fenster gefunden wurde
    if(iter != zgr_fenster.end()) return zgr_fenster[name.Upper()];
    
    // Wenn nicht ein neues erstellen
    return neuesFenster(name);
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
        
        
        // Hauptfenster zerstoeren
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

    else if(befehl_name == _T("CLOSE"))
    {
        
        
        
        if(!parameter_vorhanden)
        {
            if(anzahl_offene_fenster() == 1)
            {
                EingabeVerarbeiten(fenstername, _T("/quit"));
            }
            else
            {
                if(fenstername.Upper() != irc->CurrentHostname.Upper() && fenstername.Upper() != irc->CurrentNick.Upper())
                {
                    EingabeVerarbeiten(fenstername, _T("/part"));
                }
            }
            
            // Dieses Fenster schliessen und den Raum verlassen
            fensterzerstoeren(fenstername);
        }
        else
        {
            if(anzahl_offene_fenster() == 1)
            {
                EingabeVerarbeiten(befehl_parameter, _T("/quit"));
            }
            else
            {
                if(befehl_parameter.Upper() != irc->CurrentHostname.Upper() && befehl_parameter.Upper() != irc->CurrentNick.Upper())
                {
                    EingabeVerarbeiten(befehl_parameter, _T("/part"));
                }
            }
            
            // Das Fenster schliessen dessen Name uebergeben wurde und den Raum verlassen
            fensterzerstoeren(befehl_parameter);
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
    
    else if(befehl_name == _T("LIST"))
    {
        if(!parameter_vorhanden)
        {
            irc->irc_send_list();
        }
        else
        {
            irc->irc_send_list(befehl_parameter.mb_str());
        }
    }
    
    else if(befehl_name == _T("WHO"))
    {
        if(parameter_vorhanden)
        {
            irc->irc_send_who(befehl_parameter.mb_str());
        }
        else parameter_erwartet = true;
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
    wxPanel* dlg_ueber;

    // Neues Fenster erzeugen
    dlg_ueber = new wxPanel (notebook, wxID_ANY, wxPoint(0,0), wxSize(720, 412), 0, _T("AboutDialog"));
    
    dlg_ueber->SetBackgroundColour( config->parsecfgvalue(_T("colour_background")) );

    // Informationen in einem nicht veraenderbaren Textfeld anzeigen
    wxTextCtrl *st_infotext = new wxTextCtrl(dlg_ueber, -1, _T(""), wxPoint(5,5), dlg_ueber->GetSize(), wxTE_MULTILINE | wxTE_READONLY , wxDefaultValidator, _T("st_infotext"));

    // Textfeld anpassen
        // Schriftdarstellung aendern
        st_infotext->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxNORMAL, wxNORMAL, FALSE));
        // Textfarbe aendern
        st_infotext->SetBackgroundColour( config->parsecfgvalue(_T("colour_output_messages_background")) );
        st_infotext->SetForegroundColour( config->parsecfgvalue(_T("colour_output_messages_foreground")) );

    // Groessenaenderung anpassen
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(st_infotext, 1, wxEXPAND | wxALL, 3);
    dlg_ueber->SetSizer(sizer);
    
    
    // Fenster anzeigen
    dlg_ueber->Show();
    notebook->AddPage(dlg_ueber, _T("efirc Info"), true);
    
    // Infotext setzten
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

    while(info != _T(""))
    {
        st_infotext->AppendText(_T("<efirc> ") + info.BeforeFirst(_T('\n')) + _T('\n'));
        info = info.AfterFirst(_T('\n')); // Alles nach dem ersten Zeilenumbruch entfernen
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
    return 0;
}
