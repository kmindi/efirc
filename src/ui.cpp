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
    EVT_TEXT_ENTER (ID_WXEDIT_TOPIC,
                    UserInterface::WxButton_submitClick)
    EVT_BUTTON     (ID_WXBUTTON_SUBMIT,
                    UserInterface::WxButton_submitClick)
    EVT_TEXT_ENTER (ID_WXEDIT_INPUT_MESSAGES,
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
    SetTitle(wxT(config->efirc_version_string));
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
    WxEdit_channel_users->InsertColumn(0, wxT("Userlist"),
                                       wxLIST_FORMAT_LEFT, -1);
    WxEdit_topic = new wxTextCtrl(this, ID_WXEDIT_TOPIC,
                                  wxT("TOPIC: testtopic ["
                                      "http://www.testtopic.com/]"),
                                  wxPoint(4,4), wxSize(434,20),
                                  wxTE_READONLY, wxDefaultValidator,
                                  wxT("WxEdit_topic"));
    WxEdit_input_messages = new wxTextCtrl(this,
                                           ID_WXEDIT_INPUT_MESSAGES,
                                           wxT("Type your Message and"
                                               "press ENTER"),
                                           wxPoint(4,289),
                                           wxSize(434,20), 0,
                                           wxDefaultValidator,
                                           wxT("WxEdit_input"
                                               "_messages"));

    WxButton_submit = new wxButton(this, ID_WXBUTTON_SUBMIT,
                                   wxT("Submit"), wxPoint(442,289),
                                   wxSize(111,21), 0,
                                   wxDefaultValidator,
                                   wxT("WxButton_submit"));

    WxEdit_output_messages = new wxTextCtrl(this,
                                            ID_WXEDIT_OUTPUT_MESSAGES,
                                            wxT("Output"),
                                            wxPoint(4,26),
                                            wxSize(434,259),
                                            wxTE_READONLY |
                                               wxTE_MULTILINE,
                                            wxDefaultValidator,
                                            wxT("WxEdit_output"
                                                "messages"));

    SetBackgroundColour(wxColour(parsecfgvalue(
       "colour_background")));
    WxEdit_topic->SetBackgroundColour(wxColour(parsecfgvalue(
       "colour_topic_background")));
    WxEdit_topic->SetForegroundColour(wxColour(parsecfgvalue(
       "colour_topic_foreground")));
    WxEdit_output_messages->SetBackgroundColour(wxColour(parsecfgvalue(
       "colour_output_messages_background")));
    WxEdit_output_messages->SetForegroundColour(wxColour(parsecfgvalue(
       "colour_output_messages_foreground")));
    WxEdit_input_messages->SetBackgroundColour(wxColour(parsecfgvalue(
       "colour_input_messages_background")));
    WxEdit_input_messages->SetForegroundColour(wxColour(parsecfgvalue(
       "colour_input_messages_foreground")));
    WxEdit_channel_users->SetBackgroundColour(wxColour(parsecfgvalue(
       "colour_channel_users_background")));
    WxEdit_channel_users->SetForegroundColour(wxColour(parsecfgvalue(
       "colour_channel_users_foreground")));
    WxButton_submit->SetBackgroundColour(wxColour(parsecfgvalue(
       "colour_button_background")));
    WxButton_submit->SetForegroundColour(wxColour(parsecfgvalue(
       "colour_button_foreground")));

    WxEdit_topic->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, FALSE,
                                 wxT(
       parsecfgvalue("font_topic"))));
    WxEdit_output_messages->SetFont(wxFont(8, wxSWISS, wxNORMAL,
                                    wxNORMAL, FALSE, wxT(
       parsecfgvalue("font_output_messages"))));
    WxEdit_input_messages->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL,
                                   FALSE, wxT(
       parsecfgvalue("font_inputmessages"))));
    WxEdit_channel_users->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL,
                                        FALSE, wxT(
       parsecfgvalue("font_channel_users"))));
    WxButton_submit->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL,
                                    FALSE, wxT(
       parsecfgvalue("font_button"))));

    string clientinfotext_deutsch = "Das hier ist efirc (v. "
        + config->efirc_version_string
        + ").\n"
        + "efirc steht unter keiner Lizenz. Wir (die Programmierer) "
        + "koennen nicht fuer das Verantwortlich gemacht werden was es "
        + "macht oder auf irgendeine Weise hervorruft.\n"
        + "Wenn du mehr wissen willst hilft dir vielleicht diese Seite "
        + "weiter: \"http://efirc.sourceforge.net/\".\n"
        + "Mit /hilfe kannst du dir eine Liste mit allen "
        + "verfügbaren Befehlen anzeigen lassen."
        + "\n\n"
        + "Server: " + parsecfgvalue("irc_server") + "\n"
        + "Raum: " + parsecfgvalue("irc_channel") + "\n"
        + "Du bist bekannt als: " + parsecfgvalue("irc_nickname")
        + "\n\n";

    string clientinfotext_englisch = "This is efirc (v. "
        + config->efirc_version_string
        + ").\n"
        + "efirc is not released under any license. We (the people who "
        + "made efirc) can not be held responsible for anything it does "
        + "or causes.\n"
        + "If you want to get further information then have a look at: "
        + "\"http://efirc.sourceforge.net/\".\n"
        + "With /help you get a list of supported commands."
        + "\n\n"
        + "Server: " + parsecfgvalue("irc_server") + "\n"
        + "Channel: " + parsecfgvalue("irc_channel") + "\n"
        + "You are known as: " + parsecfgvalue("irc_nickname")
        + "\n\n";

    if (parsecfgvalue("text_language") == "de")
    {
        WxEdit_topic->SetValue("Thema: ");
        WxEdit_output_messages->Clear();
        WxEdit_input_messages->SetValue("Geben Sie hier ihre Nachricht "
                                        "ein");
        WxButton_submit->SetLabel("Senden");
        add_message(clientinfotext_deutsch);
    }

    if (parsecfgvalue("text_language") == "en")
    {
        WxEdit_topic->SetValue("Topic: ");
        WxEdit_output_messages->Clear();
        WxEdit_input_messages->SetValue("Put your message here");
        WxButton_submit->SetLabel("Submit");
        add_message(clientinfotext_englisch);
    }

    SetTitle(wxT(parsecfgvalue("text_title")
                               + " - [ "
                               + parsecfgvalue("irc_channel") + " ]"));
}

