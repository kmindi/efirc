#include <irc.h>

IRCInterface::IRCInterface() : IRCSocket(6667, "chat.freenode.net",
                                         "efirc", "efirc", "efirc",
                                         "pass",
                                         fopen("cppirc.log", "w"))
{
}
