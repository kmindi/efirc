#ifndef __IRCInterface_Header__
#define __IRCInterface_Header__
#include <string>
#include <ircsocket.h>

using namespace std;

class IRCInterface : public IRCSocket
{
    public:
        IRCInterface(string, string, string, string, string, string);
        ~IRCInterface();
};
#endif
