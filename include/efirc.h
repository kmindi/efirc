#ifndef __Efirc_Header__
#define __Efirc_Header__
#include <process.h>
#include <ui.h>
#include <conf.h>
#include <irc.h>

class Efirc : public wxApp
{
    public:
        bool OnInit();
        int OnExit();
};

IRCInterface *irc;
UserInterface *frame;
ConfigInterface *config;
#endif
