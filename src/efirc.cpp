#include <efirc.h>

// TODO was das wieder?
IMPLEMENT_APP(Efirc)

using namespace std;

// Thread fuer recv_raw-Schleife
void
recv_thread(void *cp)
{
    IRCSocket *ircsocket = (IRCSocket *)cp;
    ircsocket->recv_raw();
}

// Abarbeitung der Befehlsschlange
void
call_thread(void *cp)
{
    IRCSocket *ircsocket = (IRCSocket *)cp;
    ircsocket->call_cmd();
}

// Empfangene normale Nachrichten werden ausgegeben
void
irc_pmsg(const irc_msg_data *msg_data, void *cp)
{
    string text = msg_data->params_a[1];
    string user = msg_data->nick;

    // CTCP vorerst ignorieren
    if (text[0] != '\001')
        frame->add_message("<" + user + "> " + text);
}

// Am Ende der Nachricht des Tages automatisch den in
// der Konfigurationsdatei genannten Kanal betreten
void
irc_endofmotd(const irc_msg_data *msg_data, void *cp)
{
    IRCSocket *ircsocket = (IRCSocket *) cp;
    ircsocket->send_join(config->parsecfgvalue("irc_channel").c_str());
}

// Thema des Raums anzeigen lassen
void
irc_topic(const irc_msg_data *msg_data, void *cp)
{
    string topic;
    topic = msg_data->params_a[2];
    frame->set_topic(topic);
}

// Benutzerliste einlesen
void
irc_userlist(const irc_msg_data *msg_data, void *cp)
{
    string benutzerliste;
    benutzerliste = msg_data->params_a[3];
    frame->add_user(benutzerliste);
    frame->add_message("<i> Folgende Benutzer sind zur Zeit im Raum: \n"
                       + benutzerliste);
}

// Benutzerliste aktualisieren / Benutzer hat den Raum
// betreten
void
irc_join(const irc_msg_data *msg_data, void *cp)
{
    string benutzer;
    benutzer = msg_data->nick;
    frame->add_message("<i> " + benutzer + " hat den Raum betreten");

    if (benutzer == irc->CurrentNick)
    {
        irc->CurrentChannel = msg_data->params_a[0];
        frame->SetTitle(wxT(frame->parsecfgvalue("text_title")
                               + " - [ "
                               + irc->CurrentChannel + " ]"));
    }
    else
        // wenn selber, reicht irc_userlist() aus, da
        // neuer Channel und eventuell Operator
        frame->add_user(benutzer);
}

// Benutzerliste aktualisieren / Benutzer hat den Raum
// verlassen
void
irc_leave(const irc_msg_data *msg_data, void *cp)
{
    string benutzer;
    benutzer = msg_data->nick;
    frame->delete_user(benutzer);
    frame->add_message("<i> " + benutzer + " hat den Raum verlassen");
    frame->SetTitle(wxT(frame->parsecfgvalue("text_title")));
}

// Benutzerliste aktualisieren / Benutzer hat den Raum und
// das Netzwerk verlassen
void
irc_quit(const irc_msg_data *msg_data, void *cp)
{
    string benutzer, nachricht;
    benutzer = msg_data->nick;
    nachricht = msg_data->params_a[0];
    
    frame->delete_user(benutzer);
    frame->add_message("<i> " + benutzer + " ist gegangen: "
                       + nachricht);
}

// Benutzerliste aktualisieren / Benutzer hat seinen
//  Nickname geaendert
void
irc_changenick(const irc_msg_data *msg_data, void *cp)
{
    string alternick, neuernick;
    alternick = msg_data->nick;
    neuernick = msg_data->params_a[0];
    frame->change_nick(alternick + " -> " + neuernick);

    if (alternick == irc->CurrentNick)
        irc->CurrentNick = neuernick;
}

void
irc_pong(const irc_msg_data *msg_data, void *cp)
{
    irc->send_pong(msg_data->params_a[0]);
}

void
irc_kick(const irc_msg_data *msg_data, void *cp)
{
    string benutzer,sender;
    sender = msg_data->nick;
    benutzer = msg_data->params_a[1];
    if(benutzer == irc->CurrentNick)
    {
        frame->add_message("<i> Sie wurden von " 
        + sender + "aus dem Raum geworfen");

        frame->clear_userlist();
        frame->set_topic("");
        frame->SetTitle(wxT(frame->parsecfgvalue("text_title")));
    }
    else
    {
        frame->delete_user(benutzer);
        frame->add_message("<i> " + benutzer + " wurde von " 
        + sender + " aus dem Raum geworfen");
    }
}

void
irc_nickinuse(const irc_msg_data *msg_data, void *cp)
{
    frame->add_message("<i> Nickname wird bereits verwendet");
    
    // Nickname erneuern
    config->reset_nickname();
    irc->CurrentNick = config->parsecfgvalue("irc_nickname");
    frame->add_message("<i> Sie sind jetzt bekannt als "
                    + config->parsecfgvalue("irc_nickname"));
                    
    // Verbindung abbrechen
    irc->disconnect_server("");    
}

bool
Efirc::OnInit()
{
    frame = new UserInterface(NULL);
    frame->Show();

    WSADATA wsaData;

    // TODO nach irc (class)
    if (WSAStartup(MAKEWORD(1, 1), &wsaData))
        frame->add_message("Failed to initialise winsock!\n");

    config = new ConfigInterface();
    irc = new IRCInterface(config->parsecfgvalue("irc_port"),
                           config->parsecfgvalue("irc_server"),
                           config->parsecfgvalue("irc_nickname"),
                           config->parsecfgvalue("irc_username"),
                           config->parsecfgvalue("irc_realname"),
                           "pass");

    // Wer sagt mir, dass der Nick verfuegbar ist???
    irc->CurrentNick = config->parsecfgvalue("irc_nickname");
    irc->CurrentChannel = config->parsecfgvalue("irc_channel");

    frame->irc = irc;

    // Ereignisverknüpfung
    // TODO wirklich Ereignisse implementieren
    irc->add_link("PRIVMSG", &irc_pmsg);
    irc->add_link("376", &irc_endofmotd);
    irc->add_link("332", &irc_topic);
    irc->add_link("353", &irc_userlist);
    irc->add_link("JOIN", &irc_join);
    irc->add_link("PART", &irc_leave);
    irc->add_link("QUIT", &irc_quit);
    irc->add_link("NICK", &irc_changenick);
    irc->add_link("PING", &irc_pong);
    irc->add_link("KICK", &irc_kick);
    irc->add_link("433", &irc_nickinuse);

    _beginthread(recv_thread, 0, irc);
    _beginthread(call_thread, 0, irc);

    return true;
}

int
Efirc::OnExit()
{
    WSACleanup();

    return 0;
}
