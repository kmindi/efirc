OBJS= irc.o ui.o thread.o irc_event_functions.o
CXXFLAGS+= -Lcontrib/cppirc -Iinclude -Icontrib/cppirc
LDLIBS= -lircsocket -lsstring -mwindows -lwxmsw28 -lwsock32 -luuid -lcomctl32 -lole32 -loleaut32

efirc.exe: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(@) zentrale.cpp $(LDLIBS)
