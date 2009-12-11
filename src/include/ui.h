/**
** efirc steht unter der "Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland Lizenz".
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the "Creative Commons Attribution-Share Alike 3.0 Germany License".
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// ui.h
// Enthaelt die Definition der Fensterklasse

#ifndef _Fenster_
#define _Fenster_

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/regex.h>

class Fenster : public wxPanel
{
    private:
        // Zeiger fuer die zu erstellenden Objekte erzeugen
        wxListCtrl *WxList_benutzerliste;
        wxButton   *WxButton_senden;
        wxTextCtrl *WxEdit_thema;
        wxTextCtrl *WxEdit_eingabefeld;
        wxTextCtrl *WxEdit_ausgabefeld;

        void ObjekteAnpassen();

        enum
        {
            ID_WxList_benutzerliste = 1007,
            ID_WxEdit_thema = 1006,
            ID_WxButton_senden = 1004,
            ID_WxEdit_eingabefeld = 1003,
            ID_WxEdit_ausgabefeld = 1001,
        };

        void WxButton_sendenClick(wxCommandEvent&);
        void BeiMausAufURL(wxTextUrlEvent& event);

        void NachrichtSenden();

        // Eingabegeschichte
        wxArrayString geschichte_texte;
        unsigned int geschichte_position;


        DECLARE_EVENT_TABLE()
        void WxEdit_eingabefeldTasteGedrueckt(wxKeyEvent& event);
        void WxEdit_eingabefeldFokus(wxKeyEvent&);
        
        static void FormatiertAnzeigen(wxTextCtrl *textfeld, wxString text);

    public:
        wxString fenster_name;
        Fenster(const wxString& title, const wxPoint& pos, const wxSize& size);
        ~Fenster() { }

        void TitelSetzen(wxString titel);
        void NachrichtAnhaengen(wxString local, wxString param1 = _T(""), wxString param2 = _T(""), wxString param3 = _T(""), wxString param4 = _T(""));
        void ThemaAendern(wxString thema, wxString benutzer=_T(""));
        void AusgabefeldLeeren();

        //Benutzerlistenfunktionen
        void BenutzerHinzufuegen(wxString benutzerliste = _T(""));
        bool BenutzerEntfernen(wxString benutzer = _T(""));
        bool BenutzerAendern(wxString altername, wxString neuername);
        wxString HoleBenutzerModi(wxString benutzer);
        void BenutzerModiHinzufuegen(wxString benutzer, wxString modi);
        void BenutzerModiEntfernen(wxString benutzer, wxString modi);

        bool AnzeigeBegrenzungErreicht();
};

static const wxString IRC_Farben [] =
{    
    // siehe: http://www.mirc.com/help/color.txt
    
    _T("#FFFFFF"),  // white
    _T("#000000"),  // Black
    _T("#000080"),  // Navy Blue
    _T("#008000"),  // Green
    _T("#ff0000"),  // Red
    _T("#800000"),  // Maroon Brown
    _T("#800080"),  // purple
    _T("#ffa500"),  // Orange
    _T("#ffff00"),  // Yellow
    _T("#00ff00"), // LIme
    _T("#00ffff"), // Cyan
    _T("#e0ffff"), // light cyan
    _T("#add8e6"), // light blue
    _T("#ffc0cb"), // pink
    _T("#808080"), // Gray
    _T("#c0c0c0")  // Silver
};

#endif
