#include <ui.h>

int
wxCALLBACK
sortUserlist(long item1, long item2, long sortData)
{
    // beide Strings vergleichen (Zeichen fuer Zeichen)
    return ((string *)item1)->compare(*(string *)item2);
}

using namespace std;

// Eventzuweisung
BEGIN_EVENT_TABLE(UserInterface, wxFrame)
    EVT_CLOSE      (UserInterface::OnClose)
    EVT_BUTTON     (ID_WXBUTTON_SUBMIT,
                    UserInterface::WxButton_submitClick)
END_EVENT_TABLE()

UserInterface::UserInterface(wxWindow *parent, wxWindowID id,
                   const wxString &title, const wxPoint &position,
                   const wxSize& size, long style)
         : wxFrame(parent, id, title, position, size, style)
{
    hindex = 0;
    hpos = 0;

    // Eingabefeld wurde noch nicht
    // "fokusiert"
    entered = false;

    // History ist leer
    for (int i = 0; i < HISTORY_SIZE; i++)
        history[i] = "";

    // Unsere Konfiguration
    config = new ConfigInterface();

    // Erstellen der grafischen Oberfläche (Objekte, Attribute,...)
    CreateGUIControls();

    // Eine gedrueckte Taste bei Fokus im Eingabefeld
    // loest das Ereignis aus, welches die festgelegte
    // Methode aufruft
    WxEdit_input_messages->Connect(wxEVT_KEY_DOWN,
       wxKeyEventHandler(UserInterface::WxEdit_input_messagesKeyDown),
       NULL, this);

    // Bei Fokus ggf. den Text im Eingabefeld loeschen
    WxEdit_input_messages->Connect(wxEVT_SET_FOCUS,
       wxKeyEventHandler(UserInterface::WxEdit_input_messagesFocus),
       NULL, this);
}