// Fuegt eine Nachricht im Ausgabefeld an und setzt davor einen
// Zeitstempel
void
UserInterface::add_message(string message)
{
    char timestamp[11];
    time_t raw_time;
    tm *local_time;

    time(&raw_time);
    local_time = localtime(&raw_time);

    strftime(timestamp, 11, "[%H:%M:%S]", local_time);

    string prefix(timestamp);
    WxEdit_output_messages->AppendText(prefix + " " + message + "\n");
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
        if(WxEdit_channel_users->FindItem(-1, user) == -1)
        {
            // Am Ende hinzufuegen
            item = WxEdit_channel_users->InsertItem(
               WxEdit_channel_users->GetItemCount() + 1, user);

            // UserData ist ein Pointer auf
            // den Bezeichner
            itemData = new string(WxEdit_channel_users->GetItemText(item));

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
    // TODO angemessenere Loesung, um eventuellen Operator aus
    //      der Liste zu nehmen
    WxEdit_channel_users->DeleteItem(WxEdit_channel_users->FindItem(-1,
                                     user));
    WxEdit_channel_users->DeleteItem(WxEdit_channel_users->FindItem(-1,
                                     "@" + user));
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
    // Alten Nickname entfernen und neuen an die Liste anhaengen
    delete_user(alternick);
    // Operator-Status beachten
    if(WxEdit_channel_users->FindItem(-1, "@" + alternick) > -1)
        add_user("@" + neuernick);
    else
        add_user(neuernick);

    // Nachricht anzeigen das jemand seinen Nickname geaendert hat
    add_message(alternick + "'s neuer Nickname ist: " + neuernick);
}

// Zeigt das Thema des Channels an
void
UserInterface::set_topic(string topic)
{
    WxEdit_topic->SetValue("Thema: " + topic);
    add_message("<i> Thema: " + topic);
}

void
UserInterface::clear_userlist()
{
    WxEdit_channel_users->DeleteAllItems();
}

string
UserInterface::parsecfgvalue(string cfgvalue)
{
    return config->parsecfgvalue(cfgvalue);
}


void
UserInterface::ParseClientCmd(string text)
{
    string cmd;
    string param;
    string channel;

    cmd = text.substr(0,text.find(" ",0));
    if(text.find(" ",0) != string::npos)
    {
        param = text.substr(text.find(" ",0)+1);
    }
    else
    {
        param = "";
    }
    channel = irc->CurrentChannel;

    if(cmd == "nick")
    {
        irc->send_nick(param.c_str());
        // Aktuellen Nick manuell veraendern
        // damit die nickinuse-Funktion den
        // gewollten nick als aktuellen nickname
        // erhaelt
        irc->CurrentNick = param;
    }

    if(cmd == "join")
    {
        WxEdit_channel_users->DeleteAllItems();
        WxEdit_topic->Clear();

        if (channel != "")
            irc->send_part(channel.c_str());

        irc->send_join(param.c_str());
    }

    if(cmd == "quit")
    {
        irc->send_quit(param.c_str());

        add_message("<i> Du hast das IRC Netzwerk verlassen");

        WxEdit_channel_users->DeleteAllItems();
        WxEdit_topic->Clear();
        SetTitle(wxT(parsecfgvalue("text_title")));
    }

    if(cmd == "leave" || cmd == "part")
    {
        irc->send_part(channel.c_str());

        channel = "";

        WxEdit_channel_users->DeleteAllItems();
        WxEdit_topic->Clear();
        SetTitle(wxT(parsecfgvalue("text_title")));
    }

    if(cmd == "clear")
    {
        WxEdit_output_messages->Clear();
    }

    if(cmd == "kick")
    {
        //kick
    }

    if(cmd == "whois")
    {
        // whois
    }


    if(cmd == "me")
    {
        // me
    }
    
    if(cmd == "topic")
    {
        irc->send_topic(irc->CurrentChannel.c_str(),param.c_str());
    }

    if(cmd == "help" || cmd == "hilfe")
    {
        if(parsecfgvalue("text_language") == "de")
        {
            add_message("<i> Dieser Client unterstuetzt zur Zeit folgende Befehle: \n"
             "/nick Neuernick - ändert den Nickname zu Neuernick\n"
             "/join #raum - verlässt den alten Raum und betritt den Raum #raum\n"
             "/leave - verlässt den aktuellen Raum"
             "/quit - verlässt das IRC-Netzwerk\n"
             "/topic [Thema] - zeigt das aktuelle Thema an oder aendert es zu [Thema]\n"
             "/clear - löscht das Ausgabefeld");
        }
        else
        {
            add_message("<i> Currently the following commands are supported by this client: \n"
             "/nick newnick - changes the nickname to newnick\n"
             "/join #channel - leaves the current channel and joins the channel #channel\n"
             "/leave - leaves the current channel\n"
             "/quit - quits the irc-network\n"
             "/topic [topic] - shows the current topic or sets the topic to [topic]\n"
             "/clear - removes all text from the output window");
        }
    }

}

void
UserInterface::WxButton_submitClick(wxCommandEvent& event)
{
    string text;
    string nick;
    string channel;

    nick = irc->CurrentNick;
    channel = irc->CurrentChannel;
    text = WxEdit_input_messages->GetValue();

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
    else
    {
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
    WxEdit_input_messages->WriteText(myCommand);
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
