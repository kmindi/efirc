/**
** efirc steht unter der "Creative Commons Namensnennung-Weitergabe unter gleichen Bedingungen 3.0 Deutschland Lizenz".
** efirc wird und wurde bis jetzt von Kai Mindermann und Fabian Ruch entwickelt.
** Dieser Hinweis darf nicht entfernt werden!
**
** efirc is licensed under the "Creative Commons Attribution-Share Alike 3.0 Germany License".
** efirc was and is developed by Kai Mindermann and Fabian Ruch.
** This important information must not be removed!
**/

// zentrale.h
// Definiert die Klasse Zentrale

#ifndef _ZENTRALE_
#define _ZENTRALE_

#include <map>

// wxwidgets Quellen einbinden
#include <wx/app.h>
#include <wx/version.h> // fuer Versionsinformationen von wxWidgets
#include <wx/platinfo.h> // fuer Informationen der Laufzeitumgebung
#include <wx/utils.h> // fuer BS, Benutzer und Netzwerkfunktionen
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/textdlg.h> // fuer textdialog zur eingabe eines einzeiligen textes
#include <wx/panel.h>
#include <wx/frame.h> 
#include <wx/cmdline.h> // um uebergebene Kommandozeilenparameter verarbeiten zu koennen

//fuer Reiter (Tab) basierende Anzeige der Fenster
#include <wx/aui/aui.h>
#include <wx/aui/auibook.h> 

class Zentrale; //Thread muss wissen, dass Zentrale eine Klasse ist

// eigene Quellen einbinden
#include <thread.h>
#include <config.h>
#include <ui.h>
#include <irc.h>
#include <event.h>
#include <version.h>

#include <icon.xpm>

// Klasse fuer das Hauptfenster
// Eigene Klasse wird benoetigt um diesem Fenster Ereignisse zuordnen zu koennen
class Hauptfenster : public wxFrame
{
    public:
        Hauptfenster(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr) : wxFrame(parent, id, title, pos, size, style, name) { }
        DECLARE_EVENT_TABLE();
    private:
        void BeiFensterSchliessen(wxAuiNotebookEvent& );
};

void irc_allgemein(const irc_msg_data *msg_data, void *cp);

class Zentrale : public wxApp
{
    public:
        Zentrale() {}
        ~Zentrale() {}

        virtual bool OnInit();
        
        wxString efirc_version_string;
        wxString raeume;

        // Freunde damit sie auf die Zentrale zugreifen koennen
        friend class Thread; 
        friend class Ereignisverwalter; 
        friend class Hauptfenster; 

        IRC *irc;
        Ereignisverwalter *Ereignisvw; // Zeiger auf einen eigenen Ereignisverwalter
        Konfiguration *config; // Zeiger zur Konfiguration

        void TitelSetzen(wxString titel = _T(""), wxString nick = _T(""));
        void fensterzerstoeren(wxString);
        void EingabeVerarbeiten(wxString, wxString);

        Fenster* fenstersuchen(wxString);
        Fenster* fenster(wxString name);
        unsigned int anzahl_offene_fenster();
        
        wxAuiNotebook* notebook;
        

    private:
        wxFrame* reiterframe;
    
        map<wxString, Fenster*> zgr_fenster;
        Fenster* neuesFenster(wxString); // erstellt neue Instanz fuer uebergebenen Raum
        
        wxTextEntryDialog* nickdialog; // Nickname aendern - Dialog

        int OnExit();
        
        wxCmdLineParser* parser;

        void zeige_ueber();

        //Konfiguration
        wxString standardkonfiguration();
        wxString zufallstext(int anzahl_zeichen = 4);
        void Konfiguration_anpassen();

        void BefehlVerarbeiten(wxString fenstername, wxString befehl);
        void NachrichtSenden(wxString fenstername, wxString nachricht);

        //Threads
        void threads_starten();
            void connect_thread();
            void recv_thread();
            void call_thread();

        Thread* thrd_connect;
        Thread* thrd_recv;
        Thread* thrd_call;

