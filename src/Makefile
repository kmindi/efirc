OBJS= irc.o ui.o thread.o config.o irc_event_functions.o
CXXFLAGS+= -Lcontrib/cppirc -Iinclude -Icontrib/cppirc
LDLIBS= -lircinterface -lsstring -mwindows -lwxmsw28 -lwsock32 -luuid -lcomctl32 -lole32 -loleaut32

efirc.exe: $(OBJS) zentrale.cpp contrib/cppirc/libircinterface.a contrib/cppirc/libsstring.a
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(@) zentrale.cpp $(LDLIBS)

irc.o irc_event_functions.o: contrib/cppirc/ircinterface.h
