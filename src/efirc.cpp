#include <efirc.h>

// TODO was das wieder?
IMPLEMENT_APP(Efirc)

using namespace std;

// Empfangene normale Nachrichten werden ausgegeben
void
irc_pmsg(const irc_msg_data *msg_data, void *cp)
{
    string text = msg_data->params_a[1];
    string recipient = msg_data->params_a[0];
    string user = msg_data->nick;

    // CTCP abfragen ("'\001'BEFEHL text'\001'")
    //Falls kein CTCP:
    if (text[0] != '\001' and text[text.length()-1] != '\001')
    {
        if(user != "")
        {
            if(recipient == irc->CurrentNick)
            {
                frame->add_message("[" + user + "] " + text);
            }
            else
            {
                frame->add_message("<" + user + "> " + text);
            }
        }
        else
        {
            frame->add_message(text);
        }
    }
    //falls CTCP:
    else
    {
        //CTCP behandeln
        string ctcp = text.substr(1,text.length()-2);
        string ctcp_befehl = ctcp.substr(0,ctcp.find(" ",0));
        string ctcp_text = ctcp.substr(ctcp.find(" ",0)+1);

        if(ctcp_befehl == "VERSION")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
            frame->add_message("[" + user +"] [CTCP] VERSION");
            string answer;
            answer = "\001VERSION efirc:"
                + config->efirc_version_string + ":Windoofs\001";
            irc->send_notice(user.c_str(), answer.c_str());
            }
            else
            {
                frame->add_message("[" + user +"] [CTCP] VERSION " + ctcp_text);
            }
        }

        if(ctcp_befehl == "FINGER")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
            frame->add_message("[" + user +"] [CTCP] FINGER");
            }
            else
            {
                frame->add_message("[" + user +"] [CTCP] FINGER " + ctcp_text);
            }
        }

        if(ctcp_befehl == "SOURCE")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
            frame->add_message("[" + user +"] [CTCP] SOURCE");
            string answer = "\001SOURCE http://efirc.sf.net/\001";
            irc->send_notice(user.c_str(), answer.c_str());
            }
            else
            {
                frame->add_message("[" + user +"] [CTCP] SOURCE " + ctcp_text);
            }
        }

        if(ctcp_befehl == "USERINFO")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
            frame->add_message("[" + user +"] [CTCP] USERINFO");
            }
            else
            {
                frame->add_message("[" + user +"] [CTCP] USERINFO " + ctcp_text);
            }
        }

        if(ctcp_befehl == "CLIENTINFO")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
            frame->add_message("[" + user +"] [CTCP] CLIENTINFO");
            string answer = "\001CLIENTINFO VERSION FINGER SOURCE USERINFO CLIENTINFO PING TIME\001";
            irc->send_notice(user.c_str(), answer.c_str());
            }
            else
            {
                frame->add_message("[" + user +"] [CTCP] CLIENTINFO " + ctcp_text);
            }
        }

        // TODO
        // ping format immer mit timestamp
        // unterschied ist nur privmsg zum anfordern und notice als antwort
        if(ctcp_befehl == "PING")
        {
                frame->add_message("[" + user +"] [CTCP] PING " + ctcp_text);
        }


        if(ctcp_befehl == "TIME")
        {
            if(ctcp_text == ctcp || ctcp_text == "")
            {
            frame->add_message("[" + user +"] [CTCP] TIME");

            char timestamp[30];
            time_t raw_time;
            tm *local_time;

            time(&raw_time);
            local_time = localtime(&raw_time);

            strftime(timestamp, 30, "%d.%m.%Y %H:%M:%S", local_time);

            string timestring(timestamp);


            string answer = "\001TIME "+ timestring + "\001";
            irc->send_notice(user.c_str(), answer.c_str());
            }
            else
            {
                frame->add_message("[" + user +"] [CTCP] TIME " + ctcp_text);
            }
        }

        if(ctcp_befehl == "ACTION")
        {
            if(recipient == irc->CurrentNick)
            {
                frame->add_message("[" + user + "]* " + ctcp_text);
            }
            else
            {
                frame->add_message("<" + user + ">* " + ctcp_text);
            }
        }
    }
}

