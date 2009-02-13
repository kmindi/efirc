// ui.cpp
// Implementiert die Fensterklasse

#include <ui.h>
#include <zentrale.h>
DECLARE_APP(Zentrale) // braucht man fuer wxGetApp() um damit auf die funktionen der Zentrale zuzugreifen

// Ereignisse definieren
BEGIN_EVENT_TABLE(Fenster, wxFrame)
    EVT_CLOSE      (Fenster::OnClose)
    EVT_BUTTON     (ID_WxButton_senden,
                    Fenster::WxButton_sendenClick)
    EVT_ACTIVATE (Fenster::BeiAktivierung)
END_EVENT_TABLE()

//Konstruktor der Fenster-Klasse
Fenster::Fenster(const wxString& title, const int& id, const wxPoint& pos, const wxSize& size, long style) : wxFrame((wxFrame *)NULL, id, title, pos, size, style)
{
    // für was wird dann die position übergeben?
    Center();
    
    // Objekte erzeugen
    WxList_benutzerliste = new wxListCtrl(this, ID_WxList_benutzerliste, wxPoint(442,3), wxSize(111,282), wxHSCROLL | wxLC_REPORT | wxLC_ALIGN_LEFT | wxLC_NO_HEADER);
    WxList_benutzerliste->InsertColumn(0, _T("Benutzerliste"), wxLIST_FORMAT_LEFT, -1);
    
    WxEdit_thema = new wxTextCtrl(this, ID_WxEdit_thema, _T(""), wxPoint(4,4), wxSize(434,20), wxTE_READONLY, wxDefaultValidator, _T("WxEdit_thema"));
    WxEdit_eingabefeld = new wxTextCtrl(this, ID_WxEdit_eingabefeld, _T("Nachricht eingeben und Senden"), wxPoint(4,289), wxSize(434,20), 0, wxDefaultValidator, _T("WxEdit_eingabefeld"));
    WxButton_senden = new wxButton(this, ID_WxButton_senden, _T("Senden"), wxPoint(442,289), wxSize(111,21), 0, wxDefaultValidator, _T("WxButton_senden"));
    WxEdit_ausgabefeld = new wxTextCtrl(this, ID_WxEdit_ausgabefeld, _T("Ausgabefeld"), wxPoint(4,26), wxSize(434,259), wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH, wxDefaultValidator, _T("WxEdit_ausgabefeld"));    
    
    
    SetIcon(wxIcon(icon));
    
    // OBJEKTE ANPASSEN, VERBINDUNG ZUR KONFIGURATION
    // IN EIGENE FUNKTION AUSLAGERN
    WxEdit_ausgabefeld->SetDefaultStyle(wxTextAttr(*wxBLACK)); // voreingestelltes Aussehen setzen ...
    defaultstyle = WxEdit_ausgabefeld->GetDefaultStyle(); // ... und speichern
    
    // Ereignisse verknuepfen
    
    // Eine gedrueckte Taste bei Fokus im Eingabefeld
    WxEdit_eingabefeld->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Fenster::WxEdit_eingabefeldTasteGedrueckt), NULL, this);

    // Bei Fokus ggf. den Text im Eingabefeld loeschen
    WxEdit_eingabefeld->Connect(wxEVT_SET_FOCUS, wxKeyEventHandler(Fenster::WxEdit_eingabefeldFokus), NULL, this);
}


// Ereignissabhaengige Funktionen

// Schliessen
void Fenster::OnClose(wxCloseEvent& WXUNUSED(event))
{
    //wxGetApp().fensterzerstoeren(fensternummer);
    wxGetApp().EingabeVerarbeiten(fensternummer,_T("/part"));
    // ueberpruefung ob nur noch ein fenster und ob schon im raum usw
}

void Fenster::BeiAktivierung(wxActivateEvent& event)
{
    // wenn das Fenster "aktiviert" wird
    // wird es als oberstes Fenster gesetzt
    // damit man mit GetTopWindow() abfragen kann welches das oberste ist
    if(event.GetActive())
    wxGetApp().SetTopWindow(dynamic_cast<wxWindow*>(event.GetEventObject()));
    event.Skip();
}

// Sendenknopf
void Fenster::WxButton_sendenClick(wxCommandEvent& event)
{
    NachrichtSenden();
}

