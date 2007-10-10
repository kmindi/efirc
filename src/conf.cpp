#include <conf.h>

using namespace std;

ConfigInterface::ConfigInterface()
{
    // _vor_ opencfg(), da verwendet
    efirc_version_string = "pre-alpha 0.1";

    opencfg();
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
ConfigInterface::parsecfgvalue(string searchstring)
{
    unsigned int pos = configtext.find(searchstring, 0);
    string value = "";

    if(pos != string::npos)
    {
        pos += searchstring.length() + 3;
        value = configtext.substr(pos, configtext.find(";",pos) - pos);
    }

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
