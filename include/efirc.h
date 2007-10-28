#ifndef __Efirc_Header__
#define __Efirc_Header__
#ifdef WINDOWS
#include <process.h>
#else
#include <pthread.h>
#endif
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
