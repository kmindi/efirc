/**
** efirc steht unter der MIT-Lizenz.
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the MIT License.
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

#ifndef _KONFIGURATION_
#define _KONFIGURATION_

#include <wx/textfile.h>
#include <wx/string.h>
#include <wx/sstream.h>
#include <wx/url.h>

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