// Tastendruck im Eingabefeld
void Fenster::WxEdit_eingabefeldTasteGedrueckt(wxKeyEvent& event)
{
    // Der Wert der gedrueckten Taste
    int gedrueckte_taste = event.GetKeyCode();

    if (gedrueckte_taste == WXK_RETURN)
    {
        NachrichtSenden();
    }

    else
    {
        // Ereignis weiterleiten
        event.Skip();
    }
    return;
}

// Bei Fokuserhalt Standardtext loeschen
void Fenster::WxEdit_eingabefeldFokus(wxKeyEvent& event)
{
    // VERBINDUNG ZUR KONFIGURATION
    if (WxEdit_eingabefeld->GetValue() == _T("Nachricht eingeben und Senden"))
    {
        WxEdit_eingabefeld->Clear();
    }
}

void Fenster::NachrichtSenden()
{
    // IN GESCHICHTE SPEICHERN
    
    wxString eingabe = WxEdit_eingabefeld->GetValue(); // Eingegebenen Text in Variable speichern
    WxEdit_eingabefeld->Clear(); // Eingabefeld leeren
    wxGetApp().EingabeVerarbeiten(fensternummer, eingabe); // an Zentrale uebergeben
}

// OEFFENTLICHE FUNKTIONEN
void Fenster::TitelSetzen(wxString titel, wxString nick, wxString hostname, wxString port)
{
    // VERBINDUNG ZUR KONFIGURATION
    if(nick = _T("")) 
        nick = wxGetApp().irc->CurrentNick;
    if(hostname = _T("")) 
        hostname = wxGetApp().irc->CurrentHostname;
    if(port = _T(""))
        port = wxGetApp().irc->CurrentPort;
        
    SetTitle(_T("efirc [" + nick + "@" + hostname + ":" + port + "/" + titel + "]"));
}

