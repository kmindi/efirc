#ifndef _KONFIGURATION_
#define _KONFIGURATION_

#include <wx/textfile.h>
#include <wx/string.h>

class Konfiguration
{
    private:
        wxString konfiguration_text;
        wxString standardkonfiguration_text;

        void opencfg(wxString = _T(""));

    public:
        Konfiguration(wxString dateipfad = _T(""), wxString standardkonfiguration = _T(""));
        ~Konfiguration();

        wxString parsecfgvalue(wxString);

        bool edit_cfg(wxString option, wxString neuerwert, bool aenderungenspeichern = false);
        bool edit_cfg_replace(wxString alterwert, wxString neuerwert, bool aenderungenspeichern = false);
};
#endif