void
UserInterface::CreateGUIControls()
{
    SetTitle(_U(config->efirc_version_string));
    // TODO wie macht man das jetzt so...hmmmmmm? :D
    SetIcon(wxIcon(icon));
    SetSize(8, 8, 566, 341);
    Center();

    // TODO kurze Beschreibung der Objekte?
    // TODO das ist interessant (@bitweise)
    WxEdit_channel_users = new wxListCtrl(this, ID_WXEDIT_CHANNEL_USERS,
                                          wxPoint(442,3),
                                          wxSize(111,282),
                                          wxHSCROLL | wxLC_REPORT |
                                             wxLC_ALIGN_LEFT |
                                             wxLC_NO_HEADER);
    WxEdit_channel_users->InsertColumn(0, _U("Userlist"),
                                       wxLIST_FORMAT_LEFT, -1);
    WxEdit_topic = new wxTextCtrl(this, ID_WXEDIT_TOPIC,
                                  _U(""),
                                  wxPoint(4,4), wxSize(434,20),
                                  wxTE_READONLY, wxDefaultValidator,
                                  _U("WxEdit_topic"));
    WxEdit_input_messages = new wxTextCtrl(this,
                                           ID_WXEDIT_INPUT_MESSAGES,
                                           _U("Type your Message and"
                                               "press ENTER"),
                                           wxPoint(4,289),
                                           wxSize(434,20), 0,
                                           wxDefaultValidator,
                                           _U("WxEdit_input"
                                               "_messages"));

    WxButton_submit = new wxButton(this, ID_WXBUTTON_SUBMIT,
                                   _U("Submit"), wxPoint(442,289),
                                   wxSize(111,21), 0,
                                   wxDefaultValidator,
                                   _U("WxButton_submit"));

    WxEdit_output_messages = new wxTextCtrl(this,
                                            ID_WXEDIT_OUTPUT_MESSAGES,
                                            _U("Output"),
                                            wxPoint(4,26),
                                            wxSize(434,259),
                                            wxTE_READONLY |
                                               wxTE_MULTILINE,
                                            wxDefaultValidator,
                                            _U("WxEdit_output"
                                                "messages"));

    SetBackgroundColour(wxColour(_U(parsecfgvalue(
       "colour_background"))));
    WxEdit_topic->SetBackgroundColour(wxColour(_U(parsecfgvalue(
       "colour_topic_background"))));
    WxEdit_topic->SetForegroundColour(wxColour(_U(parsecfgvalue(
       "colour_topic_foreground"))));
    WxEdit_output_messages->SetBackgroundColour(wxColour(_U(parsecfgvalue(
       "colour_output_messages_background"))));
    WxEdit_output_messages->SetForegroundColour(wxColour(_U(parsecfgvalue(
       "colour_output_messages_foreground"))));
    WxEdit_input_messages->SetBackgroundColour(wxColour(_U(parsecfgvalue(
       "colour_input_messages_background"))));
    WxEdit_input_messages->SetForegroundColour(wxColour(_U(parsecfgvalue(
       "colour_input_messages_foreground"))));
    WxEdit_channel_users->SetBackgroundColour(wxColour(_U(parsecfgvalue(
       "colour_channel_users_background"))));
    WxEdit_channel_users->SetForegroundColour(wxColour(_U(parsecfgvalue(
       "colour_channel_users_foreground"))));
    WxButton_submit->SetBackgroundColour(wxColour(_U(parsecfgvalue(
       "colour_button_background"))));
    WxButton_submit->SetForegroundColour(wxColour(_U(parsecfgvalue(
       "colour_button_foreground"))));

    WxEdit_topic->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, FALSE,
                                 _U(
       parsecfgvalue("font_topic"))));
    WxEdit_output_messages->SetFont(wxFont(8, wxSWISS, wxNORMAL,
                                    wxNORMAL, FALSE, _U(
       parsecfgvalue("font_output_messages"))));
    WxEdit_input_messages->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL,
                                   FALSE, _U(
       parsecfgvalue("font_inputmessages"))));
    WxEdit_channel_users->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL,
                                        FALSE, _U(
       parsecfgvalue("font_channel_users"))));
    WxButton_submit->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL,
                                    FALSE, _U(
       parsecfgvalue("font_button"))));

    string clientinfotext_deutsch = "Das hier ist efirc (v. " +
        config->efirc_version_string + 
        ").\n"
        "efirc steht unter keiner Lizenz. Wir (die Programmierer) "
        "können nicht fuer das Verantwortlich gemacht werden was es "
        "macht oder auf irgendeine Weise hervorruft.\n"
        "Wenn Sie mehr wissen wollen hilft ihnen vielleicht diese Seite "
        "weiter: \"http://efirc.sourceforge.net/\".\n"
        "Mit /hilfe können Sie sich eine Liste mit allen "
        "verfügbaren Befehlen anzeigen lassen."
        "\n\n"
        "Server: " + parsecfgvalue("irc_server") + "\n"
        "Raum: " + parsecfgvalue("irc_channel") + "\n"
        "Sie sind bekannt als: " + parsecfgvalue("irc_nickname") +
        "\n\n------------------------------------------------\n";

    string clientinfotext_englisch = "This is efirc (v. " +
        config->efirc_version_string + 
        ").\n"
        "efirc is not released under any license. We (the programmer) "
        "can not be held responsible for anything it does "
        "or causes.\n"
        "If you want to get further information then have a look at: "
        "\"http://efirc.sourceforge.net/\".\n"
        "With /help you get a list of supported commands."
        "\n\n"
        "Server: " + parsecfgvalue("irc_server") + "\n"
        "Channel: " + parsecfgvalue("irc_channel") + "\n"
        "You are known as: " + parsecfgvalue("irc_nickname") +
        "\n\n------------------------------------------------\n";

    WxEdit_input_messages->SetFocus();

    if (parsecfgvalue("text_language") == "de")
    {
        //WxEdit_topic->SetValue("Thema: ");
        WxEdit_output_messages->Clear();
        WxEdit_input_messages->SetValue(_U("Geben Sie hier ihre Nachricht "
                                        "ein"));
        WxEdit_input_messages->SetInsertionPoint(33);
        WxButton_submit->SetLabel(_U("Senden"));
        add_message("(i) " + clientinfotext_deutsch);
    }
    // Englisch als Alternative
    else
    {
        //WxEdit_topic->SetValue("Topic: ");
        WxEdit_output_messages->Clear();
        WxEdit_input_messages->SetValue(_U("Put your message here"));
        WxEdit_input_messages->SetInsertionPoint(21);
        WxButton_submit->SetLabel(_U("Submit"));
        add_message("(i) " + clientinfotext_englisch);
    }

    SetTitle(_U(parsecfgvalue("text_title")
                 + " - [ "
                 + parsecfgvalue("irc_channel") + " ]"));
}

