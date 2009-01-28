// ui.h
// Enthaelt die Definition der Fensterklasse

#ifndef _Fenster_
#define _Fenster_

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/string.h>
#include <wx/colour.h>

#include <icon.xpm>

class Fenster : public wxFrame
{
    private:
        // Zeiger f�r die zu erstellenden Objekte erzeugen
        wxListCtrl *WxList_benutzerliste;
        wxButton   *WxButton_senden;
        wxTextCtrl *WxEdit_thema;
        wxTextCtrl *WxEdit_eingabefeld;
        wxTextCtrl *WxEdit_ausgabefeld;
        
        wxTextAttr defaultstyle;
        
        enum
        {
            ID_WxList_benutzerliste = 1007,
            ID_WxEdit_thema = 1006,
            ID_WxButton_senden = 1004,
            ID_WxEdit_eingabefeld = 1003,
            ID_WxEdit_ausgabefeld = 1001,
        };
        
        void OnClose(wxCloseEvent& event);
        void WxButton_sendenClick(wxCommandEvent& event);
        void BeiAktivierung(wxActivateEvent& event);
        
        wxString eingabe_geschichte[50];
        //Autocomplete.
        

        DECLARE_EVENT_TABLE()
    
    public:
        int fensternummer;
        Fenster(const wxString& title, const int& id, const wxPoint& pos, const wxSize& size, long style= wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX); 
        
        
        void TitelSetzen(wxString titel, wxString nick = "", wxString hostname = "", wxString port = "");
        void NachrichtAnhaengen(wxString local, wxString param1 = "", wxString param2 = "", wxString param3 = "", wxString param4 = "");
        void Fehler(int fehlernummer = 0, wxString param1 = "");
        void ThemaAendern(wxString thema, wxString benutzer="");
        void AusgabefeldLeeren();
        
        //Benutzerlistenfunktionen
        void BenutzerHinzufuegen(wxString benutzerliste = "");
        void BenutzerEntfernen(wxString benutzer = "");
        void BenutzerAendern(wxString altername, wxString neuername);
        //void Benutzerliste_benutzer_modus(wxString& benutzer, wxString);
        
        
        
};

#endif
