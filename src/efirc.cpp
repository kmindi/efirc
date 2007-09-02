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
    while(1) ircsocket->call_cmd();
}

// Empfangene normale Nachrichten werden ausgegeben
void
irc_pmsg(const irc_msg_data *msg_data, void *cp)
{
    string text = msg_data->text;
    string user = msg_data->nick;
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
    topic = msg_data->text;
    frame->set_topic(topic);
}

// Benutzerliste einlesen
void
irc_userlist(const irc_msg_data *msg_data, void *cp)
{
    string benutzerliste;
    benutzerliste = msg_data->text;
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
    frame->add_user(benutzer);
    frame->add_message("<i> " + benutzer + " hat den Raum betreten");
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
}

// Benutzerliste aktualisieren / Benutzer hat den Raum und
// das Netzwerk verlassen
void
irc_quit(const irc_msg_data *msg_data, void *cp)
{
    string benutzer, nachricht;
    benutzer = msg_data->nick;
    nachricht = msg_data->text;
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
    neuernick = msg_data->text;
    frame->change_nick(alternick + " -> " + neuernick);
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
    irc = new IRCInterface();
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

    _beginthread(recv_thread,0,irc);
    _beginthread(call_thread,0,irc);

    return true;
}

int
Efirc::OnExit()
{
    WSACleanup();

    return 0;
}
