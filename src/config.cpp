#include <config.h>

using namespace std;

Konfiguration::Konfiguration(wxString dateipfad, wxString standardkonfiguration)
{
    opencfg(dateipfad);
    standardkonfiguration_text = standardkonfiguration;
}

void Konfiguration::opencfg(wxString dateipfad)
{
    wxTextFile file;
    bool datei_ist_nicht_vorhanden = file.Create(dateipfad); // Create liefert false wenn die Datei schon vorhanden ist
    wxString zeile;
    konfiguration_text = _T("");
    
    if(!datei_ist_nicht_vorhanden)
    {
        file.Open(dateipfad);
        for ( int i = 0; i<file.GetLineCount(); i++ )
        {
            konfiguration_text += file.GetLine(i) + _T("\n");
        }
    }
}

wxString Konfiguration::parsecfgvalue(wxString searchstring)
{
    if(konfiguration_text.Contains(searchstring + _T(" = ")))
    // Wenn die Konfiguration die gesuchte Option enthaelt ...
    {
        unsigned int beginn_auslesen = konfiguration_text.Find(searchstring + _T(" = ")) + searchstring.Len() + 3;
        wxString tmp_wert = konfiguration_text.Mid(beginn_auslesen); // alles nach der Position auslesen
        tmp_wert = tmp_wert.BeforeFirst(_T('\n'));
        
        return tmp_wert;
    }
    else
    // ... wenn nicht, in Standardkonfiguration suchen
    {
        if(standardkonfiguration_text.Contains(searchstring))
        {
            unsigned int beginn_auslesen = standardkonfiguration_text.Find(searchstring + _T(" = ")) + searchstring.Len() + 3;
            wxString tmp_wert = standardkonfiguration_text.Mid(beginn_auslesen); // alles nach der Position auslesen
            tmp_wert = tmp_wert.BeforeFirst(_T('\n'));
            
            return tmp_wert;
        }
    }

    return _T("");
}

// Konfiguration aendern
bool Konfiguration::edit_cfg(wxString option, wxString neuerwert, bool aenderungenspeichern)
{
    bool erfolg = false;
    
    
    return erfolg;
}

// Konfiguration aendern, text ersetzen
bool Konfiguration::edit_cfg_replace(wxString alterwert, wxString neuerwert, bool aenderungenspeichern)
{
    bool erfolg = false;
    
    int i = konfiguration_text.Replace(alterwert,neuerwert);
    if(i >0) {erfolg = true;}
    return erfolg;
}
