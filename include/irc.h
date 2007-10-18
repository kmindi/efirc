#ifndef __IRCInterface_Header__
#define __IRCInterface_Header__
#include <string>
#include <cppirc/ircsocket.h>

using namespace std;

class IRCInterface : public IRCSocket
{
    public:
        string CurrentNick;
        string CurrentChannel;
        string wantedNick;

        IRCInterface(string, string, string, string, string, string);
        ~IRCInterface();

        void connect();
};
#endif
