#ifndef __ConfigInterface_Header__
#define __ConfigInterface_Header__
#include <fstream>

using namespace std;

class ConfigInterface
{
    public:
        ConfigInterface();
        ~ConfigInterface();

        string parsecfgvalue(string, string param1 = "", 
                             string param2 = "", string param3 = "");
        void reset_nickname(string);

        // TODO wrapper (_alle_ public-vars)
        string efirc_version_string;

    private:
        string configtext;
        //standardtexte
        string locals_deutsch,locals_englisch;

        void opencfg();

        string ReplaceString(const string &, const string &, string);
        string generaterndstring(int);
};
#endif