void Fenster::NachrichtAnhaengen(wxString local, wxString param1, wxString param2, wxString param3, wxString param4)
{
    //VERBINDUNG ZUR KONFIGURATION
    // Nachrichten an den eigenen Nickname in [  ] anzeigen, alles ausser das Prefix
    
    // Zeitstempel erzeugen
    char timestamp[12];
    time_t raw_time;
    tm *local_time;
    time(&raw_time);
    local_time = localtime(&raw_time);
    strftime(timestamp, 12, "[%H:%M:%S] ", local_time);
    wxString prefix(timestamp);
    
    // Bei jedem Aufruf einen Zeilenumbruch erzeugen und prefix voranstellen
    WxEdit_ausgabefeld->AppendText(_T("\n"+prefix+" "));

    // locale abfragen
    // MIT SWITCH / CASE 
    
    if(local == _T("P_PRIVMSG"))
    {
        if(param3 == "")
        {
            WxEdit_ausgabefeld->AppendText(_T("[ <" + param1 + "> " + param2 + " ]"));
        }
        else
        // wenn 3 Parameter uebergeben wurden, dann hat man selbst eine Nachricht an einen Benutzer geschrieben
        {
            WxEdit_ausgabefeld->AppendText(_T("[ <" + param1 + "->" + param2 + "> " + param3 + " ]"));
        }
    }
    
    else if(local == _T("PRIVMSG"))
    {
        if(param1 == "")
        {
            WxEdit_ausgabefeld->AppendText(_T(param2));
        }
        else
        {
            WxEdit_ausgabefeld->AppendText(_T("<" + param1 + "> " + param2));
        }
    }
    
    else if(local == _T("P_ACTION"))
    {
        WxEdit_ausgabefeld->AppendText(_T("[ "));
        WxEdit_ausgabefeld->SetDefaultStyle(wxTextAttr(wxNullColour, wxNullColour, *wxITALIC_FONT));
        WxEdit_ausgabefeld->AppendText(_T("*" + param1 + " " + param2));
        WxEdit_ausgabefeld->SetDefaultStyle(defaultstyle);
        WxEdit_ausgabefeld->AppendText(_T(" ]"));
    }
    
    else if(local == _T("ACTION"))
    {
        WxEdit_ausgabefeld->SetDefaultStyle(wxTextAttr(wxNullColour, wxNullColour, *wxITALIC_FONT));
        WxEdit_ausgabefeld->AppendText(_T("*" + param1 + " " + param2));
        WxEdit_ausgabefeld->SetDefaultStyle(defaultstyle);
    }
        
    else if(local == _T("TOPIC"))
    {
        if(param2 == "")
        {
            WxEdit_ausgabefeld->AppendText(_T("Das Thema ist:" + param1));
        }
        else
        {
            WxEdit_ausgabefeld->AppendText(_T("*" + param2 + " hat das Thema geaendert: " + param1));
        }
    }
    
    else if(local == _T("JOIN"))
        WxEdit_ausgabefeld->AppendText(_T(param1 + " hat den Raum betreten"));
        
    else if(local == _T("PART"))
        WxEdit_ausgabefeld->AppendText(_T(param1 + " hat den Raum verlassen (" + param2 + ")"));
    
    else if(local == _T("QUIT"))
        WxEdit_ausgabefeld->AppendText(_T(param1 + " hat das IRC-Netzwerk verlassen (" + param2 + ")"));
        
    else if(local == _T("MOTD"))
        WxEdit_ausgabefeld->AppendText(_T(param1));
    
    else if(local == _T("CTCP"))
    {
         if(param3 == _T(""))
        {
            WxEdit_ausgabefeld->AppendText(_T("[ <" + param1 + "@[CTCP]> " + param2 + " ]"));
        }
        else
        // wenn 3 Parameter uebergeben wurden, dann hat man selbst eine Nachricht an einen Benutzer geschrieben
        {
            WxEdit_ausgabefeld->AppendText(_T("[ <" + param1 + "@[CTCP]->" + param2 + "> " + param3 + " ]"));
        }
    }
       
    else if(local == _T("MODE"))
    {
        WxEdit_ausgabefeld->AppendText(_T(param1 + " setzt Modus: " + param2));
    }
          
    
    else if(local == _T("AWAY"))
    {
        // ES WIRD EINE LEERE ZEILE AUSGEGEBEN WENN MAN DEN STATUS WIEDER AUF VERFUEGBAR SETZT
        if(param1 != _T(""))
            WxEdit_ausgabefeld->AppendText(_T("Sie sind jetzt abwesend: " + param1));
    }
    
    else if(local == _T("RPL_UNAWAY"))
        WxEdit_ausgabefeld->AppendText(_T("Sie sind jetzt nicht mehr abwesend"));

    else if(local == _T("RPL_NOWAWAY"))
        WxEdit_ausgabefeld->AppendText(_T("Sie sind jetzt als abwesend markiert"));
    
    // Whois Antworten
    else if(local == _T("WHOIS_BENUTZER"))
        WxEdit_ausgabefeld->AppendText(_T("[ WHOIS: " + param1 + " (" + param2 + "@" + param3 + " - " + param4 + ") ]"));
    
    else if(local == _T("WHOIS_ABWESEND"))
        WxEdit_ausgabefeld->AppendText(_T("[ WHOIS: " + param1 + " ist abwesend: " + param2 + " ]"));
        
    else if(local == _T("WHOIS_RAEUME"))
        WxEdit_ausgabefeld->AppendText(_T("[ WHOIS: " + param1 + " ist in: " + param2 + " ]"));
        
    else if(local == _T("WHOIS_UNTAETIG"))
        WxEdit_ausgabefeld->AppendText(_T("[ WHOIS: " + param1 + " ist untaetig seit: " + param2 + " ]"));
        
    else if(local == _T("WHOIS_SERVERNACHRICHT"))
        WxEdit_ausgabefeld->AppendText(_T("[ WHOIS: " + param1 + " " + param2 + " " + param3 + " ]"));
        
}

void Fenster::Fehler(int fehlernummer, wxString param1)
{
    
    //VERBINDUNG ZUR KONFIGURATION
    
    // Zeitstempel erzeugen
    char timestamp[12];
    time_t raw_time;
    tm *local_time;
    time(&raw_time);
    local_time = localtime(&raw_time);
    strftime(timestamp, 12, "[%H:%M:%S] ", local_time);
    wxString prefix(timestamp);
    
    // Aussehen aendern (Farbe)
    WxEdit_ausgabefeld->SetDefaultStyle(wxTextAttr(*wxRED, *wxBLUE));
    
        // Bei jedem Aufruf einen Zeilenumbruch erzeugen und prefix voranstellen    
        WxEdit_ausgabefeld->AppendText(_T("\n"+prefix+" (!) "));
        
        wxString fehlernummer_str;
        fehlernummer_str << fehlernummer;
        param1 = _T("(" + fehlernummer_str + ") (" + param1 + ")");
        // Fehlernummern abfragen
        if(fehlernummer == 1)
            WxEdit_ausgabefeld->AppendText(_T("Es konnte kein weiteres Fenster erstellt werden" + param1));
        if(fehlernummer == 2)
            WxEdit_ausgabefeld->AppendText(_T("IRC Fehler aufgetreten " + param1));
        if(fehlernummer == 3)
            WxEdit_ausgabefeld->AppendText(_T("Fenster wurde nicht gefunden " + param1));
        
    // Voreingestelltes Aussehen wiederherstellen (Farbe wieder entfernen)
    WxEdit_ausgabefeld->SetDefaultStyle(defaultstyle);
}

