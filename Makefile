CPP=g++.exe
DEF=-DWINDOWS
INCDIR=-Iinclude
LIBDIR=-Llib
SRCDIR=src
OBJDIR=lib
BINDIR=bin
LIB=-mwindows -lwxmsw28 -lcomctl32 -lole32 -loleaut32 -luuid -lwsock32 -lws2_32

default:
	$(CPP) $(INCDIR) $(LIBDIR) $(DEF) $(OBJDIR)/ircinterface.o $(OBJDIR)/ircsocket.o $(OBJDIR)/userinterface.o $(OBJDIR)/configinterface.o $(SRCDIR)/efirc.cpp -o $(BINDIR)/efirc ${LIB}

all: ircinterface userinterface configinterface default

ircsocket:
	$(CPP) $(INCDIR) $(DEF) -c $(SRCDIR)/ircsocket.cpp -o $(OBJDIR)/ircsocket.o

ircinterface: ircsocket
	$(CPP) $(INCDIR) $(DEF) -c $(SRCDIR)/irc.cpp -o $(OBJDIR)/ircinterface.o

userinterface:
	$(CPP) $(INCDIR) $(DEF) -c $(SRCDIR)/ui.cpp -o $(OBJDIR)/userinterface.o

configinterface:
	$(CPP) $(INCDIR) -c $(SRCDIR)/conf.cpp -o $(OBJDIR)/configinterface.o
