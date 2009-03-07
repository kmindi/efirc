// ui.cc
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
    // f�r was wird dann die position �bergeben?
    Center();

    // Objekte erzeugen
    WxList_benutzerliste = new wxListCtrl(this, ID_WxList_benutzerliste, wxPoint(608,4), wxSize(111,384), wxHSCROLL | wxLC_REPORT | wxLC_ALIGN_LEFT | wxLC_NO_HEADER);
    WxList_benutzerliste->InsertColumn(0, _T("Benutzerliste"), wxLIST_FORMAT_LEFT, -1);

    WxEdit_thema = new wxTextCtrl(this, ID_WxEdit_thema, _T(""), wxPoint(4,4), wxSize(600,20), wxTE_READONLY, wxDefaultValidator, _T("WxEdit_thema"));
    WxEdit_eingabefeld = new wxTextCtrl(this, ID_WxEdit_eingabefeld, _T("Nachricht eingeben und Senden"), wxPoint(4,392), wxSize(600,20), 0, wxDefaultValidator, _T("WxEdit_eingabefeld"));
    WxButton_senden = new wxButton(this, ID_WxButton_senden, _T("Senden"), wxPoint(608,392), wxSize(111,20), 0, wxDefaultValidator, _T("WxButton_senden"));
    WxEdit_ausgabefeld = new wxTextCtrl(this, ID_WxEdit_ausgabefeld, _T(""), wxPoint(4,28), wxSize(600,360), wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH, wxDefaultValidator, _T("WxEdit_ausgabefeld"));


    SetIcon(wxIcon(icon));

    // OBJEKTE ANPASSEN, VERBINDUNG ZUR KONFIGURATION
    // IN EIGENE FUNKTION AUSLAGERN
    ObjekteAnpassen();

    // Ereignisse verknuepfen

    // Eine gedrueckte Taste bei Fokus im Eingabefeld
    WxEdit_eingabefeld->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(Fenster::WxEdit_eingabefeldTasteGedrueckt), NULL, this);

    // Bei Fokus ggf. den Text im Eingabefeld loeschen
    WxEdit_eingabefeld->Connect(wxEVT_SET_FOCUS, wxKeyEventHandler(Fenster::WxEdit_eingabefeldFokus), NULL, this);



    // Groessenaenderung beachten
        // Sizer erzeugen:
        wxBoxSizer *sizer_links = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer *sizer_rechts = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer *sizer_alles = new wxBoxSizer(wxHORIZONTAL);

        // Vertikale Groessenaenderung: Thema, Ausgabefeld und Eingabefeld
            sizer_links->Add(WxEdit_thema, 0 , wxEXPAND | wxBOTTOM, 3); // Thema behaelt seine Groesse bei vertikaler Groessenaenderung
            sizer_links->Add(WxEdit_ausgabefeld, 1, wxEXPAND | wxBOTTOM, 3); // Ausgabefeld wird hoeher bei vertikaler Groessenaenderung
            sizer_links->Add(WxEdit_eingabefeld, 0, wxEXPAND); // Eingabefeld behaelt seine Groesse bei vertikaler Groessenaenderung
            // Bei Horizontaler Groessenaenderung werden alle Objekte groesser (wxEXPAND)

        // Vertikale Groessenaenderung: Benutzerliste, Sendenknopf
            sizer_rechts->Add(WxList_benutzerliste, 1, wxEXPAND | wxBOTTOM, 3); // Benutzerliste wird hoeher bei vertikaler Groessenaenderung
            sizer_rechts->Add(WxButton_senden, 0, wxEXPAND ); // Sendenknopf behaelt seine Groesse bei vertikaler Groessenaenderung
            // Bei Horizontaler Groessenaenderung werden alle Objekte groesser (wxEXPAND)

        // Horizontal beide zusammen
            sizer_alles->Add(sizer_links, 1, wxEXPAND | wxALL, 3); // Linker Teil wird breiter bei horizontaler Groessenaenderung
            sizer_alles->Add(sizer_rechts, 0, wxEXPAND | wxTOP | wxRIGHT | wxBOTTOM, 3); // Rechter Teil behaelt seine Groesse


        sizer_alles->SetSizeHints( this ); // Minimale Groesse des Fensters (this) beachten ( = nicht unterschreiten)
        SetSizer(sizer_alles); // sizer_alles als Sizer fuer dieses Fenster verwenden
}

