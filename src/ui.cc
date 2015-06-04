/**
** efirc steht unter der MIT-Lizenz.
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the MIT License.
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// ui.cc
// Implementiert die Fensterklasse

#include <ui.h>
#include <zentrale.h>
DECLARE_APP(Zentrale) // braucht man fuer wxGetApp() um damit auf die funktionen der Zentrale zuzugreifen

// Ereignisse definieren
BEGIN_EVENT_TABLE(Fenster, wxPanel)
    EVT_BUTTON     (ID_WxButton_senden,
                    Fenster::WxButton_sendenClick)
    //EVT_TEXT_URL (wxID_ANY, Fenster::BeiMausAufURL)
END_EVENT_TABLE()

//Konstruktor der Fenster-Klasse
Fenster::Fenster(const wxString& title, const wxPoint& pos, const wxSize& size) : wxPanel(wxGetApp().notebook, wxID_ANY, pos, size, 0)
{
    // Objekte erzeugen
    WxList_benutzerliste = new wxListCtrl(this, ID_WxList_benutzerliste, wxPoint(606,3), wxSize(111,383), wxHSCROLL | wxLC_REPORT | wxLC_ALIGN_LEFT | wxLC_NO_HEADER);
    WxList_benutzerliste->InsertColumn(0, _T("Benutzerliste"), wxLIST_FORMAT_LEFT, -1);

    WxEdit_thema = new wxTextCtrl(this, ID_WxEdit_thema, _T(""), wxPoint(3,3), wxSize(600,20), wxTE_READONLY | wxTE_RICH, wxDefaultValidator, _T("WxEdit_thema"));
    WxEdit_eingabefeld = new wxTextCtrl(this, ID_WxEdit_eingabefeld, wxGetApp().config->parsecfgvalue(_T("local_label_input")), wxPoint(3,389), wxSize(600,20), 0, wxDefaultValidator, _T("WxEdit_eingabefeld"));
    WxButton_senden = new wxButton(this, ID_WxButton_senden, wxGetApp().config->parsecfgvalue(_T("local_label_button")), wxPoint(606,389), wxSize(111,20), 0, wxDefaultValidator, _T("WxButton_senden"));
    WxEdit_ausgabefeld = new wxTextCtrl(this, ID_WxEdit_ausgabefeld, _T(""), wxPoint(3,26), wxSize(600,360), wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH, wxDefaultValidator, _T("WxEdit_ausgabefeld"));

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

    geschichte_position = 0;
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

    WxEdit_eingabefeld->SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxNORMAL,wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_input_messages"))));
    WxList_benutzerliste->SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_channel_users"))));
    WxButton_senden->SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxNORMAL,wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_button"))));

    // Ausgabe- und Themafeld wird extra behandelt, weil die Fehler-Funktion Farbe verwendet und danach den Default Style wieder herstellen muss
    WxEdit_thema->SetDefaultStyle
    (
        wxTextAttr
        (
            wxColour(wxGetApp().config->parsecfgvalue(_T("colour_topic_foreground"))),
            wxColour(wxGetApp().config->parsecfgvalue(_T("colour_topic_background"))),
            wxFont(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_topic")))
        )
    );
    
    WxEdit_ausgabefeld->SetDefaultStyle
    (
        wxTextAttr
        (
            wxColour(wxGetApp().config->parsecfgvalue(_T("colour_output_messages_foreground"))),
            wxColour(wxGetApp().config->parsecfgvalue(_T("colour_output_messages_background"))),
            wxFont(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL, FALSE, wxGetApp().config->parsecfgvalue(_T("font_output_messages")))
        )
    );
    // Hintergrund an dem kein Text steht muss seperat eingestellt werden
    WxEdit_ausgabefeld->SetBackgroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_output_messages_background"))));
}

// Ereignissabhaengige Funktionen

// Sendenknopf
void Fenster::WxButton_sendenClick(wxCommandEvent&)
{
    NachrichtSenden();
}

// Tastendruck im Eingabefeld
void Fenster::WxEdit_eingabefeldTasteGedrueckt(wxKeyEvent& event)
{
    // Der Wert der gedrueckten Taste
    int gedrueckte_taste = event.GetKeyCode();

    switch(gedrueckte_taste)
    {
        case WXK_RETURN:
            NachrichtSenden();
            break;

        case WXK_UP:
            if(geschichte_position > 0)
            {
                --geschichte_position;
                WxEdit_eingabefeld->ChangeValue(_T(""));
                WxEdit_eingabefeld->AppendText(geschichte_texte[geschichte_position]);
            }
            break;

        case WXK_DOWN:
            if(geschichte_texte.GetCount() != 0)
            {
                if(geschichte_position < geschichte_texte.GetCount() - 1)
                {
                    ++geschichte_position;
                    WxEdit_eingabefeld->ChangeValue(_T(""));
                    WxEdit_eingabefeld->AppendText(geschichte_texte[geschichte_position]);
                }
                else
                {
                    geschichte_position = geschichte_texte.GetCount();
                    WxEdit_eingabefeld->ChangeValue(_T(""));
                    WxEdit_eingabefeld->SetInsertionPointEnd();
                }
            }
            break;

        default:
            event.Skip();
    }
}

// Bei Fokuserhalt Standardtext loeschen
void Fenster::WxEdit_eingabefeldFokus(wxKeyEvent& event)
{
    if (WxEdit_eingabefeld->GetValue() == wxGetApp().config->parsecfgvalue(_T("local_label_input")))
    {
        WxEdit_eingabefeld->Clear();
    }
    
    event.Skip();
}

// Mausklick auf hervorgehobene URL
//void Fenster::BeiMausAufURL(wxTextUrlEvent& event)
//{
//    const wxMouseEvent& mausereignis = event.GetMouseEvent();
//    if(mausereignis.LeftDown())
//    {
//        wxString adresse = WxEdit_ausgabefeld->GetRange(event.GetURLStart(), event.GetURLEnd());
//        wxLaunchDefaultBrowser(adresse);
//    }
//    event.Skip();
//}


void Fenster::NachrichtSenden()
{
    wxString eingabe = WxEdit_eingabefeld->GetValue(); // Eingegebenen Text in Variable speichern
    if(eingabe != _T(""))
    {
        WxEdit_eingabefeld->Clear(); // Eingabefeld leeren
        wxGetApp().EingabeVerarbeiten(fenster_name, eingabe); // an Zentrale uebergeben
        geschichte_texte.Add(eingabe); // In Eingabegeschichte speichern
    }

    geschichte_position = geschichte_texte.GetCount();

    if(geschichte_position > 30)
    {
        geschichte_texte.RemoveAt(0);
        --geschichte_position;
    }
}

// OEFFENTLICHE FUNKTIONEN
void Fenster::TitelSetzen(wxString titel)
{
    if(titel == _T("")) titel = fenster_name;
    wxGetApp().notebook->SetPageText(wxGetApp().notebook->GetPageIndex(this), titel);
}

void Fenster::NachrichtAnhaengen(wxString local, wxString param1, wxString param2, wxString param3, wxString param4)
{
    // Zeitstempel erzeugen
    wxDateTime zeit = wxDateTime::Now();

    // Bei jedem Aufruf einen Zeilenumbruch erzeugen und prefix voranstellen
    wxString prefix = _T("\n") + zeit.Format("[%H:%M:%S]") + _T(" ");

    // Aus Konfiguration Nachrichtformat auslesen
    wxString nachricht = wxGetApp().config->parsecfgvalue(_T("local_") + local);

    // Parameter in der Nachricht mit den uebergebenen Parametern ersetzen
    nachricht.Replace(_T("%param1"),param1);
    nachricht.Replace(_T("%param2"),param2);
    nachricht.Replace(_T("%param3"),param3);
    nachricht.Replace(_T("%param4"),param4);

    // Nachricht nicht anzeigen wenn es eine Benutzerlistenaenderung ist und die Begrenzung erreicht ist
    if(local.Left(3) != _T("BL_") || (local.Left(3) == _T("BL_") && !AnzeigeBegrenzungErreicht()))
    {
        wxString text = prefix + nachricht;
        FormatiertAnzeigen(WxEdit_ausgabefeld, text);
    }
    
    // Wenn eine Nachricht in einem nicht aktiven Fenster angezeigt wird, dieses blinken lassen, ausser es ist eine Benutzerlistenaenderung
    //if(this != wxGetActiveWindow() && this != wxGetApp().GetTopWindow() && local.Left(3) != _T("BL_"))
    //this->RequestUserAttention();
}

void Fenster::ThemaAendern(wxString thema, wxString benutzer)
{
    WxEdit_thema->Clear();
    FormatiertAnzeigen(WxEdit_thema, thema);
    
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
            // Stelle in der Liste finden an dem der Eintrag sortiert eingetragen werden muss
            if(WxList_benutzerliste->GetItemCount() == 0)
            // Wenn noch kein Benutzer in der Liste ist braucht man nicht sortieren
            {
                WxList_benutzerliste->InsertItem(0, Benutzer);
            }
            else
            {
                // binaere Suche verwenden
                long L = 0; // Linke Grenze
                long R = WxList_benutzerliste->GetItemCount() - 1; // Rechte Grenze
                long M = (R + L) / 2; // Mitte
                
                while(L <= R )
                {
                    // M jedes mal neuberechnen, weil sich die Grenezen geaendert haben
                    M = (R + L) / 2;
                    // Benutzer in der Mitte mit dem verglichen wird
                    Benutzer_Vergleich = WxList_benutzerliste->GetItemText(M);
                    // Gleichen Benutzer kann es nicht geben nach Vorrausseztung der anderen if Abfrage
                    // Wenn der neue Benutzer "kleiner" ist
                    if(Benutzer.CmpNoCase(Benutzer_Vergleich) < 1)
                    {
                        R = M - 1;
                    }
                    else
                    {
                        L = M + 1;
                    }
                    
                    // Folgender Fall tritt auf jeden Fall ein, L > R = R < L
                    if (L > R)
                    {
                        WxList_benutzerliste->InsertItem(L, Benutzer);
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
    wxString Benutzer_prefix_liste[]={_T(""), _T("@"), _T("+"), _T("@+"), _T("+@")};
    long id = 0;
    bool entfernt = false;

    for(int i = 0;i<5;i++)
    {
        id = WxList_benutzerliste->FindItem(-1,Benutzer_prefix_liste[i] + benutzer);
        if(id != -1)
        {
            entfernt = WxList_benutzerliste->DeleteItem(id);
        }
    }
    return entfernt;
}

bool Fenster::BenutzerAendern(wxString altername, wxString neuername)
{
    // PREFIXE VOM SERVER HIER EINFUEGEN
    wxString Benutzer_prefix_liste[]={_T(""), _T("@"), _T("+"), _T("@+"), _T("+@")};
    long id = 0;

    for(int i = 0;i<5;i++)
    {
        id = WxList_benutzerliste->FindItem(-1,Benutzer_prefix_liste[i] + altername);
        if(id != -1)
        {
            BenutzerEntfernen(altername);
            BenutzerHinzufuegen(Benutzer_prefix_liste[i] + neuername);
            return true;
        }
    }
    return false;
}

wxString Fenster::HoleBenutzerModi(wxString benutzer)
{
    wxString Benutzer_prefix_liste[]={_T(""), _T("@"), _T("+"), _T("@+"), _T("+@")};
    for(int i = 0;i<5;i++)
    {
        if(WxList_benutzerliste->FindItem(-1, Benutzer_prefix_liste[i] + benutzer) != -1)
        {
            return Benutzer_prefix_liste[i];
        }
    }
    return _T("");
}


void Fenster::BenutzerModiHinzufuegen(wxString benutzer, wxString modi)
{
    wxString altemodi = HoleBenutzerModi(benutzer);
    if(!altemodi.Contains(modi))
    {
        long id = WxList_benutzerliste->FindItem(-1, altemodi + benutzer);
        if(id != -1)
        {
            BenutzerEntfernen(benutzer);
            BenutzerHinzufuegen(altemodi + modi + benutzer);
        }
    }
}

void Fenster::BenutzerModiEntfernen(wxString benutzer, wxString modi)
{
    wxString altemodi = HoleBenutzerModi(benutzer);
    long id = WxList_benutzerliste->FindItem(-1, altemodi + benutzer);
    if(id != -1)
    {
        altemodi.Replace(modi, _T(""));
        BenutzerEntfernen(benutzer);
        BenutzerHinzufuegen(altemodi + benutzer);
    }
}

bool Fenster::AnzeigeBegrenzungErreicht()
{
    long configwert = 0;
    (wxGetApp().config->parsecfgvalue(_T("max_DONT_SHOW_USERLIST_CHANGES"))).ToLong(&configwert, 10);

    if(WxList_benutzerliste->GetItemCount() >= configwert && configwert != 0) return true;
    return false;
}


void Fenster::FormatiertAnzeigen(wxTextCtrl *textfeld, wxString text)
{
    wxTextAttr defaultstyle = textfeld->GetDefaultStyle(); // Voreingestellte Formatierungen speichern um sie nachher wiederherstellen zu koennen
    wxTextAttr textattr = defaultstyle; // Voreingestellte Formatierung als Ausgangswerte benutzen
    bool fett, unterstrichen, kursiv = false;
    
    unsigned long pos = 0;
    // Jedes Zeichen der Nachricht auf Steuerzeichen ueberpruefen
    while(pos <= text.Len() && text != _T(""))
    {
        textattr = textfeld->GetDefaultStyle();
        
        switch(text.GetChar(pos).GetValue())
        {
            // Text bis zum Formatierungszeichen ausgeben
            // Den Ausgegebenen Text inklusive dem aktuellen Steuerzeichen entfernen (++pos)
            // und pos auf 0 setzen.
            
            case 0x0002: // CTRL B (STX) Fett
                
                textfeld->AppendText(text.Left(pos));
                text = text.Mid(++pos);
                pos = 0;
                
                if(!fett)
                {
                    textattr.SetFontWeight(wxFONTWEIGHT_BOLD); 
                    textfeld->SetDefaultStyle(textattr); // Fett setzen
                    fett = true;
                }
                else
                {
                    textattr.SetFontWeight(wxFONTWEIGHT_NORMAL); 
                    textfeld->SetDefaultStyle(textattr); // Fett setzen
                    fett = false;
                }
                
                break;
                
            case 0x0003: // CTRL C (ETX) Farbig
                {
                    wxString farbangaben = _T("");
                    wxRegEx mitkomma(_T("[\x0003][0-9]{1,2}[,]{1,1}[0-9]{1,2}"));
                    wxRegEx ohnekomma(_T("[\x0003][0-9]{1,2}"));
                    
                    textfeld->AppendText(text.Left(pos));
                    text = text.Mid(pos); // inklusive Steuerzeichen (CTRL+C)
                    pos = 0;
                    
                    // Wenn CTRL+C[n,nn],[m,mm] gefunden wurde
                    if(mitkomma.Matches(text))
                    {
                        long unsigned int n,m = 0;
                        farbangaben = mitkomma.GetMatch(text);
                        farbangaben = farbangaben.Mid(1); // Steuerzeichen entfernen
                        farbangaben.BeforeFirst(_T(',')).ToULong(&n);
                        farbangaben.AfterFirst(_T(',')).ToULong(&m);
                        mitkomma.Replace(&text, "",1); // Farbangaben entfernen
                        
                        textattr.SetTextColour(wxColour(IRC_Farben[n%15]));
                        textattr.SetBackgroundColour(wxColour(IRC_Farben[m%15]));
                        textfeld->SetDefaultStyle(textattr);
                    }
                    
                    // Wenn CTRL+C[n,nn] gefunden wurde
                    else if(ohnekomma.Matches(text))
                    {
                        long unsigned int n = 0;
                        farbangaben = ohnekomma.GetMatch(text);
                        farbangaben = farbangaben.Mid(1); // Steuerzeichen entfernen
                        farbangaben.ToULong(&n);
                        ohnekomma.Replace(&text, "",1); // Farbangaben entfernen
                        
                        textattr.SetTextColour(wxColour(IRC_Farben[n%15]));
                        textfeld->SetDefaultStyle(textattr);
                    }
                    
                    else // Wenn keine Nummer hinter dem Steuerzeichen steht soll die Farbe wieder aufgehoben werden
                    {
                        textattr.SetTextColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_output_messages_foreground"))));
                        textattr.SetBackgroundColour(wxColour(wxGetApp().config->parsecfgvalue(_T("colour_output_messages_background"))));
                        textfeld->SetDefaultStyle(textattr);
                        
                        text.Replace(_T("\x0003"), _T("")); // Steuerzeichen entfernen, pos nicht erhoehen
                    }
                }
                break;
                
            case 0x001F: // CTRL U? (US) Unterstrichen
                textfeld->AppendText(text.Left(pos));
                text = text.Mid(++pos);
                pos = 0;
                
                if(!unterstrichen) unterstrichen = true;
                else unterstrichen = false;
                
                textattr.SetFontUnderlined(unterstrichen); 
                textfeld->SetDefaultStyle(textattr); // Fett setzen
                
                
                break;
                
            case 0x0012: // CTRL R Umgetauscht
                textfeld->AppendText(text.Left(pos));
                text = text.Mid(++pos);
                pos = 0;
                
                // NOCH EINBAUEN
                
                break;
                
            case 0x0009: // CTRL I Kursiv
                textfeld->AppendText(text.Left(pos));
                text = text.Mid(++pos);
                pos = 0;
                
                if(!kursiv)
                {
                    textattr.SetFontStyle(wxFONTSTYLE_ITALIC); // kursiv setzen
                    textfeld->SetDefaultStyle(textattr);
                    kursiv = true;
                }
                else
                {
                    textattr.SetFontStyle(wxFONTSTYLE_NORMAL); // kursiv setzen
                    textfeld->SetDefaultStyle(textattr);
                    kursiv = false;
                }
                
                break;
                
            case 0x000F: // CTRL O Normal
                textfeld->AppendText(text.Left(pos));
                text = text.Mid(++pos);
                pos = 0;
                
                textfeld->SetDefaultStyle(defaultstyle);
                
                break;
            
            default:
                pos++;
        }
    }
    // Rest vom text Anhaengen
    textfeld->AppendText(text);
    
    // Normale Formatierung wiederherstellen
    textfeld->SetDefaultStyle(defaultstyle);
}

