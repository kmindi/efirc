#ifndef __UserInterface_Header__
#define __UserInterface_Header__
#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/colour.h>
#include <sstream>
#include <conf.h>
#include <irc.h>

#include <icon.xpm>

// fester Grenzwert der History-Eintraege
#define HISTORY_SIZE 512

using namespace std;

class UserInterface : public wxFrame
{
    public:
        IRCInterface *irc;

        UserInterface(wxWindow *parent, wxWindowID id = 1,
                 const wxString &title = wxT("efirc"),
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long  style        = wxCAPTION | wxSYSTEM_MENU |
                                      wxMINIMIZE_BOX | wxCLOSE_BOX); // TODO wat das jetzt?

        void add_message(string);

        void add_user(string);
        void change_nick(string);
        void delete_user(string);
        void set_topic(string);
        void clear_userlist();
        
        string parsecfgvalue(string);

        
        
    private:
        ConfigInterface *config;

        // Position in der History
        int hindex;
        // Aktueller History-Index
        int hpos;

        // Legt fest, ob das Eingabefeld
        // geleert wird oder nicht
        bool entered;

        // Die History wird durch ein Array repraesentiert
        // und ist begrenzt in den moeglichen Eintraegen
        string history[HISTORY_SIZE];
        
        
        // grafische Objekte auf dem Frame
        wxListCtrl *WxEdit_channel_users;
        wxButton   *WxButton_submit;
        wxTextCtrl *WxEdit_topic;
        wxTextCtrl *WxEdit_input_messages;
        wxTextCtrl *WxEdit_output_messages;
        // TODO wozu das denn???
        enum
        {
            ID_WXEDIT_CHANNEL_USERS = 1007,
            ID_WXEDIT_TOPIC = 1006,
            ID_WXBUTTON_SUBMIT = 1004,
            ID_WXEDIT_INPUT_MESSAGES = 1003,
            ID_WXEDIT_OUTPUT_MESSAGES = 1001,
            ID_DUMMY_VALUE_
        };

        DECLARE_EVENT_TABLE();

        

        // wrapper
        void ParseClientCmd(string);

        void CreateGUIControls();

        void WxButton_submitClick(wxCommandEvent& event);
        void WxEdit_input_messagesKeyDown(wxKeyEvent& event);
        void WxEdit_input_messagesFocus(wxKeyEvent& event);

        void OnClose(wxCloseEvent& event);
};
#endif
