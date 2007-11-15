#include <conf.h>

using namespace std;

ConfigInterface::ConfigInterface()
{
    // _vor_ opencfg(), da verwendet
    efirc_version_string = "pre-alpha 0.2";

    opencfg();

    locals_deutsch =
        "local_changenick = %param1's neuer Nickname ist %param2;\n"
        "local_joinself = Sie haben den Raum betreten;\n"
        "local_join = %param1 hat den Raum betreten;\n"
        "local_leave = %param1 hat den Raum verlassen;\n"
        "local_quit = %param1 hat das Netzwerk verlassen (\"%param2\");\n"
        "local_kickself = Sie wurden von %param1 aus dem Raum geworfen;\n"
        "local_kick = %param1 wurde von %param2 aus dem Raum geworfen;\n"
        "local_nickinuseinfo = Nickname wird bereits verwendet;\n"
        "local_newnickinfo = Ihr neuer Nickname ist %param1;\n"
        "local_whoisaway = %param1 ist gerade abwesend (\"%param2\");\n"
        "local_whoischan = %param1 befindet sich in %param2;\n"
        "local_whoisidle = %param1 ist inaktiv seit %param2 Sekunden;\n"
        "local_topic = Thema: %param1;\n"
        "local_quitself = Sie haben das IRC Netzwerk verlassen;\n"
        "local_unaway = Sie sind jetzt nicht mehr als abwesend markiert;\n"
        "local_away = Sie sind jetzt als abwesend (%param1) markiert;\n"
        "local_mode = %param1 setzt Modus %param2";

    locals_englisch =
        "local_changenick = %param1's new nick is %param2;\n"
        "local_joinself = You joined the channel;\n"
        "local_join = %param1 joined the channel;\n"
        "local_leave = %param1 left;\n"
        "local_quit = %param1 left the network (\"%param2\");\n"
        "local_kickself = You were kicked by %param1;\n"
        "local_kick = %param1 was kicked by %param2;\n"
        "local_nickinuseinfo = This nickname is already in use;\n"
        "local_newnickinfo = Your new nick is %param1;\n"
        "local_whoisaway = %param1 is currently away (\"%param2\");\n"
        "local_whoischan = %param1 is on %param2;\n"
        "local_whoisidle = %param1 is inactive since %param2 Sekunden;\n"
        "local_topic = Topic: %param1;\n"
        "local_quitself = You left the IRC-Network;\n"
        "local_unaway = Removed your away status;\n"
        "local_away = You are marked as being away (%param1);\n"
        "local_mode = %param1 set mode %param2";

    if(parsecfgvalue("text_language")== "de")
    {
        configtext += "\n" + locals_deutsch;
    }
    else
    {
        configtext += "\n" + locals_englisch;
    }
}

void
ConfigInterface::opencfg()
{
    string s;

    // TODO nur lesend oder static!
    ifstream f("efirc.cfg");
    while(f)
    {
        getline(f, s);

        if(!s.empty())
            configtext += s + "\n";
    }

    configtext = ReplaceString("[efirc_version]",
                               efirc_version_string,
                               configtext);
    configtext = ReplaceString("[efirc_random_string]",
                               generaterndstring(4), configtext);
}

string
ConfigInterface::parsecfgvalue(string searchstring, string param1,
                               string param2, string param3)
{
    unsigned int pos = configtext.find(searchstring + " = ", 0);
    string value = "";

    if(pos != string::npos)
    {
        pos += searchstring.length() +3;
        value = configtext.substr(pos, configtext.find(";",pos) - pos);
    }

    // Moegliche uebergebene parameter in den texten ersetzen
    value = ReplaceString("%param1", param1, value);
    value = ReplaceString("%param2", param2, value);
    value = ReplaceString("%param3", param3, value);

    return value;
}

// TODO API
string
ConfigInterface::ReplaceString(const string &stringSearchString,
                               const string &stringReplaceString,
                               string stringStringToReplace)
{

    int intLengthSearch = stringSearchString.length();
    string::size_type pos = stringStringToReplace.find(
                               stringSearchString, 0);

    while(string::npos != pos )
    {
        stringStringToReplace.replace(pos, intLengthSearch,
                                      stringReplaceString);

        pos = stringStringToReplace.find(stringSearchString, 0);
    }

    return stringStringToReplace;
}

string
ConfigInterface::generaterndstring(int chars)
{
    int i;
    string rndstring;

    // Zufallsgenerator initialisieren
    srand(time(NULL));

    for(i = 1; i <= chars; i++)
        rndstring += char(65 + rand()%26);

    return rndstring;
}

void
ConfigInterface::reset_nickname(std::string usednick)
{
    // option + wert in der temp. konfiguration aendern
    // mit anhaengen von zwei zufaelligen zeichen
    // nick -> nickQP ; nick -> nickSG; oder so

    string alternick = parsecfgvalue("irc_nickname");
    configtext = ReplaceString("irc_nickname = " + alternick + ";",
                               "irc_nickname = " + usednick +
                               generaterndstring(2) + ";", configtext);

}