void
irc_mode(const irc_msg_data *msg_data, void *cp)
{
    int i;
    string Sender = msg_data->nick;
    string Parameters = "";
    string Channel = "";
    string Modus = "";

    if(Sender == "")
    {
        /*if(string(msg_data->sender) == irc->CurrentNick)
        {
            Sender = irc->CurrentNick;
        }
        else
        {*/
            Sender = "Server";
        //}
    }

    for(i = 0; i < msg_data->params_i; i++)
    {
        if(msg_data->params_a[i][0] == '#')
        {
            Channel = " (" + string(msg_data->params_a[i]) + ")";
        }
        else if(msg_data->params_a[i][0] == '+' ||
                msg_data->params_a[i][0] == '-')
        {
            Modus = msg_data->params_a[i];
        }
        else
        {
            Parameters += " " + string(msg_data->params_a[i]);
        }
    }

    Modus += Parameters + Channel;

    frame->add_message(config->parsecfgvalue("local_mode", Sender, Modus));
}

void
irc_motd(const irc_msg_data *msg_data, void *cp)
{
    frame->add_message(msg_data->params_a[1]);
}

// Am Ende der Nachricht des Tages automatisch den in
// der Konfigurationsdatei genannten Kanal betreten
void
irc_endofmotd(const irc_msg_data *msg_data, void *cp)
{
    IRCSocket *ircsocket = (IRCSocket *) cp;
    ircsocket->send_join(config->parsecfgvalue("irc_channel").c_str());
    frame->add_message("------------------------------------------------");
}

// Thema des Raums anzeigen lassen
void
irc_topic(const irc_msg_data *msg_data, void *cp)
{
    string topic = msg_data->params_a[2];
    frame->set_topic(topic);
}

void
irc_requestedtopic(const irc_msg_data *msg_data, void *cp)
{
    string topic = msg_data->params_a[1];
    frame->set_topic(topic);
}

// Benutzerliste einlesen
void
irc_userlist(const irc_msg_data *msg_data, void *cp)
{
    string benutzerliste;
    benutzerliste = msg_data->params_a[3];
    frame->add_user(benutzerliste);

    /*
    frame->add_message("(i) Folgende Benutzer sind zur Zeit im Raum: \n"
                       + benutzerliste);
    */
}

// Benutzerliste aktualisieren / Benutzer hat den Raum
// betreten
void
irc_join(const irc_msg_data *msg_data, void *cp)
{
    string benutzer;
    benutzer = msg_data->nick;

    if (benutzer == irc->CurrentNick)
    {
        irc->CurrentChannel = msg_data->params_a[0];
        frame->SetTitle(_U(config->parsecfgvalue("text_title")
                               + " - [ "
                               + irc->CurrentChannel + " ]"));
        frame->add_message("(i) " + config->parsecfgvalue("local_joinself"));
    }
    else
    {
        frame->add_message("(i) " + config->parsecfgvalue("local_join",
                                        benutzer));

        // wenn selber, reicht irc_userlist() aus, da
        // neuer Channel und eventuell Operator
        frame->add_user(benutzer);
    }
}

// Benutzerliste aktualisieren / Benutzer hat den Raum
// verlassen
void
irc_leave(const irc_msg_data *msg_data, void *cp)
{
    string benutzer;
    benutzer = msg_data->nick;
    frame->delete_user(benutzer);
    frame->add_message("(i) " + config->parsecfgvalue("local_leave", benutzer));
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
    frame->add_message("(i) " + config->parsecfgvalue("local_quit", benutzer, nachricht));
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
        frame->add_message("(i) " + config->parsecfgvalue("local_kickself",sender));

        frame->clear_userlist();
        frame->set_topic("");
        frame->SetTitle(_U(config->parsecfgvalue("text_title")));
    }
    else
    {
        frame->delete_user(benutzer);
        frame->add_message("(i) " + config->parsecfgvalue("local_kick",
                                            benutzer, sender));
    }
}