// Fuegt eine Nachricht im Ausgabefeld an und setzt davor einen
// Zeitstempel
void
UserInterface::add_message(string message)
{
    char timestamp[12];
    time_t raw_time;
    tm *local_time;

    time(&raw_time);
    local_time = localtime(&raw_time);

    strftime(timestamp, 12, "[%H:%M:%S] ", local_time);

    string prefix(timestamp);
    WxEdit_output_messages->AppendText(_U(prefix + message + "\n"));
}

// Fuegt einen Benutzer der Benutzerliste hinzu
void
UserInterface::add_user(string usersinastring)
{
    int item;
    string user;
    // Zeiger auf den im Speicher hinterlegten
    // Item-Bezeichner
    string *itemData;

    // es wird dem String ein Stream
    // zugewiesen
    stringstream ss(usersinastring);

    // Bis zum naechsten Leerzeichen aus
    // dem Stream lesen und eingelesene
    // Zeichenkette in user schreiben
    while (ss >> user)
        // noch nicht in der Liste?
        if(WxEdit_channel_users->FindItem(-1, _U(user)) == -1)
        {
            // Am Ende hinzufuegen
            item = WxEdit_channel_users->InsertItem(
               WxEdit_channel_users->GetItemCount() + 1, _U(user));

            // UserData ist ein Pointer auf
            // den Bezeichner
            itemData = new string
            (
                WxEdit_channel_users->GetItemText(item).mb_str(wxConvLocal)
            );

            // die Speicheradresse des
            // Bezeichners
            WxEdit_channel_users->SetItemData(item, (long)itemData);
        }

    // Liste sortieren
    WxEdit_channel_users->SortItems(sortUserlist, 0);
}

// Loescht einen Benutzer aus der Benutzerliste
void
UserInterface::delete_user(string user)
{
    // TODO angemessenere Loesung, um eventuellen Operator
    // oder Benutzer mit Voice Status aus der Liste zu nehmen
    WxEdit_channel_users->DeleteItem(WxEdit_channel_users->FindItem(-1,
                                     _U(user)));
    WxEdit_channel_users->DeleteItem(WxEdit_channel_users->FindItem(-1,
                                     _U("@" + user)));
    WxEdit_channel_users->DeleteItem(WxEdit_channel_users->FindItem(-1,
                                     _U("+" + user)));
}

// Aendert einen Nickname und zeigt die Aenderung an
void
UserInterface::change_nick(string nickchangeinput)
{
    // Uebergebenes Format: "nick -> neuernick"
    // Alten und neuen Nickname einlesen und in Variable (2)  speichern
    string alternick,neuernick;
    alternick = nickchangeinput.substr(0, nickchangeinput.find(" -> ",
                                       0));
    neuernick = nickchangeinput.substr(nickchangeinput.find(" -> ", 0)
                                       + 4, nickchangeinput.length()
                                       - 4 - alternick.length());

    // !!!UEBERARBEITEN!!!! TODO oh ja :D

    // Operator- und Voice-Status beachten
    if(WxEdit_channel_users->FindItem(-1, _U("@" + alternick)) > -1)
    {
        add_user("@" + neuernick);
    }
    else if(WxEdit_channel_users->FindItem(-1, _U("+" + alternick)) > -1)
    {

        add_user("+" + neuernick);
    }
    else
    {
        add_user(neuernick);
    }
    // Alten Nickname entfernen
    delete_user(alternick);

    // Nachricht anzeigen das jemand seinen Nickname geaendert hat
    add_message("(i) " + parsecfgvalue("local_changenick", alternick,
                                    neuernick));
}

// Zeigt das Thema des Channels an
void
UserInterface::set_topic(string topic)
{
    WxEdit_topic->SetValue(_U(topic));
    add_message("(i) " + parsecfgvalue("local_topic", topic));
}

void
UserInterface::clear_userlist()
{
    WxEdit_channel_users->DeleteAllItems();
}

string
UserInterface::parsecfgvalue(string searchstring, string param1,
                               string param2, string param3)
{
    return config->parsecfgvalue(searchstring, param1, param2, param3);
}