// Farben aus der Konfiguration lesen und bei allen Objekten anpassen
void Fenster::ObjekteAnpassen()
{
    SetBackgroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_background"))));

    WxEdit_thema->SetBackgroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_topic_background"))));
    WxEdit_thema->SetForegroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_topic_foreground"))));

    WxEdit_eingabefeld->SetBackgroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_input_messages_background"))));
    WxEdit_eingabefeld->SetForegroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_input_messages_foreground"))));

    WxList_benutzerliste->SetBackgroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_channel_users_background"))));
    WxList_benutzerliste->SetForegroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_channel_users_foreground"))));

    WxButton_senden->SetBackgroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_button_background"))));
    WxButton_senden->SetForegroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_button_foreground"))));

    WxEdit_thema->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxNORMAL,wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_topic"))));
    WxEdit_eingabefeld->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxNORMAL,wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_input_messages"))));
    WxList_benutzerliste->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxNORMAL, wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_channel_users"))));
    WxButton_senden->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxNORMAL,wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_button"))));

    // Ausgabefeld wird extra behandelt, weil die Fehler-Funktion Farbe verwendet und danach den Default Style wieder herstellen muss
    WxEdit_ausgabefeld->SetDefaultStyle
    (
        wxTextAttr
        (
            wxColour(wxGetApp().config->parsecfgvalue(_T("colour_output_messages_foreground"))),
            wxColour(wxGetApp().config->parsecfgvalue(_T("colour_output_messages_background"))),
            wxFont(8, wxFONTFAMILY_MODERN, wxNORMAL, wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_output_messages")))
        )
    );
    // Hintergrund an dem kein Text steht muss seperat eingestellt werden
    WxEdit_ausgabefeld->SetBackgroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_output_messages_background"))));
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
    if(nick = _T(""))
        nick = wxGetApp().irc->CurrentNick;
    if(hostname = _T(""))
        hostname = wxGetApp().irc->CurrentHostname;
    if(port = _T(""))
        port = wxGetApp().irc->CurrentPort;

    // Aus Konfiguration Titelformat auslesen
    wxString titel_text = wxGetApp().config->parsecfgvalue(_T("text_title"));

    // Parameter ersetzen
    titel_text.Replace(_T("%param1"),nick);
    titel_text.Replace(_T("%param2"),hostname);
    titel_text.Replace(_T("%param3"),port);
    titel_text.Replace(_T("%param4"),titel);

    SetTitle(titel_text + _T("efirc ") + wxGetApp().efirc_version_string);
}

void Fenster::NachrichtAnhaengen(wxString local, wxString param1, wxString param2, wxString param3, wxString param4)
{
    // Zeitstempel erzeugen
    char timestamp[12];
    time_t raw_time;
    tm *local_time;
    time(&raw_time);
    local_time = localtime(&raw_time);
    strftime(timestamp, 12, "[%H:%M:%S]", local_time);
    wxString prefix(timestamp, wxConvUTF8);

    // Bei jedem Aufruf einen Zeilenumbruch erzeugen und prefix voranstellen
    prefix = _T("\n") + prefix + _T(" ");

    // Aus Konfiguration Nachrichtformat auslesen
    wxString nachricht = wxGetApp().config->parsecfgvalue(_T("local_") + local);

    // Parameter in der Nachricht mit den uebergebenen Parametern ersetzen
    nachricht.Replace(_T("%param1"),param1);
    nachricht.Replace(_T("%param2"),param2);
    nachricht.Replace(_T("%param3"),param3);
    nachricht.Replace(_T("%param4"),param4);

    nachricht.Trim(false);
    nachricht.Trim();

    // Besondere Textformatierungen beachten und anwenden
        // Wenn [italic] gefunden wurde:
        if(nachricht.Contains(_T("[italic]")) && nachricht.Contains(_T("[/italic]")))
        {
            int pos = nachricht.Find(_T("[italic]"));
            //alles vor italic ausgeben
                WxEdit_ausgabefeld->AppendText(prefix + nachricht.Left(pos));
            //alles italic formatiert ausgeben
                wxTextAttr defaultstyle = WxEdit_ausgabefeld->GetDefaultStyle();
                WxEdit_ausgabefeld->SetDefaultStyle(wxTextAttr(wxNullColour, wxNullColour, *wxITALIC_FONT));

                WxEdit_ausgabefeld->AppendText(nachricht.Mid(pos + 8, nachricht.Find(_T("[/italic]")) - pos - 8));

                WxEdit_ausgabefeld->SetDefaultStyle(defaultstyle);
            //alles nach italic ausgeben
                WxEdit_ausgabefeld->AppendText(nachricht.Mid(nachricht.Find(_T("[/italic]")) + 9));
            // Nachricht nicht nochmal ausgeben
                return;
        }

    // Nachricht nur ausgeben wenn sie nicht leer ist
    if(nachricht != _T(""))
    WxEdit_ausgabefeld->AppendText(prefix + nachricht);
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
    strftime(timestamp, 12, "[%H:%M:%S]", local_time);
    wxString prefix(timestamp, wxConvUTF8);

    // Aussehen aendern
    // zuvor gesetzten style speichern...
    wxTextAttr defaultstyle = WxEdit_ausgabefeld->GetDefaultStyle();
    WxEdit_ausgabefeld->SetDefaultStyle(wxTextAttr(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_error")))));

        // Bei jedem Aufruf einen Zeilenumbruch erzeugen und prefix voranstellen
        WxEdit_ausgabefeld->AppendText(_T("\n")+prefix+_T(" (!) "));

        wxString fehlernummer_str =_T("");
        fehlernummer_str << fehlernummer;
        param1 = _T("(") + fehlernummer_str + _T(") (") + param1 + _T(")");
        // Fehlernummern abfragen
        switch(fehlernummer)
        {
            case 1:
                WxEdit_ausgabefeld->AppendText(_T("Es konnte kein weiteres Fenster erstellt werden") + param1);
                break;
            case 2:
                WxEdit_ausgabefeld->AppendText(_T("IRC Fehler aufgetreten ") + param1);
                break;
            case 3:
                WxEdit_ausgabefeld->AppendText(_T("Fenster wurde nicht gefunden ") + param1);
                break;
            case 4:
                WxEdit_ausgabefeld->AppendText(_T("Befehl nicht implementiert oder zu wenig Parameter angegeben: ") + param1);
                break;
            case 5:
                WxEdit_ausgabefeld->AppendText(_T("IRC Befehl nicht implementiert: ") + param1);
                break;
            default:
                WxEdit_ausgabefeld->AppendText(_T("Nicht definierter Fehler aufgetreten"));
                break;
        }


    // ...voreingestelltes Aussehen wiederherstellen
    WxEdit_ausgabefeld->SetDefaultStyle(defaultstyle);
}

void Fenster::ThemaAendern(wxString thema, wxString benutzer)
{
    WxEdit_thema->SetValue(thema);
    if(benutzer == _T(""))
    {
        NachrichtAnhaengen(_T("TOPIC"),thema,benutzer);
    }
    else
    {
        NachrichtAnhaengen(_T("TOPIC_CHANGE"),thema,benutzer);
    }

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
        if(WxList_benutzerliste->FindItem(-1,Benutzer) == -1)
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
        id = WxList_benutzerliste->FindItem(-1,Benutzer_prefix_liste[i] + benutzer);
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
        id = WxList_benutzerliste->FindItem(-1,Benutzer_prefix_liste[i] + altername);
        if(id != -1)
        {
            WxList_benutzerliste->DeleteItem(id);
            BenutzerHinzufuegen(Benutzer_prefix_liste[i] + neuername);
        }
    }
}


