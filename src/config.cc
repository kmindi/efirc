/**
** efirc steht unter der "Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland Lizenz".
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the "Creative Commons Attribution-Share Alike 3.0 Germany License".
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

#include <config.h>

Konfiguration::Konfiguration(wxString dateipfad, wxString standardkonfiguration)
{
    standardkonfiguration_text = standardkonfiguration;
    opencfg(dateipfad);
}

void Konfiguration::opencfg(wxString dateipfad)
{
    if(dateipfad.Left(7) == _T("http://") || dateipfad.Left(6) == _T("ftp://"))
    {
        wxURL url(dateipfad);
        
        if(url.GetError() == wxURL_NOERR)   
        {
            wxInputStream *in = url.GetInputStream();

            if(in && in->IsOk()) 
            {
                wxStringOutputStream html_stream(&konfiguration_text);
                in->Read(html_stream);
            }
            delete in;
        }
    }
    else
    {
        wxTextFile file;
        bool datei_ist_nicht_vorhanden = file.Create(dateipfad); // Create liefert false wenn die Datei schon vorhanden ist
        wxString zeile;
        konfiguration_text = _T("");

        if(!datei_ist_nicht_vorhanden)
        {
            file.Open(dateipfad, wxConvLocal);
            for ( unsigned int i = 0; i < file.GetLineCount(); i++ )
            {
                konfiguration_text += file.GetLine(i) + _T("\n");
            }
        }
        else
        {
            file.Open(dateipfad, wxConvLocal);

            wxString tmp_standardkonfiguration_text = standardkonfiguration_text;
            while(tmp_standardkonfiguration_text != _T(""))
            {
                file.AddLine(tmp_standardkonfiguration_text.BeforeFirst(_T('\n')));
                tmp_standardkonfiguration_text = tmp_standardkonfiguration_text.AfterFirst(_T('\n'));
            }
            file.Write(); // Aenderungen anwenden
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

    // fuer Standardkonfiguration
    i = standardkonfiguration_text.Replace(alterwert,neuerwert);
    if(i >0) {erfolg = true;}

    return erfolg;
}