void
UserInterface::ParseClientCmd(string text)
{
    string cmd;
    string param;
    string channel;
    channel = irc->CurrentChannel;

    cmd = text.substr(0,text.find(" ",0));
    if(text.find(" ",0) != string::npos)
    {
        param = text.substr(text.find(" ",0)+1);
    }
    else
    {
        param = "";
    }

    if(cmd == "nick")
    {
        if(param == "")
        {
            WxEdit_input_messages->SetValue(_U(irc->CurrentNick));
        }
        else
        {
            irc->send_nick(param.c_str());
            // Aktuellen Nick manuell veraendern
            // damit die nickinuse-Funktion den
            // gewollten nick als aktuellen nickname
            // erhaelt
            irc->WantedNick = param;
        }
    }

    if(cmd == "join" and param != "")
    {
        WxEdit_channel_users->DeleteAllItems();
        WxEdit_topic->Clear();

        if (channel != "")
        {
            irc->send_part(channel.c_str());
        }

        irc->send_join(param.c_str());
    }

    if(cmd == "quit")
    {
        // TODO nachricht mitsenden
        string nachricht = 
        param + " [efirc " + config->efirc_version_string + "]";
        irc->disconnect_server(nachricht.c_str());

        //add_message("(i) " + parsecfgvalue("local_quitself",nachricht));

        //WxEdit_channel_users->DeleteAllItems();
        //WxEdit_topic->Clear();
        //SetTitle(_U(parsecfgvalue("text_title")));
        // kurz warten bevor alles geschlossen wird
        Sleep(10); 
        Destroy();
    }

    if(cmd == "leave" || cmd == "part")
    {
        irc->send_part(channel.c_str());

        channel = "";

        WxEdit_channel_users->DeleteAllItems();
        WxEdit_topic->Clear();
        SetTitle(_U(parsecfgvalue("text_title")));
    }

    if(cmd == "clear")
    {
        WxEdit_output_messages->Clear();
    }

    if(cmd == "kick")
    {
        //irc->send_kick(param);
        //Todo mit grund/trennungvon grund und nick
    }

    if(cmd == "whois" and param != "")
    {
        irc->send_whois(param.c_str());
    }


    if(cmd == "me" and param != "")
    {
        string me_text;
        me_text = "\001ACTION " + param + "\001";
        irc->send_privmsg(channel.c_str(), me_text.c_str());
        //geht nicht im query/msg
        add_message("*<" + irc->CurrentNick + "> " + param);
    }

    if(cmd == "topic")
    {
        if(param == "")
        {
            irc->send_topic(irc->CurrentChannel.c_str());
        }
        else
        {
            irc->send_topic(irc->CurrentChannel.c_str(),param.c_str());
        }
    }

    if(cmd == "query" || cmd == "msg" and param != "")
    {
        string recipient = param.substr(0,param.find(" ",0));
        string text = param.substr(param.find(" ",0)+1);
        add_message
        (
            "<" + irc->CurrentNick + "> -> [" + recipient + "] " + text
        );
        irc->send_privmsg(recipient.c_str(), text.c_str());
    }
    
    if(cmd == "ctcp" and param != "")
    {
        string recipient = param.substr(0,param.find(" ",0));
        string text = param.substr(param.find(" ",0)+1);
        add_message("[CTCP] -> [" + recipient + "] " + text);
        text = "\001" + text + "\001";
        irc->send_privmsg(recipient.c_str(), text.c_str());
    }
    
    if(cmd == "away")
    {
        if(param == "")
        {
            irc->send_away();
            add_message("(i) " + parsecfgvalue("local_unaway"));
        }
        else
        {
            irc->send_away(param.c_str());
            add_message("(i) " + parsecfgvalue("local_away",param));
        }
    }


    if(cmd == "help" || cmd == "hilfe")
    {
        if(parsecfgvalue("text_language") == "de")
        {
            add_message
            (
                "(i) Dieser Client unterstuetzt zur Zeit folgende Befehle: \n"
                "/nick [Neuernick] - ändert den Nickname zu Neuernick oder "
                    "zeigt den aktuellen Nick an\n"
                "/join #raum - verlässt den alten Raum und betritt den Raum "
                    "#raum\n"
                "/leave - verlässt den aktuellen Raum\n"
                "/quit - verlässt das IRC-Netzwerk\n"
                "/topic [Thema] - zeigt das aktuelle Thema an oder aendert es "
                    "zu [Thema]\n"
                "/[msg/query] Name Nachricht - sendet den text Nachricht an "
                    "Name\n"
                "/me macht irgendwas - CTCP Aktion\n"
                "/whois nick - zeigt Informationen zu Nick an\n"
                "/ctcp nick Befehl - sendet CTCP Befehl an Nick\n"
                "/clear - löscht das Ausgabefeld"
            );
        }
        else
        {
            add_message
            (
                "(i) Currently the following commands are supported by "
                    "this client: \n"
                "/nick [newnick] - changes the nickname to newnick or shows "
                    "the current nickname\n"
                "/join #channel - leaves the current channel and joins the "
                    "channel #channel\n"
                "/leave - leaves the current channel\n"
                "/quit - quits the irc-network\n"
                "/topic [topic] - shows the current topic or sets the topic "
                    "to [topic]\n"
                "/[msg/query] nick message - sends the text message to nick\n"
                "/me is doing something - CTCP Action\n"
                "/whois nick - displays information about nick\n"
                "/ctcp nick cmd - sends ctcp command cmd to nick\n"
                "/clear - removes all text from the output window"
            );
        }
    }

}