        // IRC FUNKTIONEN
        void irc_pmsg(const IRC_NACHRICHT *);
        void irc_welcome(const IRC_NACHRICHT *msg_data);
        void irc_isupport(const IRC_NACHRICHT *msg_data);
        void irc_mode(const IRC_NACHRICHT *msg_data);
        void irc_endofmotd(const IRC_NACHRICHT *msg_data);
        void irc_motd(const IRC_NACHRICHT *msg_data);
        void irc_chanurl(const IRC_NACHRICHT *msg_data);
        void irc_topic(const IRC_NACHRICHT *msg_data);
        void irc_topicwhotime(const IRC_NACHRICHT *msg_data);
        void irc_requestedtopic(const IRC_NACHRICHT *msg_data);
        void irc_pong(const IRC_NACHRICHT *msg_data);
        void irc_whoisuser(const IRC_NACHRICHT *msg_data);
        void irc_whoisaway(const IRC_NACHRICHT *msg_data);
        void irc_whoischan(const IRC_NACHRICHT *msg_data);
        void irc_whoisidle(const IRC_NACHRICHT *msg_data);
        void irc_whoisserver(const IRC_NACHRICHT *msg_data);
        void irc_whoisspecial(const IRC_NACHRICHT *msg_data);
        void irc_whoisactually(const IRC_NACHRICHT *msg_data);
        void irc_userlist(const IRC_NACHRICHT *msg_data);
        void irc_join(const IRC_NACHRICHT *msg_data);
        void irc_leave(const IRC_NACHRICHT *msg_data);
        void irc_quit(const IRC_NACHRICHT *msg_data);
        void irc_nick(const IRC_NACHRICHT *msg_data);
        void irc_nickinuse(const IRC_NACHRICHT *msg_data);
        void irc_unaway(const IRC_NACHRICHT *msg_data);
        void irc_nowaway(const IRC_NACHRICHT *msg_data);
        void irc_invite(const IRC_NACHRICHT *msg_data);
        void irc_kick(const IRC_NACHRICHT *msg_data);
        
        void irc_einfach(const IRC_NACHRICHT *msg_data);
        void irc_fehler(const IRC_NACHRICHT *msg_data);
        void irc_unbekannt(const IRC_NACHRICHT *msg_data);
};