void
irc_nickinuse(const irc_msg_data *msg_data, void *cp)
{
    frame->add_message("(i) " + config->parsecfgvalue("local_nickinuseinfo"));

    // Nickname erneuern
    // aktuellen Nickname uebergeben
    config->reset_nickname(irc->WantedNick);
    // neuen Nicknamen auslesen
    irc->CurrentNick = config->parsecfgvalue("irc_nickname");
    frame->add_message("(i) " + config->parsecfgvalue("local_newnickinfo",
                        config->parsecfgvalue("irc_nickname")));
    irc->send_nick(config->parsecfgvalue("irc_nickname").c_str());
}

void
irc_error(const irc_msg_data *msg_data, void *cp)
{
    string text = msg_data->params_a[2];
    frame->add_message("(!) " + text);
}

//whois antworten
void
irc_whoisuser(const irc_msg_data *msg_data, void *cp)
{
    string nick = msg_data->params_a[1];
    string user = msg_data->params_a[2];
    string host = msg_data->params_a[3];
    string name = msg_data->params_a[5];
    frame->add_message("(i) " + nick
                       + " (" + user + "@" + host + " - " + name + ")");
}

void
irc_whoisaway(const irc_msg_data *msg_data, void *cp)
{
    string nick = msg_data->params_a[1];
    string text = msg_data->params_a[2];
    frame->add_message("(i) " + config->parsecfgvalue("local_whoisaway",nick,text));
}

void
irc_whoischan(const irc_msg_data *msg_data, void *cp)
{
    string nick = msg_data->params_a[1];
    string chans = msg_data->params_a[2];

    // Rechte noch beachten ([@|+]#channel)
    frame->add_message("(i) " + config->parsecfgvalue("local_whoischan",nick,chans));
}

void
irc_whoisidle(const irc_msg_data *msg_data, void *cp)
{
    string nick = msg_data->params_a[1];
    string seconds = msg_data->params_a[2];

    frame->add_message("(i) " + config->parsecfgvalue("local_whoisidle",nick,seconds));
}

void
irc_whoisserver(const irc_msg_data *msg_data, void *cp)
{
    string nick = msg_data->params_a[1];
    string server = msg_data->params_a[2];
    string servernachricht = msg_data->params_a[3];

    frame->add_message("(i) "
    + nick + " " + server + " " + servernachricht);
}

// Thread fuer recv_raw-Schleife
#ifdef WINDOWS
void
#else
void *
#endif
recv_thread(void *cp)
{
    IRCSocket *ircsocket = (IRCSocket *)cp;
    ircsocket->recv_raw();
}

// Abarbeitung der Befehlsschlange
#ifdef WINDOWS
void
#else
void *
#endif
call_thread(void *cp)
{
    IRCSocket *ircsocket = (IRCSocket *)cp;
    ircsocket->call_cmd();
}