void Fenster::ThemaAendern(wxString thema, wxString benutzer)
{
    WxEdit_thema->SetValue(thema);
    NachrichtAnhaengen(_T("TOPIC"),thema,benutzer);
}

void Fenster::AusgabefeldLeeren()
{
    WxEdit_ausgabefeld->Clear();
}

// --------------------------------
// Benutzerlistenfunktionen
// --------------------------------

// Benutzer sortiert eintragen
void Fenster::BenutzerHinzufuegen(wxString benutzerliste)
{
    wxString Benutzer = _T(""); 
    wxString Benutzer_Vergleich = _T("");
    wxChar leerzeichen = _T(' ');
    
    while(benutzerliste != _T(""))
    // solange noch Benutzer in der benutzerliste stehen wird diese Schleife durchlaufen
    {
        Benutzer = benutzerliste.BeforeFirst(leerzeichen);
        // wenn der Benutzer noch nicht vorhanden ist
        if(WxList_benutzerliste->FindItem(-1,_T(Benutzer)) == -1)
        {
            long id = 0;
            bool erstellt = false;
            
            // Stelle in der Liste finden an dem der Eintrag sortiert eingetragen werden muss
            if(WxList_benutzerliste->GetItemCount() == 0)
            // Wenn noch kein Benutzer in der Liste ist braucht man nicht sortieren
            {
                WxList_benutzerliste->InsertItem(id,Benutzer);
            }
            else
            {
                while(id<WxList_benutzerliste->GetItemCount() && erstellt == false )
                // Solange man noch nicht am Ende der Liste ist und der Benutzer noch nicht hinzugefuegt wurde
                // --- ja das hier rockt oder? Viel besser als diese SetItemData mit long und dem Kack ;) ---
                {
                    Benutzer_Vergleich = WxList_benutzerliste->GetItemText(id); // Benutzer an aktueller Position zum Vergleichen auslesen
                    if(Benutzer.CmpNoCase(Benutzer_Vergleich) <1)
                    // Wenn der neue Benutzer "kleiner" ist, wird er an der aktuellen Position eingetragen, dabei werden die anderen Eintraege nach unten geschoben
                    {
                        WxList_benutzerliste->InsertItem(id,Benutzer);
                        erstellt = true; // Schleife verlassen
                    }
                    else
                    // Andernfalls wird mit dem naechsten Benutzer verglichen ...
                    {
                        id++;
                        if(id == WxList_benutzerliste->GetItemCount())
                        // oder falls schon der letzte Eintrag erreicht wurde Benuzter hinzufuegen
                        {
                            WxList_benutzerliste->InsertItem(id,Benutzer);
                            erstellt = true; // Schleife verlassen
                        }
                    }
                }
            }
        }
        //Benutzer und Leerzeichen aus benutzerliste entfernen
        benutzerliste = benutzerliste.AfterFirst(leerzeichen);
    }
}

bool Fenster::BenutzerEntfernen(wxString benutzer)
{
    wxString Benutzer_prefix_liste[3]={_T(""),_T("@"),_T("+")};
    long id = 0;
    bool entfernt = false;
    
    for(int i = 0;i<3;i++)
    {
        id = WxList_benutzerliste->FindItem(-1,_T(Benutzer_prefix_liste[i] + benutzer));
        if(id != -1)
        {
            entfernt = WxList_benutzerliste->DeleteItem(id);
        }
    }   
    return entfernt;
}

void Fenster::BenutzerAendern(wxString altername, wxString neuername)
{  
    // PREFIXE VOM SERVER HIER EINFUEGEN
    wxString Benutzer_prefix_liste[3]={_T(""),_T("@"),_T("+")};
    long id = 0;
    
    for(int i = 0;i<3;i++)
    {
        id = WxList_benutzerliste->FindItem(-1,_T(Benutzer_prefix_liste[i] + altername));
        if(id != -1)
        {
            WxList_benutzerliste->DeleteItem(id);
            BenutzerHinzufuegen(_T(Benutzer_prefix_liste[i] + neuername));
        }
    }
}


