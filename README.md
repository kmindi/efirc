**INACTIVE** (recently only moved from sourceforge to GitHub)
It was a hobby project during my school time. I don't think I will ever work on this again.

# efirc - easy and fast internet relay chat.

efirc is an easy, small and fast IRC (Internet Relay Chat) client. 
efirc is intended to be configured by you (owner of an irc channel) to fit the clients design yours and to preset the irc data for your customers/friends/clients.

http://efirc.sourceforge.net/

# Example Configuration

TODO

# Online Configuration Builder

http://efirc.sourceforge.net/make_configuration.php

TODO create github.io page for old homepage

## Building

1) Erstellen einer Arbeitskopie von wxWidgets [1]
2) Herunterladen und Entpacken von Mingw [2] 
3) Bauen von wxWidgets durch Ausführen von 
   "mingw32-make -f makefile.gcc"
   in build\msw (bei Bedarf mit anderen Optionen, siehe config.gcc)
4) Herunterladen von wx-config für Windows [3]
5) Setzen der Umgebungsvariablen WXWIN auf den Pfad der Arbeitskopie von wxWidgets (zB.: C:\wxWidgets)
6) Setzen der Umgebungsvariablen WXCFG auf den Pfad der Baukonfiguration (in der Regel gcc_lib\mswu, liegt unter lib)
7) Bauen von efirc durch Aufruf von compile.bat in trunk\src 

[1] https://github.com/wxWidgets/wxWidgets
[2] http://sourceforge.net/project/showfiles.php?group_id=2435
    - binutils-2.19.1-mingw32-bin.tar.gz
    - gcc-core-3.4.5-20060117-3.tar.gz
    - gcc-g++-3.4.5-20060117-3.tar.gz
    - mingw32-make-3.81-20080326-2.tar.gz
    - mingwrt-3.15.2-mingw32-dev.tar.gz
    - w32api-3.13-mingw32-dev.tar.gz

[3] https://sites.google.com/site/wxconfig/