void
UserInterface::WxButton_submitClick(wxCommandEvent& event)
{
    messageSubmit();
}

void
UserInterface::messageSubmit()
{
    string text;
    string nick;
    string channel;

    nick = irc->CurrentNick;
    channel = irc->CurrentChannel;
    text = WxEdit_input_messages->GetValue().mb_str(wxConvLocal);

    if(text != "" && entered)
    {
        WxEdit_input_messages->Clear();

        // Falls / als Angabe fuer einen folgenden Befehl eingegeben
        // wurde den Nachfolgenden Text als Befehl(mit Parametern)
        // an Befehlsuntersuchungsfunktion uebergeben
        if(text.substr(0,1) == "/")
            ParseClientCmd(text.substr(1));
        else
        {
            add_message("<" + nick + "> " + text);

            // Text Senden
            irc->send_privmsg(channel.c_str(), text.c_str());
        }

        // Platz für neuen Eintrag schaffen
        hpos++;

        // den begrenzten Speicher nicht verlassen
        if (hpos > HISTORY_SIZE - 1)
            hpos = HISTORY_SIZE - 1;

        // Alle Eintraege Feld nach oben schieben
        for(int i = hpos; i > 1; i--)
            history[i] = history[i - 1];

        // Neuer Eintrag hinzufuegen und
        // Position in der History auf den
        // ersten Eintrag setzen
        history[1] = text;
        hindex = 0;
    }

    WxEdit_input_messages->SetFocus();
}

// TODO name change
//      vector use
// Historywanderung
void
UserInterface::WxEdit_input_messagesKeyDown(wxKeyEvent& event)
{
    // Der Wert der gedrueckten Taste
    int myPressedKey;
    // Der History-Eintrag
    string myCommand;

    myPressedKey = event.GetKeyCode();

    // Wir warten auf die Pfeiltaste nach oben
    if (myPressedKey == WXK_UP)
    {
        // History-Up
        hindex++;

        // Ignorieren der oberen leeren Felder
        // und Bereich einhalten
        if (hindex > hpos)
            hindex = hpos;
    }
    else if (myPressedKey == WXK_DOWN)
    {
        // History-Down
        hindex--;

        if (hindex < 0)
            hindex = 0;
    }
    else if (myPressedKey == WXK_RETURN)
        messageSubmit();
    else
    {
        // siehe WxEdit_input_messagesFocus()
        if(!entered)
        {
            WxEdit_input_messages->Clear();

            entered = true;
        }

        // Ereignis weiterleiten
        // und Ende ueberspringen
        event.Skip();

        return;
    }

    // Befehl aus History an festgelegter Stelle
    // lesen
    myCommand = history[hindex];

    // History-Eintrag ins Eingabefeld kopieren
    WxEdit_input_messages->Clear();
    WxEdit_input_messages->WriteText(_U(myCommand));
}

// Bei Fokuserhalt Standardtext loeschen
void
UserInterface::WxEdit_input_messagesFocus(wxKeyEvent& event)
{
    if (!entered)
    {
        WxEdit_input_messages->Clear();

        entered = true;
    }
}

// Programm Ende
void
UserInterface::OnClose(wxCloseEvent& event)
{
    Destroy();
}