#ifdef WINDOWS
void
#else
void *
#endif
connect_thread(void *cp)
{
    // not used
    #ifndef WINDOWS
    pthread_t rt, ct;
    #endif

    irc->connect();

    // Wer sagt mir, dass der Nick verfuegbar ist???
    irc->CurrentNick = config->parsecfgvalue("irc_nickname");
    irc->WantedNick = config->parsecfgvalue("irc_nickname");
    irc->CurrentChannel = config->parsecfgvalue("irc_channel");

    // Ereignisverknüpfung
    // TODO wirklich Ereignisse implementieren
    irc->add_link("PRIVMSG", &irc_pmsg);
    irc->add_link("NOTICE", &irc_pmsg);
    irc->add_link("MODE", &irc_mode);
    irc->add_link("301", &irc_whoisaway);
    irc->add_link("311", &irc_whoisuser);
    irc->add_link("312", &irc_whoisserver);
    irc->add_link("317", &irc_whoisidle);
    irc->add_link("319", &irc_whoischan);
    irc->add_link("372", &irc_motd);
    irc->add_link("376", &irc_endofmotd);
    irc->add_link("332", &irc_topic);
    irc->add_link("TOPIC", &irc_requestedtopic);
    irc->add_link("353", &irc_userlist);
    irc->add_link("JOIN", &irc_join);
    irc->add_link("PART", &irc_leave);
    irc->add_link("QUIT", &irc_quit);
    irc->add_link("NICK", &irc_changenick);
    irc->add_link("PING", &irc_pong);
    irc->add_link("KICK", &irc_kick);

    //Fehler Antworten
    irc->add_link("401", &irc_error);
    irc->add_link("402", &irc_error);
    irc->add_link("403", &irc_error);
    irc->add_link("404", &irc_error);
    irc->add_link("405", &irc_error);
    irc->add_link("406", &irc_error);
    irc->add_link("407", &irc_error);
    irc->add_link("409", &irc_error);
    irc->add_link("411", &irc_error);
    irc->add_link("412", &irc_error);
    irc->add_link("413", &irc_error);
    irc->add_link("414", &irc_error);
    irc->add_link("421", &irc_error);
    irc->add_link("422", &irc_error);
    irc->add_link("423", &irc_error);
    irc->add_link("424", &irc_error);
    irc->add_link("431", &irc_error);
    irc->add_link("432", &irc_error);
    irc->add_link("433", &irc_nickinuse);
    irc->add_link("436", &irc_error);
    irc->add_link("441", &irc_error);
    irc->add_link("442", &irc_error);
    irc->add_link("443", &irc_error);
    irc->add_link("444", &irc_error);
    irc->add_link("445", &irc_error);
    irc->add_link("446", &irc_error);
    irc->add_link("451", &irc_error);
    irc->add_link("461", &irc_error);
    irc->add_link("462", &irc_error);
    irc->add_link("463", &irc_error);
    irc->add_link("464", &irc_error);
    irc->add_link("465", &irc_error);
    irc->add_link("467", &irc_error);
    irc->add_link("471", &irc_error);
    irc->add_link("472", &irc_error);
    irc->add_link("473", &irc_error);
    irc->add_link("474", &irc_error);
    irc->add_link("475", &irc_error);
    irc->add_link("481", &irc_error);
    irc->add_link("482", &irc_error);
    irc->add_link("483", &irc_error);
    irc->add_link("491", &irc_error);
    irc->add_link("501", &irc_error);
    irc->add_link("502", &irc_error);

    #ifdef WINDOWS
    _beginthread(recv_thread, 0, irc);
    _beginthread(call_thread, 0, irc);
    #else
    pthread_create(&rt, NULL, &recv_thread, irc);
    pthread_create(&ct, NULL, &call_thread, irc);
    #endif
}

bool
Efirc::OnInit()
{
    // not used
    #ifdef WINDOWS
    WSADATA wsaData;
    #else
    pthread_t ct;
    #endif

    frame = new UserInterface(NULL);
    frame->Show();

    config = new ConfigInterface();

    // TODO nach irc (class)
    #ifdef WINDOWS
    if (WSAStartup(MAKEWORD(1, 1), &wsaData))
        frame->add_message("(!) Failed to initialise winsock!");
    #endif

    irc = new IRCInterface(config->parsecfgvalue("irc_port"),
                           config->parsecfgvalue("irc_server"),
                           config->parsecfgvalue("irc_nickname"),
                           config->parsecfgvalue("irc_username"),
                           config->parsecfgvalue("irc_realname"),
                           "pass");

    #ifdef WINDOWS
    _beginthread(connect_thread, 0, NULL);
    #else
    pthread_create(&ct, NULL, &connect_thread, NULL);
    #endif

    frame->irc = irc;

    return true;
}

int
Efirc::OnExit()
{
    irc->disconnect_server(config->efirc_version_string.c_str());

    #ifdef WINDOWS
    WSACleanup();
    #endif

    return 0;
}