static const wxString standardkonfiguration_text =

    // Verbindungsinformationen
    _T("irc_server = irc.freenode.net\n")
    _T("irc_port = 6667\n")
    _T("irc_channel = #efirc\n")
    _T("irc_username = efirc\n")
    _T("irc_realname = %real_name\n")
    _T("irc_nickname = user_%random_string\n")

    _T("max_DONT_SHOW_USERLIST_CHANGES = 25\n")
    _T("bool_automatic_nickchange_if_in_use = 1\n")

    // Farben
    _T("colour_background = #510000\n")
    _T("colour_topic_background = #EEEEEE\n")
    _T("colour_topic_foreground = #000000\n")
    _T("colour_output_messages_background = #EEEEEE\n")
    _T("colour_output_messages_foreground = #000000\n")
    _T("colour_input_messages_background = #EEEEEE\n")
    _T("colour_input_messages_foreground = #000000\n")
    _T("colour_channel_users_background = #EEEEEE\n")
    _T("colour_channel_users_foreground = #000000\n")
    _T("colour_button_background = #EEEEEE\n")
    _T("colour_button_foreground = #640C0C\n")
    _T("colour_error = #FF0000\n")

    // Texte
    _T("text_title = %param4 [%param1@%param2:%param3] - \n")

    // Schriftarten
    _T("font_topic = \n")
    _T("font_output_messages = \n")
    _T("font_input_messages = \n")
    _T("font_channel_users = \n")
    _T("font_button = \n")

    // Standardtexte
    // S =s elber, fuer Nachrichten die man selber geschrieben hat
    // P = privat, fuer Nachrichten die nicht an einen Raum, sondern an einen selber geschrieben wurden.
    // BL = Benutzerliste
    _T("local_P_PRIVMSG = -%param1- %param2\n")
    _T("local_S_P_PRIVMSG = >%param1< %param2\n")
    _T("local_PRIVMSG = <%param1> %param2\n")
    _T("local_PRIVMSG_NOSENDER = %param2\n")
    _T("local_ACTION = *%param1 %italic%param2%italic\n")
    _T("local_P_ACTION = *%param1 %italic%param2%italic\n")
    _T("local_CHANNEL_URL = The channel url is: %param1\n")
    _T("local_TOPIC = The topic is: %param1\n")
    _T("local_TOPICWHOTIME = Topic was set by %param1 at %param2\n")
    _T("local_TOPIC_CHANGE = %param2 changed the topic to %param1\n")
    _T("local_BL_JOIN = %param1 joined the channel\n")
    _T("local_BL_PART = %param1 has left the channel (%param2)\n")
    _T("local_BL_QUIT = %param1 has left the network (%param2)\n")
    _T("local_INVITE = %param1 invited you to %param2\n")
    _T("local_BL_NICK = %param1 changed his nickname to %param2\n")
    _T("local_CTCP = -%param1@CTCP- %param2\n")
    _T("local_S_CTCP = >%param1@CTCP< %param2\n")
    _T("local_MODE = %param1 set mode %param2\n")
    _T("local_AWAY = You have been marked as being away (%param1)\n")
    _T("local_RPL_UNAWAY = You are not longer marked as being away\n")
    _T("local_RPL_NOWAWAY = You have been marked as being away\n")
    _T("local_WHOIS_USER = WHOIS: %param1 (%param2@%param3 - %param4)\n")
    _T("local_WHOIS_AWAY = WHOIS: %param1 is away %param2\n")
    _T("local_WHOIS_CHANNEL = WHOIS: %param1 is in %param2\n")
    _T("local_WHOIS_IDLE = WHOIS: %param1 is idle since %param2 seconds\n")
    _T("local_WHOIS_SERVERMSG = WHOIS: %param1 %param2 %param3\n")
    _T("local_WHOIS_SPECIAL = WHOIS: %param1 %param2\n")
    _T("local_WHOIS_ACTUALLY = WHOIS: %param1 is actually using host %param2\n")
    _T("local_KICK = %param1 has kicked %param2 (%param3)\n")
    _T("local_KICK_SELF = You were kicked by %param1 (%param2)\n")
    _T("local_quit_message = efirc sagt tschuess\n")

    _T("local_label_button = Send\n")
    _T("local_label_input = Put your message here and press Enter\n")

    // Fehler (werden in konfigurierter Farbe dargestellt)
    _T("local_ERR_IRC = (!) %param1\n") // case 2
    _T("local_ERR_COMMAND_UNKNOWN = (!) Unknown command (%param1)\n") // case 4
    _T("local_ERR_IRC_COMMAND_UNKNOWN = (!) Unknown IRC command (%param1)\n") // case 5
    _T("local_ERR_COMMAND_MISSING_PARAMETER = (!) Missing parameter for \"%param1\"\n") // case 5

    // Anzeige bestimmter Dialog oder anderer nicht irc spezifischer Nachrichten
    _T("local_DLG_NEWNICK_NICKINUSE_TEXT = The Nickname you try to use is already in use. You have to choose a new one:\n") // case 5
    _T("local_DLG_NEWNICK_CAPTION = Change your Nickname\n") // case 5
    ;

static const wxCmdLineEntryDesc befehlszeilenparameterliste [] =
{
     {
        wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters", 
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP 
     },
     { wxCMD_LINE_OPTION, "c", "config", "Path to different configuration file than the default efirc.cfg", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
     { wxCMD_LINE_OPTION, "n", "nick", "Nickname", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
     { wxCMD_LINE_OPTION, "s", "server", "IP or Hostname of the IRC-Server", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
     { wxCMD_LINE_OPTION, "ch", "channel", "Channel(s) to join (comma seperated list)", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
     { wxCMD_LINE_OPTION, "p", "port", "Port", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
 
     { wxCMD_LINE_NONE }
};



#endif
